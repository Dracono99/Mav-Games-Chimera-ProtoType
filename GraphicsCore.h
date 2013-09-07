#pragma once
#include "d3dUtility.h"
#include "BaseGameEntity.h"
#include <map>

struct GraphicsSchematic
{
	std::string mName;
	std::string mMeshFileName;
	std::string mMeshDiffuseTexName;
	std::string mMeshNormalTexName;
	std::string mMeshSpecTexName;
};

struct AdvancedGraphicsSchematic
{
	std::string mName;
	std::string mMeshFileName;
	std::string mUndamagedDiffuseTexName;
	std::string mUndamagedNormalTexName;
	std::string mUndamagedSpecTexName;
	std::string mDamagedDiffuseTexName;
	std::string mDamagedNormalTexName;
	std::string mDamagedSpecTexName;
};

class GraphicsCore
{
private:
	RECT mWindowDimensions;
	std::map<std::string, GraphicsSchematic> m_GraphicsSchematicManager;
	std::map<std::string, IDirect3DTexture9*> m_GraphicsTexturesManager;
	std::map<std::string, ID3DXMesh*> m_GraphicsMeshesManager;
	IDirect3DTexture9* mRenderTargetTexture;
	IDirect3DTexture9* mImposterTexture;
	IDirect3DTexture9* mImposterNormal;
	IDirect3DTexture9* mImposterSpec;
	LPDIRECT3DSURFACE9 RTTrenderSurface;
	LPDIRECT3DSURFACE9 ImposterSurface;
	LPDIRECT3DSURFACE9 BackBuffer;
	IDirect3DVertexBuffer9* m_vertBuffer;
	IDirect3DIndexBuffer9* m_indexBuffer;
	ID3DXEffect* mFX;
	int m_NumOfQuads;
	BaseGameEntity* m_ScreenQuad;
	D3DXHANDLE mhPhongTech;
	D3DXHANDLE mhImpostersTech;
	D3DXHANDLE mhInstanceTech;
	D3DXHANDLE mhInstanceViewProjMat;
	D3DXHANDLE mhInstanceTranslationVec;
	D3DXHANDLE mhDmgShdrVertNormTech;
	D3DXHANDLE mhDmgShdrVertMagTech;
	D3DXHANDLE mhDmgShdrTexBlendTech;
	D3DXHANDLE mhDmgShadrFinalRenderTech;
	D3DXHANDLE mhDmgShdrUseDmgMaps;
	D3DXHANDLE mhDamageSphere;
	D3DXHANDLE mhVertNormTex;
	D3DXHANDLE mhVertMagTex;
	D3DXHANDLE mhUndamagedTex;
	D3DXHANDLE mhDamagedTex;
	D3DXHANDLE mhDynamicShadowTech;
	D3DXHANDLE mhPostProcessTech;
	D3DXHANDLE mhDecalTechnique;
	D3DXHANDLE mhDecalWVPHandle;
	D3DXHANDLE mhDiffuseTexHandle;
	D3DXHANDLE mhNormalTexHandle;
	D3DXHANDLE mhSpecTexHandle;
	D3DXHANDLE mhWorldViewMatrix;
	D3DXHANDLE mhRenderTargetTexHandle;
	D3DXHANDLE mhWorldViewProjectionMatrixHandle;
	D3DXHANDLE mhWorldTransformMatrixHandle;
	D3DXHANDLE mhInverseWorldMtrxHandle;
	D3DXHANDLE mhCameraPosHandle;
	D3DXHANDLE mhSpecPowerHandle;
	D3DXHANDLE mhLightDirectionHandle;
	D3DXHANDLE mhInverseViewPortWidth;
	D3DXHANDLE mhInverseViewPortHeight;
	D3DXHANDLE mhPostProcessEffect;
	D3DXMATRIX mProjectionMatrix;
	D3DXMATRIX mRenderTextProjectionMatrix;
	int m_postProcessEffect;
	float m_SpecPower;
private:
	void BuildProjectionMatrix();
	void ResetRTTtex();
public:
	D3DXVECTOR3 mLightDirection;
	bool m_UpdateImposters;
	bool m_RenderShadows;
public:
	GraphicsCore();
	~GraphicsCore();
	void CreateGraphicsObject(BaseGameEntity* entity,GraphicsSchematic schem);
	void CreateGraphicsObjectWithRegisteredSchematic(BaseGameEntity* entity,std::string schemName);
	void RegisterGraphicsSchematic(GraphicsSchematic schem);
	void CreateAdvancedGraphicsObject(BaseGameEntity* entity,AdvancedGraphicsSchematic ags);
	ID3DXMesh* CreateCollisionMesh(std::string meshFileName);
	IDirect3DTexture9* RegisterTexture(std::string texName);
	inline void SetPostProcessEffect(int effect)
	{
		m_postProcessEffect = effect;
	}
	void OnLost();
	void OnReset();
	void Render(std::vector<BaseGameEntity*>& entities,D3DXMATRIX& viewMatrix,D3DXVECTOR3& camPos);
};