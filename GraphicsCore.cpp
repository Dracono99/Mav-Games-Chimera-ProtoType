#include "GraphicsCore.h"
#include "BaseGraphicsEntity.h"
#include "MainGameManager.h"
#include "VertexDeclarations.h"
#include "BaseGameplayObject.h"
#include "HitEffectGamePlayObject.h"
#include "AdvancedGraphicsObject.h"
#include "InputCore.h"
#include "ChimeraGameEngine.h"
//#include "d3dUtility.h"

GraphicsCore::GraphicsCore()
{
	m_RenderShadows=false;
	BackBuffer=NULL;
	ImposterSurface=NULL;
	m_UpdateImposters=false;
	InitAllVertexDeclarations();
	GraphicsSchematic screenQuad;
	screenQuad.mName = "ScreenQuad";
	screenQuad.mMeshFileName = "ScreenAllignedQuadFull.X";
	screenQuad.mMeshDiffuseTexName = "NULL";
	screenQuad.mMeshNormalTexName = "NULL";
	screenQuad.mMeshSpecTexName = "NULL";
	m_ScreenQuad = new BaseGameEntity();
	m_NumOfQuads=1;
	CreateGraphicsObject(m_ScreenQuad,screenQuad);
	m_ScreenQuad->SetBasePhysicsEntity(NULL);
	m_SpecPower=0.3f;
	m_postProcessEffect=0;
	ID3DXBuffer* errors = 0;
	gd3dDevice->SetVertexDeclaration(VertexPCI::Decl);
	gd3dDevice->CreateVertexBuffer(4 * m_NumOfQuads * sizeof(VertexPCI), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_vertBuffer, NULL);
	VertexPCI *pVert;
	if (SUCCEEDED(m_vertBuffer->Lock(0,0,(void **) &pVert,D3DLOCK_DISCARD)))
	{
		for (int i=0;(signed)i<m_NumOfQuads;i++)
		{
			for(int q = 0; q < 4; q++)
			{
				pVert[i+q].pos.x=q*10.0f;
				pVert[i+q].pos.y=q*-10.0f;
				pVert[i+q].pos.z=q*10.0f;
				pVert[i+q].indx.x=(float)i;
				pVert[i+q].indx.y=0.0f;
			}
		}
		m_vertBuffer->Unlock();
	}
	gd3dDevice->CreateIndexBuffer(2 * 3 * m_NumOfQuads * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_indexBuffer, NULL);
	WORD* indicies=NULL;
	m_indexBuffer->Lock( 0, 0, (void**)&indicies, 0);
	//bottom
	for(int q = 0; q < 2 * 3 * m_NumOfQuads; q+=6)
	{
		indicies[q] = q;  indicies[q+1] = q+1;  indicies[q+2] = 2;
		indicies[q+3] = q+2;  indicies[q+4] = q+3;  indicies[q+5] = q;
	}
	m_indexBuffer->Unlock();

	HR(D3DXCreateEffectFromFile(gd3dDevice, "PhongBasedLighting4.fx",
		0, 0,D3DXSHADER_DEBUG, 0, &mFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	// Obtain handles.
	mhPhongTech = mFX->GetTechniqueByName("ColorTech");
	mhDynamicShadowTech = mFX->GetTechniqueByName("DynamicShadows");
	mhDmgShdrVertNormTech = mFX->GetTechniqueByName("DmgShaderVertNormQuat");
	mhDmgShdrVertMagTech = mFX->GetTechniqueByName("DmgShaderVertDspMag");
	mhDmgShdrTexBlendTech = mFX->GetTechniqueByName("DmgShaderTexMapBlend");
	mhDmgShadrFinalRenderTech = mFX->GetTechniqueByName("DmgShaderFinalRender");
	mhInstanceTech = mFX->GetTechniqueByName("InstanceQuads");
	mFX->SetTechnique(mhPhongTech);
	mhPostProcessTech = mFX->GetTechniqueByName("PostProcess");
	mhImpostersTech = mFX->GetTechniqueByName("Imposters");
	mhDecalTechnique = mFX->GetTechniqueByName("DecalProjection");
	mhDecalWVPHandle = mFX->GetParameterByName(0,"DecalWVP");
	mhWorldTransformMatrixHandle = mFX->GetParameterByName(0,"gWorldTransformMatrix");
	mhWorldViewProjectionMatrixHandle = mFX->GetParameterByName(0, "ViewProjMatrix");
	mhDiffuseTexHandle = mFX->GetParameterByName(0, "gTex");
	mhDmgShdrUseDmgMaps = mFX->GetParameterByName(0, "UseDmgMaps");
	mhInstanceViewProjMat = mFX->GetParameterByName(0, "InstanceViewProjectionMatrix");
	mhInstanceTranslationVec = mFX->GetParameterByName(0, "InstanceTranslationVector");
	mhDamageSphere = mFX->GetParameterByName(0, "gDamageSphere");
	mhVertNormTex = mFX->GetParameterByName(0, "gVertNormQuatTex");
	mhVertMagTex = mFX->GetParameterByName(0, "gVertDispMagTex");
	mhUndamagedTex = mFX->GetParameterByName(0, "gUndamagedTex");
	mhDamagedTex = mFX->GetParameterByName(0, "gDamagedTex");
	mhNormalTexHandle = mFX->GetParameterByName(0, "gTexNorm");
	mhSpecTexHandle = mFX->GetParameterByName(0, "gTexSpec");
	mhRenderTargetTexHandle = mFX->GetParameterByName(0, "gRttTex");
	mhLightDirectionHandle = mFX->GetParameterByName(0, "LightDirection");
	mhInverseWorldMtrxHandle = mFX->GetParameterByName(0,"gWorldInverse");
	mhSpecPowerHandle = mFX->GetParameterByName(0,"specPower");
	mhCameraPosHandle = mFX->GetParameterByName(0,"camPos");
	mhInverseViewPortWidth = mFX->GetParameterByName(0,"InverseViewportWidth");
	mhInverseViewPortHeight = mFX->GetParameterByName(0,"InverseViewportHeight");
	mhPostProcessEffect = mFX->GetParameterByName(0,"postProcessEffect");
	HR(mFX->SetValue(mhSpecPowerHandle,&m_SpecPower,sizeof(float)));
	GetClientRect(gd3dApp->getMainWnd(),&mWindowDimensions);
	HR(mFX->SetFloat(mhInverseViewPortWidth,(1.0f/((float)mWindowDimensions.right-(float)mWindowDimensions.left))));
	HR(mFX->SetFloat(mhInverseViewPortHeight,(1.0f/((float)mWindowDimensions.bottom-(float)mWindowDimensions.top))));
	unsigned width = (unsigned)((float)mWindowDimensions.right-(float)mWindowDimensions.left);
	unsigned height = (unsigned)((float)mWindowDimensions.bottom-(float)mWindowDimensions.top);
	HRESULT val;
	val = gd3dDevice->CreateTexture(width,
		height,
		1,D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,
		&mRenderTargetTexture,NULL);
	val = mRenderTargetTexture->GetSurfaceLevel(0,&RTTrenderSurface);
	///*gd3dDevice->CreateTexture((UINT)((float)mWindowDimensions.right-(float)mWindowDimensions.left),
	//	(UINT)((float)mWindowDimensions.bottom-(float)mWindowDimensions.top),
	//	1,D3DUSAGE_RENDERTARGET,
	//	D3DFMT_R5G6B5,D3DPOOL_DEFAULT,
	//	&mImposterTexture,NULL);
	//mImposterTexture->GetSurfaceLevel(0,&ImposterSurface);*/
	BuildProjectionMatrix();
}

GraphicsCore::~GraphicsCore()
{
	delete m_ScreenQuad;
	for(auto meshes : m_GraphicsMeshesManager)
	{
		meshes.second->Release();
	}
	for(auto tex : m_GraphicsTexturesManager)
	{
		tex.second->Release();
	}
	DestroyAllVertexDeclarations();
	m_vertBuffer->Release();
	m_indexBuffer->Release();
}

void GraphicsCore::CreateGraphicsObject(BaseGameEntity* entity,GraphicsSchematic schem)
{
	BaseGraphicsEntity* g=new BaseGraphicsEntity(entity);
	entity->SetBaseGraphicsEntity(g);

	auto res = m_GraphicsSchematicManager.find(schem.mName);
	if(res==m_GraphicsSchematicManager.end())
	{
		m_GraphicsSchematicManager.insert(make_pair(schem.mName,schem));

		auto res1 = m_GraphicsMeshesManager.find(schem.mMeshFileName);
		if (res1==m_GraphicsMeshesManager.end())
		{
			ID3DXMesh* meshSys      = 0;
			ID3DXBuffer* adjBuffer  = 0;
			ID3DXBuffer* mtrlBuffer = 0;
			DWORD numMtrls          = 0;
			//""""BattleCruiserShieldCollisionMesh.XDragonMeshTestPyramid.XBattleCruiserTightCollisionMesh.XTestBoxMesh.Xbattle_cruiser_1_TightShieldCollisionMesh.X
			HRESULT hr = (D3DXLoadMeshFromX(schem.mMeshFileName.c_str(),
				D3DXMESH_SYSTEMMEM,
				gd3dDevice,
				&adjBuffer,
				&mtrlBuffer,
				NULL,
				NULL,
				&meshSys));

			D3DVERTEXELEMENT9 elements[MAX_FVF_DECL_SIZE];
			UINT numElements = 0;
			VertexPTBNT::Decl->GetDeclaration(elements, &numElements);

			ID3DXMesh* temp = 0;
			HR(meshSys->CloneMesh(D3DXMESH_SYSTEMMEM,
				elements, gd3dDevice, &temp));
			ReleaseCOM(meshSys);
			meshSys = temp;

			HR(D3DXComputeTangentFrameEx(temp,D3DDECLUSAGE_TEXCOORD,0,
				D3DDECLUSAGE_BINORMAL,0,
				D3DDECLUSAGE_TANGENT,0,
				D3DDECLUSAGE_NORMAL,0,
				0,
				0,
				0.1f,0.25f,0.01f,
				&meshSys,
				0));

			HR(meshSys->Optimize(D3DXMESH_MANAGED |
				D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
				(DWORD*)adjBuffer->GetBufferPointer(), 0, 0, 0, &g->mMesh));
			ReleaseCOM(meshSys); // Done w/ system mesh.
			ReleaseCOM(adjBuffer); // Done with buffer.
			m_GraphicsMeshesManager.insert(make_pair(schem.mMeshFileName,g->mMesh));
		}
		if (schem.mMeshDiffuseTexName!="NULL")
		{
			IDirect3DTexture9* tempTex;
			auto res2 = m_GraphicsTexturesManager.find(schem.mMeshDiffuseTexName);
			if (res2==m_GraphicsTexturesManager.end())
			{
				D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshDiffuseTexName.c_str(),&tempTex);
				m_GraphicsTexturesManager.insert(make_pair(schem.mMeshDiffuseTexName,tempTex));
				g->mDiffuse=tempTex;
			}
		}
		if (schem.mMeshNormalTexName!="NULL")
		{
			auto res2 = m_GraphicsTexturesManager.find(schem.mMeshNormalTexName);
			if (res2==m_GraphicsTexturesManager.end())
			{
				D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshNormalTexName.c_str(),&g->mNormal);
				m_GraphicsTexturesManager.insert(make_pair(schem.mMeshNormalTexName,g->mNormal));
			}
		}
		if (schem.mMeshSpecTexName!="NULL")
		{
			auto res2 = m_GraphicsTexturesManager.find(schem.mMeshSpecTexName);
			if (res2==m_GraphicsTexturesManager.end())
			{
				D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshSpecTexName.c_str(),&g->mSpec);
				m_GraphicsTexturesManager.insert(make_pair(schem.mMeshSpecTexName,g->mSpec));
			}
		}
	}
	else
	{
		g->mMesh = m_GraphicsMeshesManager.find(schem.mMeshFileName)->second;
		if (schem.mMeshDiffuseTexName!="NULL")
		{
			IDirect3DTexture9* tempTex;
			auto res2 = m_GraphicsTexturesManager.find(schem.mMeshDiffuseTexName);
			if (res2==m_GraphicsTexturesManager.end())
			{
				D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshDiffuseTexName.c_str(),&tempTex);
				m_GraphicsTexturesManager.insert(make_pair(schem.mMeshDiffuseTexName,tempTex));
				g->mDiffuse=tempTex;
			}
			else
			{
				g->mDiffuse=res2->second;
			}
		}
		if (schem.mMeshNormalTexName!="NULL")
		{
			auto res2 = m_GraphicsTexturesManager.find(schem.mMeshNormalTexName);
			if (res2==m_GraphicsTexturesManager.end())
			{
				D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshNormalTexName.c_str(),&g->mNormal);
				m_GraphicsTexturesManager.insert(make_pair(schem.mMeshNormalTexName,g->mNormal));
			}
			else
			{
				g->mNormal=res2->second;
			}
		}
		if (schem.mMeshSpecTexName!="NULL")
		{
			auto res2 = m_GraphicsTexturesManager.find(schem.mMeshSpecTexName);
			if (res2==m_GraphicsTexturesManager.end())
			{
				D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshSpecTexName.c_str(),&g->mSpec);
				m_GraphicsTexturesManager.insert(make_pair(schem.mMeshSpecTexName,g->mSpec));
			}
			else
			{
				g->mSpec=res2->second;
			}
		}
	}
}
void GraphicsCore::CreateAdvancedGraphicsObject(BaseGameEntity* entity,AdvancedGraphicsSchematic ags)
{
	AdvancedGraphicsObject* g=new AdvancedGraphicsObject(entity);
	entity->SetBaseGraphicsEntity(g);

	/*auto res = m_GraphicsSchematicManager.find(ags.mName);
	if(res==m_GraphicsSchematicManager.end())
	{*/
	//m_GraphicsSchematicManager.insert(make_pair(ags.mName,ags));

	auto res1 = m_GraphicsMeshesManager.find(ags.mMeshFileName);
	if (res1==m_GraphicsMeshesManager.end())
	{
		ID3DXMesh* meshSys      = 0;
		ID3DXBuffer* adjBuffer  = 0;
		ID3DXBuffer* mtrlBuffer = 0;
		DWORD numMtrls          = 0;
		//""""BattleCruiserShieldCollisionMesh.XDragonMeshTestPyramid.XBattleCruiserTightCollisionMesh.XTestBoxMesh.Xbattle_cruiser_1_TightShieldCollisionMesh.X
		HRESULT hr = (D3DXLoadMeshFromX(ags.mMeshFileName.c_str(),
			D3DXMESH_SYSTEMMEM,
			gd3dDevice,
			&adjBuffer,
			&mtrlBuffer,
			NULL,
			NULL,
			&meshSys));

		D3DVERTEXELEMENT9 elements[MAX_FVF_DECL_SIZE];
		UINT numElements = 0;
		VertexPTBNT::Decl->GetDeclaration(elements, &numElements);

		ID3DXMesh* temp = 0;
		HR(meshSys->CloneMesh(D3DXMESH_SYSTEMMEM,
			elements, gd3dDevice, &temp));
		ReleaseCOM(meshSys);
		meshSys = temp;

		HR(D3DXComputeTangentFrameEx(temp,D3DDECLUSAGE_TEXCOORD,0,
			D3DDECLUSAGE_BINORMAL,0,
			D3DDECLUSAGE_TANGENT,0,
			D3DDECLUSAGE_NORMAL,0,
			0,
			0,
			0.1f,0.25f,0.01f,
			&meshSys,
			0));

		HR(meshSys->Optimize(D3DXMESH_MANAGED |
			D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
			(DWORD*)adjBuffer->GetBufferPointer(), 0, 0, 0, &g->mMesh));
		ReleaseCOM(meshSys); // Done w/ system mesh.
		ReleaseCOM(adjBuffer); // Done with buffer.
		m_GraphicsMeshesManager.insert(make_pair(ags.mMeshFileName,g->mMesh));
	}
	if (ags.mUndamagedDiffuseTexName!="NULL")
	{
		IDirect3DTexture9* tempTex;
		auto res2 = m_GraphicsTexturesManager.find(ags.mUndamagedDiffuseTexName);
		if (res2==m_GraphicsTexturesManager.end())
		{
			D3DXCreateTextureFromFile(gd3dDevice,ags.mUndamagedDiffuseTexName.c_str(),&tempTex);
			m_GraphicsTexturesManager.insert(make_pair(ags.mUndamagedDiffuseTexName,tempTex));
			g->m_UnDamagedDiffuseTex=tempTex;
		}
		else
		{
			g->m_UnDamagedDiffuseTex=res2->second;
		}
	}
	if (ags.mUndamagedNormalTexName!="NULL")
	{
		IDirect3DTexture9* tempTex;
		auto res2 = m_GraphicsTexturesManager.find(ags.mUndamagedNormalTexName);
		if (res2==m_GraphicsTexturesManager.end())
		{
			D3DXCreateTextureFromFile(gd3dDevice,ags.mUndamagedNormalTexName.c_str(),&tempTex);
			m_GraphicsTexturesManager.insert(make_pair(ags.mUndamagedNormalTexName,tempTex));
			g->m_UnDamagedNormTex=tempTex;
		}
		else
		{
			g->m_UnDamagedNormTex=res2->second;
		}
	}
	if (ags.mUndamagedSpecTexName!="NULL")
	{
		IDirect3DTexture9* tempTex;
		auto res2 = m_GraphicsTexturesManager.find(ags.mUndamagedSpecTexName);
		if (res2==m_GraphicsTexturesManager.end())
		{
			D3DXCreateTextureFromFile(gd3dDevice,ags.mUndamagedSpecTexName.c_str(),&tempTex);
			m_GraphicsTexturesManager.insert(make_pair(ags.mUndamagedSpecTexName,tempTex));
			g->m_UnDamagedSpecTex=tempTex;
		}
		else
		{
			g->m_UnDamagedSpecTex=res2->second;
		}
	}
	if (ags.mDamagedDiffuseTexName!="NULL")
	{
		IDirect3DTexture9* tempTex;
		auto res2 = m_GraphicsTexturesManager.find(ags.mDamagedDiffuseTexName);
		if (res2==m_GraphicsTexturesManager.end())
		{
			D3DXCreateTextureFromFile(gd3dDevice,ags.mDamagedDiffuseTexName.c_str(),&tempTex);
			m_GraphicsTexturesManager.insert(make_pair(ags.mDamagedDiffuseTexName,tempTex));
			g->m_DamagedDiffuseTex=tempTex;
		}
		else
		{
			g->m_DamagedDiffuseTex=res2->second;
		}
	}
	if (ags.mDamagedNormalTexName!="NULL")
	{
		IDirect3DTexture9* tempTex;
		auto res2 = m_GraphicsTexturesManager.find(ags.mDamagedNormalTexName);
		if (res2==m_GraphicsTexturesManager.end())
		{
			D3DXCreateTextureFromFile(gd3dDevice,ags.mDamagedNormalTexName.c_str(),&tempTex);
			m_GraphicsTexturesManager.insert(make_pair(ags.mDamagedNormalTexName,tempTex));
			g->m_DamagedNormalTex=tempTex;
		}
		else
		{
			g->m_DamagedNormalTex=res2->second;
		}
	}
	if (ags.mDamagedSpecTexName!="NULL")
	{
		IDirect3DTexture9* tempTex;
		auto res2 = m_GraphicsTexturesManager.find(ags.mDamagedSpecTexName);
		if (res2==m_GraphicsTexturesManager.end())
		{
			D3DXCreateTextureFromFile(gd3dDevice,ags.mDamagedSpecTexName.c_str(),&tempTex);
			m_GraphicsTexturesManager.insert(make_pair(ags.mDamagedSpecTexName,tempTex));
			g->m_DamagedSpecTex=tempTex;
		}
		else
		{
			g->m_DamagedSpecTex=res2->second;
		}
	}
	//D3DFMT_A8R8G8B8
	HRESULT val;
	val = gd3dDevice->CreateTexture((UINT)2048.0f,
		(UINT)2048.0f,
		1,D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,
		&g->m_RenderedDiffuseTex,NULL);
	g->m_RenderedDiffuseTex->GetSurfaceLevel(0,&g->m_RenderedDiffuseSurface);

	val = gd3dDevice->CreateTexture((UINT)2048.0f,
		(UINT)2048.0f,
		1,D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,
		&g->m_RenderedNormTex,NULL);
	g->m_RenderedNormTex->GetSurfaceLevel(0,&g->m_RenderedNormSurface);

	val = gd3dDevice->CreateTexture((UINT)2048.0f,
		(UINT)2048.0f,
		1,D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,
		&g->m_RenderedSpecTex,NULL);
	g->m_RenderedSpecTex->GetSurfaceLevel(0,&g->m_RenderedSpecSurface);

	val = gd3dDevice->CreateTexture((UINT)2048.0f,
		(UINT)2048.0f,
		1,D3DUSAGE_RENDERTARGET,
		D3DFMT_A32B32G32R32F,D3DPOOL_DEFAULT,
		&g->m_VertexDispMagTex,NULL);
	g->m_VertexDispMagTex->GetSurfaceLevel(0,&g->m_VertexDispMagSurface);

	val = gd3dDevice->CreateTexture((UINT)2048.0f,
		(UINT)2048.0f,
		1,D3DUSAGE_RENDERTARGET,
		D3DFMT_A32B32G32R32F,D3DPOOL_DEFAULT,
		&g->m_VertexNormQuatTex,NULL);
	g->m_VertexNormQuatTex->GetSurfaceLevel(0,&g->m_VertexNormQuatSurface);

	/*}
	else
	{
	g->mMesh = m_GraphicsMeshesManager.find(schem.mMeshFileName)->second;
	if (schem.mMeshDiffuseTexName!="NULL")
	{
	IDirect3DTexture9* tempTex;
	auto res2 = m_GraphicsTexturesManager.find(schem.mMeshDiffuseTexName);
	if (res2==m_GraphicsTexturesManager.end())
	{
	D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshDiffuseTexName.c_str(),&tempTex);
	m_GraphicsTexturesManager.insert(make_pair(schem.mMeshDiffuseTexName,tempTex));
	g->mDiffuse=tempTex;
	}
	else
	{
	g->mDiffuse=res2->second;
	}
	}
	if (schem.mMeshNormalTexName!="NULL")
	{
	auto res2 = m_GraphicsTexturesManager.find(schem.mMeshNormalTexName);
	if (res2==m_GraphicsTexturesManager.end())
	{
	D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshNormalTexName.c_str(),&g->mNormal);
	m_GraphicsTexturesManager.insert(make_pair(schem.mMeshNormalTexName,g->mNormal));
	}
	else
	{
	g->mNormal=res2->second;
	}
	}
	if (schem.mMeshSpecTexName!="NULL")
	{
	auto res2 = m_GraphicsTexturesManager.find(schem.mMeshSpecTexName);
	if (res2==m_GraphicsTexturesManager.end())
	{
	D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshSpecTexName.c_str(),&g->mSpec);
	m_GraphicsTexturesManager.insert(make_pair(schem.mMeshSpecTexName,g->mSpec));
	}
	else
	{
	g->mSpec=res2->second;
	}
	}
	}*/
}
IDirect3DTexture9* GraphicsCore::RegisterTexture(std::string texName)
{
	IDirect3DTexture9* tempTex;
	auto res2 = m_GraphicsTexturesManager.find(texName);
	if (res2==m_GraphicsTexturesManager.end())
	{
		D3DXCreateTextureFromFile(gd3dDevice,texName.c_str(),&tempTex);
		m_GraphicsTexturesManager.insert(make_pair(texName,tempTex));
		return tempTex;
	}
	else
	{
		return res2->second;
	}
}
void GraphicsCore::CreateGraphicsObjectWithRegisteredSchematic(BaseGameEntity* entity,std::string schemName)
{
	GraphicsSchematic temp = m_GraphicsSchematicManager.find(schemName)->second;
	BaseGraphicsEntity* g=new BaseGraphicsEntity(entity);
	entity->SetBaseGraphicsEntity(g);
	g->mMesh=m_GraphicsMeshesManager.find(temp.mMeshFileName)->second;
	g->mDiffuse=m_GraphicsTexturesManager.find(temp.mMeshDiffuseTexName)->second;
	g->mNormal=m_GraphicsTexturesManager.find(temp.mMeshNormalTexName)->second;
	g->mSpec=m_GraphicsTexturesManager.find(temp.mMeshSpecTexName)->second;
}
ID3DXMesh* GraphicsCore::CreateCollisionMesh(std::string meshFileName)
{
	ID3DXMesh* tempMesh;
	auto res1 = m_GraphicsMeshesManager.find(meshFileName);
	if (res1==m_GraphicsMeshesManager.end())
	{
		ID3DXMesh* meshSys      = 0;
		ID3DXBuffer* adjBuffer  = 0;
		ID3DXBuffer* mtrlBuffer = 0;
		DWORD numMtrls          = 0;
		//""""BattleCruiserShieldCollisionMesh.XDragonMeshTestPyramid.XBattleCruiserTightCollisionMesh.XTestBoxMesh.Xbattle_cruiser_1_TightShieldCollisionMesh.X
		HRESULT hr = (D3DXLoadMeshFromX(meshFileName.c_str(),
			D3DXMESH_SYSTEMMEM,
			gd3dDevice,
			&adjBuffer,
			&mtrlBuffer,
			NULL,
			NULL,
			&meshSys));

		D3DVERTEXELEMENT9 elements[64];
		UINT numElements = 0;
		VertexPNT::Decl->GetDeclaration(elements, &numElements);

		ID3DXMesh* temp = 0;
		HR(meshSys->CloneMesh(D3DXMESH_SYSTEMMEM,
			elements, gd3dDevice, &temp));
		ReleaseCOM(meshSys);
		meshSys = temp;

		/*	HR(D3DXComputeTangentFrameEx(temp,D3DDECLUSAGE_TEXCOORD,0,
		D3DDECLUSAGE_BINORMAL,0,
		D3DDECLUSAGE_TANGENT,0,
		D3DDECLUSAGE_NORMAL,0,
		0,
		0,
		0.1f,0.25f,0.01f,
		&meshSys,
		0));*/

		HR(meshSys->Optimize(D3DXMESH_MANAGED |
			D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
			(DWORD*)adjBuffer->GetBufferPointer(), 0, 0, 0, &tempMesh));
		ReleaseCOM(meshSys); // Done w/ system mesh.
		ReleaseCOM(adjBuffer); // Done with buffer.
		m_GraphicsMeshesManager.insert(make_pair(meshFileName,tempMesh));
		return tempMesh;
	}
	else
	{
		tempMesh=res1->second;
		return tempMesh;
	}
}
void GraphicsCore::RegisterGraphicsSchematic(GraphicsSchematic schem)
{
	auto res = m_GraphicsSchematicManager.find(schem.mName);
	if(res==m_GraphicsSchematicManager.end())
	{
		m_GraphicsSchematicManager.insert(make_pair(schem.mName,schem));

		auto res1 = m_GraphicsMeshesManager.find(schem.mMeshFileName);
		if (res1==m_GraphicsMeshesManager.end())
		{
			ID3DXMesh* tempM;
			ID3DXMesh* meshSys      = 0;
			ID3DXBuffer* adjBuffer  = 0;
			ID3DXBuffer* mtrlBuffer = 0;
			DWORD numMtrls          = 0;
			//""""BattleCruiserShieldCollisionMesh.XDragonMeshTestPyramid.XBattleCruiserTightCollisionMesh.XTestBoxMesh.Xbattle_cruiser_1_TightShieldCollisionMesh.X
			HRESULT hr = (D3DXLoadMeshFromX(schem.mMeshFileName.c_str(),
				D3DXMESH_SYSTEMMEM,
				gd3dDevice,
				&adjBuffer,
				&mtrlBuffer,
				NULL,
				NULL,
				&meshSys));

			D3DVERTEXELEMENT9 elements[64];
			UINT numElements = 0;
			VertexPTBNT::Decl->GetDeclaration(elements, &numElements);

			ID3DXMesh* temp = 0;
			HR(meshSys->CloneMesh(D3DXMESH_SYSTEMMEM,
				elements, gd3dDevice, &temp));
			ReleaseCOM(meshSys);
			meshSys = temp;

			HR(D3DXComputeTangentFrameEx(temp,D3DDECLUSAGE_TEXCOORD,0,
				D3DDECLUSAGE_BINORMAL,0,
				D3DDECLUSAGE_TANGENT,0,
				D3DDECLUSAGE_NORMAL,0,
				0,
				0,
				0.1f,0.25f,0.01f,
				&meshSys,
				0));

			HR(meshSys->Optimize(D3DXMESH_MANAGED |
				D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
				(DWORD*)adjBuffer->GetBufferPointer(), 0, 0, 0, &tempM));
			ReleaseCOM(meshSys); // Done w/ system mesh.
			ReleaseCOM(adjBuffer); // Done with buffer.
			m_GraphicsMeshesManager.insert(make_pair(schem.mMeshFileName,tempM));
		}
		if (schem.mMeshDiffuseTexName!="NULL")
		{
			IDirect3DTexture9* tempTex;
			auto res2 = m_GraphicsTexturesManager.find(schem.mMeshDiffuseTexName);
			if (res2==m_GraphicsTexturesManager.end())
			{
				D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshDiffuseTexName.c_str(),&tempTex);
				m_GraphicsTexturesManager.insert(make_pair(schem.mMeshDiffuseTexName,tempTex));
			}
		}
		if (schem.mMeshNormalTexName!="NULL")
		{
			IDirect3DTexture9* tempTex;
			auto res2 = m_GraphicsTexturesManager.find(schem.mMeshNormalTexName);
			if (res2==m_GraphicsTexturesManager.end())
			{
				D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshNormalTexName.c_str(),&tempTex);
				m_GraphicsTexturesManager.insert(make_pair(schem.mMeshNormalTexName,tempTex));
			}
		}
		if (schem.mMeshSpecTexName!="NULL")
		{
			IDirect3DTexture9* tempTex;
			auto res2 = m_GraphicsTexturesManager.find(schem.mMeshSpecTexName);
			if (res2==m_GraphicsTexturesManager.end())
			{
				D3DXCreateTextureFromFile(gd3dDevice,schem.mMeshSpecTexName.c_str(),&tempTex);
				m_GraphicsTexturesManager.insert(make_pair(schem.mMeshSpecTexName,tempTex));
			}
		}
	}
}
void GraphicsCore::OnLost()
{
	RTTrenderSurface->Release();
	mRenderTargetTexture->Release();
	if (BackBuffer)
	{
		BackBuffer->Release();
	}
	mFX->OnLostDevice();
}
void GraphicsCore::OnReset()
{
	ResetRTTtex();
	BuildProjectionMatrix();
	mFX->OnResetDevice();
}
void GraphicsCore::Render(std::vector<BaseGameEntity*>& entities,D3DXMATRIX& viewMatrix,D3DXVECTOR3& camPos)
{
	HR(gd3dDevice->GetRenderTarget(0,&BackBuffer));
	for(auto entity :entities)
	{
		if(entity->GetGraphicsPointer()->m_AdvancedGfxObj)
		{
			AdvancedGraphicsObject* advgo = static_cast<AdvancedGraphicsObject*>(entity->GetGraphicsPointer());
			if(advgo->m_Damaged)
			{
				HR(mFX->SetInt(mhDmgShdrUseDmgMaps,1));
				if (entity->GetGamePlayObject()->m_HitEffectApplied)
				{
					// begin vert normal calculation
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_VertexNormQuatSurface));
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDmgShdrVertNormTech);
					IDirect3DTexture9* temp;
					HR(gd3dDevice->CreateTexture((UINT)2048.0f,
						(UINT)2048.0f,
						1,D3DUSAGE_RENDERTARGET,
						D3DFMT_A32B32G32R32F,D3DPOOL_DEFAULT,
						&temp,NULL));
					LPDIRECT3DSURFACE9 tempS;
					temp->GetSurfaceLevel(0,&tempS);
					RECT tempTexRect;
					tempTexRect.top=(long)0;
					tempTexRect.bottom=(long)2048;
					tempTexRect.right=(long)2048;
					tempTexRect.left=(long)0;
					gd3dDevice->StretchRect(advgo->m_VertexNormQuatSurface,&tempTexRect,tempS,&tempTexRect,D3DTEXF_NONE);
					HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					HR(mFX->SetTexture(mhVertNormTex,temp));
					D3DXVECTOR3 dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					UINT numPasses = 0;
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end vert normal calc

					//begin vert disp mag calc
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_VertexDispMagSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);					
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDmgShdrVertMagTech);
					gd3dDevice->StretchRect(advgo->m_VertexDispMagSurface,NULL,tempS,NULL,D3DTEXF_NONE);
					HR(mFX->SetTexture(mhVertMagTex,temp));
					HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(255,0,0,0), 1.0f, 0));// clear buffer
					//HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					numPasses = 0;
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());

					// end vert disp mag calc

					// begin diffuse map calc
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedDiffuseSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDmgShdrTexBlendTech);
					gd3dDevice->StretchRect(advgo->m_RenderedDiffuseSurface,NULL,tempS,NULL,D3DTEXF_NONE);
					//HR(mFX->SetTexture(mhVertMagTex,temp));
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedDiffuseTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					HR(mFX->SetTexture(mhDamagedTex, temp));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					//D3DXSaveTextureToFile("combinedDiffuseTexture",D3DXIFF_BMP,advgo->m_RenderedDiffuseTex,NULL);
					// end diffuse map calc

					// begin norm map calc
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedNormSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(255,255,255,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDmgShdrTexBlendTech);
					gd3dDevice->StretchRect(advgo->m_RenderedNormSurface,NULL,tempS,NULL,D3DTEXF_NONE);
					//HR(mFX->SetTexture(mhVertMagTex,temp));
					HR(mFX->SetTexture(mhDamagedTex, temp));
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedNormTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					//HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedNormalTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end norm map calc

					// begin spec map calc
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedSpecSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDmgShdrTexBlendTech);
					gd3dDevice->StretchRect(advgo->m_RenderedSpecSurface,NULL,tempS,NULL,D3DTEXF_NONE);
					//HR(mFX->SetTexture(mhVertMagTex,temp));
					HR(mFX->SetTexture(mhDamagedTex, temp));
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedSpecTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					//HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedSpecTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end spec map calc

					// begin decal spec application
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedSpecSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDecalTechnique);						
					HR(mFX->SetTexture(mhDiffuseTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjDiffuseTex));
					//HR(mFX->SetTexture(mhNormalTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjNormalTex));
					HR(mFX->SetTexture(mhSpecTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjSpecTex));					
					//HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedSpecTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedSpecTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
						D3DXMATRIX W;
						D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
						// Set up world matrix
						D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
						D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
						D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
						D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
						D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
						D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
						D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
						D3DXMATRIX wit;
						D3DXMatrixInverse(&wit,0,&W);
						HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
						HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end decal spec cal n application

					// begin decal spec application
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedNormSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDecalTechnique);					
					//D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
					HR(mFX->SetTexture(mhDiffuseTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjDiffuseTex));
					//HR(mFX->SetTexture(mhNormalTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjNormalTex));
					HR(mFX->SetTexture(mhSpecTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjNormalTex));
					//D3DXMATRIX W;
					//D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
					//// Set up world matrix
					//D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
					//D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
					//D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
					//D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
					//D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
					//D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
					//D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
					//D3DXMATRIX wit;
					//D3DXMatrixInverse(&wit,0,&W);
					//HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
					//HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
					//HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedSpecTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedSpecTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
						D3DXMATRIX W;
						D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
						// Set up world matrix
						D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
						D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
						D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
						D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
						D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
						D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
						D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
						D3DXMATRIX wit;
						D3DXMatrixInverse(&wit,0,&W);
						HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
						HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end decal spec cal n application

					// begin decal spec application
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedDiffuseSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDecalTechnique);					
					//D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
					HR(mFX->SetTexture(mhDiffuseTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjDiffuseTex));
					//HR(mFX->SetTexture(mhNormalTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjNormalTex));
					HR(mFX->SetTexture(mhSpecTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjDiffuseTex));
					//D3DXMATRIX W;
					//D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
					//// Set up world matrix
					//D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
					//D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
					//D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
					//D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
					//D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
					//D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
					//D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
					//D3DXMATRIX wit;
					//D3DXMatrixInverse(&wit,0,&W);
					//HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
					//HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
					//HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedSpecTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedSpecTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
						D3DXMATRIX W;
						D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
						// Set up world matrix
						D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
						D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
						D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
						D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
						D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
						D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
						D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
						D3DXMATRIX wit;
						D3DXMatrixInverse(&wit,0,&W);
						HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
						HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end decal spec cal n application

					entity->GetGamePlayObject()->HitEffectApplied();
					temp->Release();
					tempS->Release();
				}
			}
			else
			{
				HR(mFX->SetInt(mhDmgShdrUseDmgMaps,0));
				if (entity->GetGamePlayObject()->m_HitEffectApplied)
				{
					// begin vert normal calculation
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_VertexNormQuatSurface));
					IDirect3DTexture9* temp;
					HR(gd3dDevice->CreateTexture((UINT)2048.0f,
						(UINT)2048.0f,
						1,D3DUSAGE_RENDERTARGET,
						D3DFMT_A32B32G32R32F,D3DPOOL_DEFAULT,
						&temp,NULL));
					LPDIRECT3DSURFACE9 tempS;
					temp->GetSurfaceLevel(0,&tempS);
					RECT tempTexRect;
					tempTexRect.top=(long)0;
					tempTexRect.bottom=(long)2048;
					tempTexRect.right=(long)2048;
					tempTexRect.left=(long)0;
					gd3dDevice->StretchRect(advgo->m_VertexNormQuatSurface,&tempTexRect,tempS,&tempTexRect,D3DTEXF_NONE);
					HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,1), 1.0f, 0));// clear buffer
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(mFX->SetTexture(mhVertNormTex,temp));
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDmgShdrVertNormTech);
					//HR(mFX->SetTexture(mhVertNormTex, advgo->m_VertexNormQuatTex));
					HR(mFX->SetTexture(mhVertNormTex,temp));
					D3DXVECTOR3 dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					//HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(50.0f,50.0f,50.0f,10.0f))));
					UINT numPasses = 0;
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end vert normal calc
					//D3DXSaveTextureToFile("combinedDiffuseTexture",D3DXIFF_BMP,advgo->m_VertexNormQuatTex,NULL);

					// begin vert disp mag calc
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_VertexDispMagSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);

					HR(gd3dDevice->BeginScene());//begin scene
					gd3dDevice->StretchRect(advgo->m_VertexDispMagSurface,NULL,tempS,NULL,D3DTEXF_NONE);
					HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(255,0,0,0), 1.0f, 0));// clear buffer
					mFX->SetTechnique(mhDmgShdrVertMagTech);
					HR(mFX->SetTexture(mhVertMagTex, temp));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					//HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(50.0f,50.0f,50.0f,10.0f))));
					numPasses = 0;
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());

					// end vert disp mag calc

					//// begin diffuse map calc
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedDiffuseSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(255,0,0,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDmgShdrTexBlendTech);
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedDiffuseTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedDiffuseTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					//HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(50.0f,50.0f,50.0f,10.0f))));
					numPasses = 0;
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					//// end diffuse map calc

					// begin norm map calc
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedNormSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(255,255,255,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDmgShdrTexBlendTech);
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedNormTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedNormalTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					//HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(50.0f,50.0f,50.0f,10.0f))));
					numPasses = 0;
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end norm map calc

					// begin spec map calc
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedSpecSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(255,255,255,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDmgShdrTexBlendTech);
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedSpecTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedSpecTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					//HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(50.0f,50.0f,50.0f,10.0f))));
					numPasses = 0;
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end spec map calc

					// begin decal spec application
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedSpecSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDecalTechnique);					
					//D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
					HR(mFX->SetTexture(mhDiffuseTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjDiffuseTex));
					//HR(mFX->SetTexture(mhNormalTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjNormalTex));
					HR(mFX->SetTexture(mhSpecTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjSpecTex));
					//D3DXMATRIX W;
					//D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
					//// Set up world matrix
					//D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
					//D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
					//D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
					//D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
					//D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
					//D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
					//D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
					//D3DXMATRIX wit;
					//D3DXMatrixInverse(&wit,0,&W);
					//HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
					//HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
					//HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedSpecTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedSpecTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
						D3DXMATRIX W;
						D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
						// Set up world matrix
						D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
						D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
						D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
						D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
						D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
						D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
						D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
						D3DXMATRIX wit;
						D3DXMatrixInverse(&wit,0,&W);
						HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
						HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end decal spec cal n application

					// begin decal spec application
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedNormSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDecalTechnique);					
					//D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
					HR(mFX->SetTexture(mhDiffuseTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjDiffuseTex));
					//HR(mFX->SetTexture(mhNormalTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjNormalTex));
					HR(mFX->SetTexture(mhSpecTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjNormalTex));
					//D3DXMATRIX W;
					//D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
					//// Set up world matrix
					//D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
					//D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
					//D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
					//D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
					//D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
					//D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
					//D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
					//D3DXMATRIX wit;
					//D3DXMatrixInverse(&wit,0,&W);
					//HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
					//HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
					//HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedSpecTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedSpecTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
						D3DXMATRIX W;
						D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
						// Set up world matrix
						D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
						D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
						D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
						D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
						D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
						D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
						D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
						D3DXMATRIX wit;
						D3DXMatrixInverse(&wit,0,&W);
						HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
						HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end decal spec cal n application

					// begin decal spec application
					HR(gd3dDevice->SetRenderTarget(0,advgo->m_RenderedDiffuseSurface));
					gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
					gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
					gd3dDevice->SetRenderState(D3DRS_CLIPPING,false);
					//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
					HR(gd3dDevice->BeginScene());//begin scene
					mFX->SetTechnique(mhDecalTechnique);					
					//D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
					HR(mFX->SetTexture(mhDiffuseTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjDiffuseTex));
					//HR(mFX->SetTexture(mhNormalTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjNormalTex));
					HR(mFX->SetTexture(mhSpecTexHandle, entity->GetGamePlayObject()->m_HitFX->m_ProjDiffuseTex));
					//D3DXMATRIX W;
					//D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
					//// Set up world matrix
					//D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
					//D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
					//D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
					//D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
					//D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
					//D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
					//D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
					//D3DXMATRIX wit;
					//D3DXMatrixInverse(&wit,0,&W);
					//HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
					//HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
					//HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
					HR(mFX->SetTexture(mhUndamagedTex, advgo->m_UnDamagedSpecTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));
					HR(mFX->SetTexture(mhDamagedTex, advgo->m_DamagedSpecTex));
					dmgsphrPos = entity->GetGamePlayObject()->m_HitFX->m_DecalPosition;
					HR(mFX->SetVector(mhDamageSphere,&(D3DXVECTOR4(dmgsphrPos.x,dmgsphrPos.y,dmgsphrPos.z,20.0f))));
					HR(mFX->Begin(&numPasses, 0));
					for(UINT i = 0; i < numPasses; ++i)
					{
						HR(mFX->BeginPass(i));
						D3DXMATRIX DecalviewProjectMat = entity->GetGamePlayObject()->m_HitFX->GetViewProjectionMatrix(entity)*entity->GetGamePlayObject()->m_HitFX->m_ProjectionMtrx;
						D3DXMATRIX W;
						D3DXVECTOR3 trans(entity->GetPhysicsPointer()->m_Position);
						// Set up world matrix
						D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
						D3DXQUATERNION* rot = &entity->GetPhysicsPointer()->m_QuatRot;
						D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
						D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
						D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
						D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
						D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
						D3DXMATRIX wit;
						D3DXMatrixInverse(&wit,0,&W);
						HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
						HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
						mFX->CommitChanges();
						entity->GetGraphicsPointer()->mMesh->DrawSubset(0);
						HR(mFX->EndPass());
					}
					HR(mFX->End());
					HR(gd3dDevice->EndScene());
					// end decal spec cal n application
					advgo->m_Damaged=true;
					entity->GetGamePlayObject()->HitEffectApplied();
					temp->Release();
					tempS->Release();
				}
			}
		}
	}

	HR(gd3dDevice->SetRenderTarget(0,RTTrenderSurface));

	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
	gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	gd3dDevice->SetRenderState(D3DRS_ZENABLE,true);
	gd3dDevice->SetRenderState(D3DRS_CLIPPING,true);
	if(m_RenderShadows)
	{
		HR(gd3dDevice->BeginScene());//begin scene

		// Setup the rendering FX
		mFX->SetTechnique(mhDynamicShadowTech);
		HR(gd3dDevice->SetRenderTarget(0,RTTrenderSurface));
		D3DXVECTOR4 ty = D3DXVECTOR4(mLightDirection.x,mLightDirection.y,mLightDirection.z,0.0f);
		HR(mFX->SetVector(mhLightDirectionHandle, &ty));
		HR(mFX->SetVector(mhCameraPosHandle,&(D3DXVECTOR4(camPos.x,camPos.y,camPos.z,0.0f))));
		D3DXMATRIX viewProjectMat = viewMatrix * mProjectionMatrix;

		// Begin passes.
		UINT numPasses = 0;
		HR(mFX->Begin(&numPasses, 0));
		for(UINT i = 0; i < numPasses; ++i)
		{
			HR(mFX->BeginPass(i));
			for( const auto BaseGameEntity : entities)
			{
				if (BaseGameEntity->GetGamePlayObject()!=NULL)
				{
					if (BaseGameEntity->GetGamePlayObject()->m_HitEffectApplicaple)
					{
						D3DXMATRIX W;
						D3DXVECTOR3 trans(BaseGameEntity->GetPhysicsPointer()->m_Position);
						// Set up world matrix
						D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
						D3DXQUATERNION* rot = &BaseGameEntity->GetPhysicsPointer()->m_QuatRot;
						D3DXMatrixRotationQuaternion( &rotationMatrix, rot);

						D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
						D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);

						D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
						D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
						D3DXMATRIX wit;
						D3DXMatrixInverse(&wit,0,&W);
						HR(mFX->SetMatrix(mhWorldTransformMatrixHandle,&W));
						//HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
						HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(viewProjectMat)));
						mFX->CommitChanges();
						BaseGameEntity->GetGraphicsPointer()->mMesh->DrawSubset(0);
					}
				}
			}
			HR(mFX->EndPass());
		}
		HR(mFX->End());
		HR(gd3dDevice->EndScene());
	}
	HR(gd3dDevice->BeginScene());//begin scene
	// Setup the rendering FX
	mFX->SetTechnique(mhPhongTech);
	D3DXVECTOR4 ty = D3DXVECTOR4(mLightDirection.x,mLightDirection.y,mLightDirection.z,0.0f);
	HR(mFX->SetVector(mhLightDirectionHandle, &ty));
	HR(mFX->SetVector(mhCameraPosHandle,&(D3DXVECTOR4(camPos.x,camPos.y,camPos.z,0.0f))));
	//HR(mFX->SetValue(mhFVLightPosition, &m_camera->GetPosition(), sizeof(D3DXVECTOR3)));
	D3DXMATRIX viewProjectMat = viewMatrix * mProjectionMatrix;

	// Begin passes.
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	for(UINT i = 0; i < numPasses; ++i)
	{
		HR(mFX->BeginPass(i));
		for( const auto BaseGameEntity : entities)
		{
			if (BaseGameEntity->GetGraphicsPointer()->m_AdvancedGfxObj==false)
			{
				HR(mFX->SetTexture(mhDiffuseTexHandle, BaseGameEntity->GetGraphicsPointer()->mDiffuse));
				HR(mFX->SetTexture(mhNormalTexHandle, BaseGameEntity->GetGraphicsPointer()->mNormal));
				HR(mFX->SetTexture(mhSpecTexHandle, BaseGameEntity->GetGraphicsPointer()->mSpec));

				D3DXMATRIX W;
				D3DXVECTOR3 trans(BaseGameEntity->GetPhysicsPointer()->m_Position);
				// Set up world matrix
				D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
				D3DXQUATERNION* rot = &BaseGameEntity->GetPhysicsPointer()->m_QuatRot;
				D3DXMatrixRotationQuaternion( &rotationMatrix, rot);

				D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
				D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);

				D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
				D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
				D3DXMATRIX wit;
				D3DXMatrixInverse(&wit,0,&W);
				//D3DXMatrixTranspose(&wit,&wit);
				HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
				HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
				mFX->CommitChanges();
				BaseGameEntity->GetGraphicsPointer()->mMesh->DrawSubset(0);
			}
			else
			{
				AdvancedGraphicsObject* advgo = static_cast<AdvancedGraphicsObject*>(BaseGameEntity->GetGraphicsPointer());
				if (!advgo->m_Damaged)
				{
					HR(mFX->SetTexture(mhDiffuseTexHandle, advgo->m_UnDamagedDiffuseTex));
					HR(mFX->SetTexture(mhNormalTexHandle, advgo->m_UnDamagedNormTex));
					HR(mFX->SetTexture(mhSpecTexHandle, advgo->m_UnDamagedSpecTex));

					D3DXMATRIX W;
					D3DXVECTOR3 trans(BaseGameEntity->GetPhysicsPointer()->m_Position);
					// Set up world matrix
					D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
					D3DXQUATERNION* rot = &BaseGameEntity->GetPhysicsPointer()->m_QuatRot;
					D3DXMatrixRotationQuaternion( &rotationMatrix, rot);

					D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
					D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);

					D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
					D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
					D3DXMATRIX wit;
					D3DXMatrixInverse(&wit,0,&W);
					//D3DXMatrixTranspose(&wit,&wit);
					HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
					HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
					mFX->CommitChanges();
					BaseGameEntity->GetGraphicsPointer()->mMesh->DrawSubset(0);
				}
			}
		}
		HR(mFX->EndPass());
	}
	HR(mFX->End());
	HR(gd3dDevice->EndScene());

	HR(gd3dDevice->BeginScene());//begin scene
	// Setup the rendering FX
	mFX->SetTechnique(mhDmgShadrFinalRenderTech);
	HR(gd3dDevice->SetRenderTarget(0,RTTrenderSurface));
	ty = D3DXVECTOR4(mLightDirection.x,mLightDirection.y,mLightDirection.z,0.0f);
	HR(mFX->SetVector(mhLightDirectionHandle, &ty));
	HR(mFX->SetVector(mhCameraPosHandle,&(D3DXVECTOR4(camPos.x,camPos.y,camPos.z,0.0f))));
	//HR(mFX->SetValue(mhFVLightPosition, &m_camera->GetPosition(), sizeof(D3DXVECTOR3)));
	viewProjectMat = viewMatrix * mProjectionMatrix;

	// Begin passes.
	numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	for(UINT i = 0; i < numPasses; ++i)
	{
		HR(mFX->BeginPass(i));
		for( const auto BaseGameEntity : entities)
		{
			if (BaseGameEntity->GetGraphicsPointer()->m_AdvancedGfxObj)
			{
				AdvancedGraphicsObject* advgo = static_cast<AdvancedGraphicsObject*>(BaseGameEntity->GetGraphicsPointer());
				if (advgo->m_Damaged)
				{
					HR(mFX->SetTexture(mhDiffuseTexHandle,advgo->m_RenderedDiffuseTex ));
					HR(mFX->SetTexture(mhNormalTexHandle, advgo->m_RenderedNormTex));
					HR(mFX->SetTexture(mhSpecTexHandle, advgo->m_RenderedSpecTex));
					HR(mFX->SetTexture(mhVertNormTex, advgo->m_VertexNormQuatTex));
					HR(mFX->SetTexture(mhVertMagTex, advgo->m_VertexDispMagTex));

					D3DXMATRIX W;
					D3DXVECTOR3 trans(BaseGameEntity->GetPhysicsPointer()->m_Position);
					// Set up world matrix
					D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
					D3DXQUATERNION* rot = &BaseGameEntity->GetPhysicsPointer()->m_QuatRot;
					D3DXMatrixRotationQuaternion( &rotationMatrix, rot);

					D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
					D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);

					D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
					D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
					D3DXMATRIX wit;
					D3DXMatrixInverse(&wit,0,&W);
					//D3DXMatrixTranspose(&wit,&wit);
					HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
					HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
					mFX->CommitChanges();
					BaseGameEntity->GetGraphicsPointer()->mMesh->DrawSubset(0);
				}
			}
		}
		HR(mFX->EndPass());
	}
	HR(mFX->End());
	HR(gd3dDevice->EndScene());

	//HR(gd3dDevice->BeginScene());//begin scene
	//// Setup the rendering FX
	//mFX->SetTechnique(mhInstanceTech);
	//ty = D3DXVECTOR4(mLightDirection.x,mLightDirection.y,mLightDirection.z,0.0f);
	//HR(mFX->SetVector(mhLightDirectionHandle, &ty));
	//HR(mFX->SetVector(mhCameraPosHandle,&(D3DXVECTOR4(camPos.x,camPos.y,camPos.z,0.0f))));
	////HR(mFX->SetValue(mhFVLightPosition, &m_camera->GetPosition(), sizeof(D3DXVECTOR3)));
	//viewProjectMat = viewMatrix * mProjectionMatrix;
	//// Begin passes.
	//numPasses = 0;
	//HR(mFX->Begin(&numPasses, 0));
	//for(UINT i = 0; i < numPasses; ++i)
	//{
	//	HR(mFX->BeginPass(i));
	//	for(int q = 0; q < 100; q++)
	//	{
	//		//HR(mFX->SetValue(mhInstanceTranslationVec,&D3DXVECTOR3((float)q,(float)q,(float)q),sizeof(D3DXVECTOR3)));
	//		//HR(mFX->SetTexture(mhDiffuseTexHandle,mImposterTexture));
	//		D3DXMATRIX W;
	//		D3DXVECTOR3 trans((q*10.0f),(q*10.0f),(q*15.0f));
	//		// Set up world matrix
	//		D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
	//		D3DXQUATERNION* rot = &D3DXQUATERNION(0.0f,0.0f,0.0f,1.0f);
	//		D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
	//		D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
	//		D3DXMatrixScaling(&scaleMatrix,100.0f,100.0f,100.0f);
	//		D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
	//		D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
	//		D3DXMATRIX wit;
	//		D3DXMatrixInverse(&wit,0,&W);
	//		//D3DXMatrixTranspose(&wit,&wit);
	//		HR(mFX->SetMatrix(mhInstanceTranslationVec,&W));
	//		HR(mFX->SetMatrix(mhInstanceViewProjMat,&(W*viewProjectMat)));
	//		gd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	//		mFX->CommitChanges();
	//		gd3dDevice->SetVertexDeclaration(VertexPCI::Decl);
	//		gd3dDevice->SetStreamSource( 0, m_vertBuffer, 0, sizeof(VertexPCI));
	//		gd3dDevice->SetIndices( m_indexBuffer );
	//		gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4*m_NumOfQuads, 0, 2*m_NumOfQuads);
	//	}
	//	HR(mFX->EndPass());
	//}
	//HR(mFX->End());
	//HR(gd3dDevice->EndScene());
	// end technique 1
	// Setup the rendering FX
	////HR(gd3dDevice->BeginScene());//begin scene
	////if(!m_UpdateImposters)
	////{
	////	mFX->SetTechnique(mhImpostersTech);
	////	D3DXVECTOR4 ty = D3DXVECTOR4(mLightDirection.x,mLightDirection.y,mLightDirection.z,0.0f);
	////	HR(mFX->SetVector(mhLightDirectionHandle, &ty));
	////	HR(mFX->SetVector(mhCameraPosHandle,&(D3DXVECTOR4(camPos.x,camPos.y,camPos.z,0.0f))));
	////	//HR(mFX->SetValue(mhFVLightPosition, &m_camera->GetPosition(), sizeof(D3DXVECTOR3)));
	////	D3DXMATRIX viewProjectMat = viewMatrix * mProjectionMatrix;
	////	// Begin passes.
	////	UINT numPasses = 0;
	////	HR(mFX->Begin(&numPasses, 0));
	////	for(UINT i = 0; i < numPasses; ++i)
	////	{
	////		HR(mFX->BeginPass(i));
	////		for(int q = 4 ; q > 1;q--)
	////		{
	////			HR(mFX->SetTexture(mhDiffuseTexHandle,mImposterTexture));
	////			D3DXMATRIX W;
	////			D3DXVECTOR3 trans((q*100.0f),(q*100.0f),(q*150.0f));
	////			// Set up world matrix
	////			D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
	////			D3DXQUATERNION* rot = &D3DXQUATERNION(0.0f,0.0f,0.0f,1.0f);
	////			D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
	////			D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
	////			D3DXMatrixScaling(&scaleMatrix,100.0f,100.0f,100.0f);
	////			D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
	////			D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
	////			D3DXMATRIX wit;
	////			D3DXMatrixInverse(&wit,0,&W);
	////			//D3DXMatrixTranspose(&wit,&wit);
	////			HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
	////			HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
	////			mFX->CommitChanges();
	////			m_ScreenQuad->GetGraphicsPointer()->mMesh->DrawSubset(0);
	////		}
	////		HR(mFX->EndPass());
	////	}
	////	HR(mFX->End());
	////}
	////HR(gd3dDevice->EndScene());
	//HR(gd3dDevice->BeginScene());//begin scene
	// Setup the rendering FX
	//mFX->SetTechnique(mhDecalTechnique);
	//ty = D3DXVECTOR4(mLightDirection.x,mLightDirection.y,mLightDirection.z,0.0f);
	//HR(mFX->SetVector(mhLightDirectionHandle, &ty));
	//HR(mFX->SetVector(mhCameraPosHandle,&(D3DXVECTOR4(camPos.x,camPos.y,camPos.z,0.0f))));
	//viewProjectMat = viewMatrix * mProjectionMatrix;
	//// Begin passes.
	//numPasses = 0;
	//HR(mFX->Begin(&numPasses, 0));
	//for(UINT i = 0; i < numPasses; ++i)
	//{
	//	HR(mFX->BeginPass(i));
	//	for( const auto BaseGameEntity : entities)
	//	{
	//		if (BaseGameEntity->GetGamePlayObject()->m_HitEffectApplied)
	//		{
	//			for (auto hitFX : BaseGameEntity->GetGamePlayObject()->m_HitFX)
	//			{
	//				D3DXMATRIX DecalviewProjectMat = hitFX->GetViewProjectionMatrix(BaseGameEntity)*hitFX->m_ProjectionMtrx;
	//				HR(mFX->SetTexture(mhDiffuseTexHandle, hitFX->m_ProjDiffuseTex));
	//				HR(mFX->SetTexture(mhNormalTexHandle, hitFX->m_ProjNormalTex));
	//				HR(mFX->SetTexture(mhSpecTexHandle, hitFX->m_ProjSpecTex));
	//				D3DXMATRIX W;
	//				D3DXVECTOR3 trans(BaseGameEntity->GetPhysicsPointer()->m_Position);
	//				// Set up world matrix
	//				D3DXMATRIXA16 rotationMatrix, translateMatrix, scaleMatrix;
	//				D3DXQUATERNION* rot = &BaseGameEntity->GetPhysicsPointer()->m_QuatRot;
	//				D3DXMatrixRotationQuaternion( &rotationMatrix, rot);
	//				D3DXMatrixTranslation(&translateMatrix, trans.x,trans.y,trans.z);
	//				D3DXMatrixScaling(&scaleMatrix,1.0f,1.0f,1.0f);
	//				D3DXMatrixMultiply(&W, &scaleMatrix,&rotationMatrix); // M = R * S
	//				D3DXMatrixMultiply(&W, &W, &translateMatrix); // M = M * T
	//				D3DXMATRIX wit;
	//				D3DXMatrixInverse(&wit,0,&W);
	//				HR(mFX->SetMatrix(mhDecalWVPHandle,&(W*DecalviewProjectMat)));
	//				HR(mFX->SetMatrix(mhInverseWorldMtrxHandle,&wit));
	//				HR(mFX->SetMatrix(mhWorldViewProjectionMatrixHandle,&(W*viewProjectMat)));
	//				mFX->CommitChanges();
	//				BaseGameEntity->GetGraphicsPointer()->mMesh->DrawSubset(0);
	//			}
	//		}
	//	}
	//	HR(mFX->EndPass());
	//}
	//HR(mFX->End());
	//HR(gd3dDevice->EndScene());


	////if(m_UpdateImposters)
	////{
	////	//LPDIRECT3DSURFACE9 ts=NULL;
	////	//mImposterTexture->GetSurfaceLevel(0,&ImposterSurface);
	////	gd3dDevice->StretchRect(RTTrenderSurface,&mWindowDimensions,ImposterSurface,&mWindowDimensions,D3DTEXF_NONE);

	////}

	//// and begin post processing
	HR(gd3dDevice->SetRenderTarget(0,BackBuffer));
	HR(gd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true));
	gd3dDevice->SetRenderState(D3DRS_ZENABLE,false);
	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0));// clear buffer
	HR(gd3dDevice->BeginScene());//begin scene


	//// Setup the rendering FX
	mFX->SetTechnique(mhPostProcessTech);

	HR(mFX->SetTexture(mhRenderTargetTexHandle, mRenderTargetTexture));
	//D3DXSaveTextureToFile("combinedDiffuseTexture",D3DXIFF_BMP,mRenderTargetTexture,NULL);
	HR(mFX->SetInt(mhPostProcessEffect,m_postProcessEffect));
	//// Begin passes.
	numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));

	HR(mFX->BeginPass(0));

	mFX->CommitChanges();
	m_ScreenQuad->GetGraphicsPointer()->mMesh->DrawSubset(0);
	HR(mFX->EndPass());
	HR(mFX->End());

	gd3dApp->DisplayStats();
	HR(gd3dDevice->EndScene());
	// Present the back buffer.
	//HR(gd3dDevice->Present(0, 0, 0, 0));
	BackBuffer->Release();
	BackBuffer=NULL;
}
void GraphicsCore::BuildProjectionMatrix()
{
	D3DXMatrixPerspectiveFovLH(&mProjectionMatrix,
		D3DX_PI * 0.25f,    // the horizontal field of view
		((float)mWindowDimensions.right-(float)mWindowDimensions.left) / ((float)mWindowDimensions.bottom-(float)mWindowDimensions.top), // aspect ratio
		1.0f,    // the near view-plane
		300000.0f);    // the far view-plane

	D3DXMatrixPerspectiveFovLH(&mRenderTextProjectionMatrix,
		D3DX_PI * 0.35f,    // the horizontal field of view
		((float)mWindowDimensions.right-(float)mWindowDimensions.left) / ((float)mWindowDimensions.bottom-(float)mWindowDimensions.top), // aspect ratio
		1.0f,    // the near view-plane
		300000.0f);    // the far view-plane
}
void GraphicsCore::ResetRTTtex()
{
	GetClientRect(gd3dApp->getMainWnd(),&mWindowDimensions);
	HRESULT val;
	val = gd3dDevice->CreateTexture((UINT)((float)mWindowDimensions.right-(float)mWindowDimensions.left),
		(UINT)((float)mWindowDimensions.bottom-(float)mWindowDimensions.top),
		1,D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,
		&mRenderTargetTexture,NULL);
	mRenderTargetTexture->GetSurfaceLevel(0,&RTTrenderSurface);
	HR(mFX->SetFloat(mhInverseViewPortWidth,(1.0f/((float)mWindowDimensions.right-(float)mWindowDimensions.left))));
	HR(mFX->SetFloat(mhInverseViewPortHeight,(1.0f/((float)mWindowDimensions.bottom-(float)mWindowDimensions.top))));
}