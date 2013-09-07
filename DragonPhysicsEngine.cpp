#include "DragonPhysicsEngine.h"
#include "BasePhysicsEntity.h"
#include "VertexDeclarations.h"

DragonPhysicsEngine::DragonPhysicsEngine()
{
	m_bCollisionDetectionOn=true;
	m_bCollisionResolutionOn=true;
	m_cCollisionEngine=new DragonCollisionEngine();
	m_cDragonForceEngine=new DragonForceEngine();
	m_MeshPartitioner=new MeshOctreePartitioner();
}
DragonPhysicsEngine::~DragonPhysicsEngine()
{
	delete m_cCollisionEngine;
	delete m_cDragonForceEngine;
	delete m_MeshPartitioner;
}
void DragonPhysicsEngine::update(real dt)
{
	for(const auto BaseGameEntity : m_vEntities)
	{
		BaseGameEntity->GetPhysicsPointer()->Update(dt);
	}
	m_cCollisionEngine->GenerateContacts(m_vEntities);
	m_cCollisionEngine->ResolveContacts(dt);
}
void DragonPhysicsEngine::takeInEntities(std::vector<BaseGameEntity*> entities)
{
	m_vEntities=entities;
}
void DragonPhysicsEngine::updateListOfEntities(std::vector<BaseGameEntity*>& entities,real dt)
{
	m_cDragonForceEngine->updateForces(dt);
	for(const auto BaseGameEntity : entities)
	{
		BaseGameEntity->GetPhysicsPointer()->Update(dt);
	}
	if(m_bCollisionDetectionOn&&m_bCollisionResolutionOn)
	{
		m_cCollisionEngine->GenerateContacts(entities);
		m_cCollisionEngine->ResolveContacts(dt);
	}
	else if (m_bCollisionDetectionOn&&!m_bCollisionResolutionOn)
	{
		m_cCollisionEngine->GenerateContacts(entities);
	}
}
void DragonPhysicsEngine::addEntity(BaseGameEntity* newEntity)
{
	m_vEntities.push_back(newEntity);
}
void DragonPhysicsEngine::clearEntities()
{
	m_vEntities.clear();
}
void DragonPhysicsEngine::RemoveEntity(BaseGameEntity* entity)
{
}
std::vector<DragonContact*> DragonPhysicsEngine::UpdateVectorOfEntitiesAndReturnVectorOfContacts(std::vector<BaseGameEntity*> entities,real dt)
{
	m_cDragonForceEngine->updateForces(dt);
	for(const auto BaseGameEntity : entities)
	{
		BaseGameEntity->GetPhysicsPointer()->Update(dt);
	}
	return m_cCollisionEngine->GetListOfContactsFromList(entities);
}
void DragonPhysicsEngine::ResolveContacts(real dt)
{
	m_cCollisionEngine->ResolveContacts(dt);
}

std::vector<DragonXVector3*> DragonPhysicsEngine::GetVertexPositionsFromMesh(ID3DXMesh* mesh)
{
	std::vector<DragonXVector3*> vPos;
	LPDIRECT3DVERTEXBUFFER9 pVBuf;
	if (SUCCEEDED(mesh->GetVertexBuffer(&pVBuf)))
	{
		VertexPNT *pVert;
		if (SUCCEEDED(pVBuf->Lock(0,0,(void **) &pVert,D3DLOCK_DISCARD)))
		{
			DWORD numVerts=mesh->GetNumVertices();
			for (int i=0;(unsigned)i<numVerts;i++)
			{
				DragonXVector3* v = new DragonXVector3();
				*v=pVert[i].pos;
				vPos.push_back(v);
			}
			pVBuf->Unlock();
		}
		pVBuf->Release();
	}
	return vPos;
}
std::vector<DragonXVector3*> DragonPhysicsEngine::GetIndeciesFromMesh(ID3DXMesh* mesh)
{
	std::vector<DragonXVector3*> vPos;
	LPDIRECT3DINDEXBUFFER9 pIBuf;
	if (SUCCEEDED(mesh->GetIndexBuffer(&pIBuf)))
	{
		WORD* pInd;
		DWORD numFaces=mesh->GetNumFaces();
		D3DINDEXBUFFER_DESC desc;
		pIBuf->GetDesc(&desc);
		if (SUCCEEDED(pIBuf->Lock(0,0,(void **) &pInd,D3DLOCK_DISCARD)))
		{
			for(int i = 0; (unsigned)i<numFaces*3;i+=3)
			{
				DragonXVector3* p = new DragonXVector3(pInd[i],pInd[i+1],pInd[i+2]);
				vPos.push_back(p);
			}
			pIBuf->Unlock();
		}
		pIBuf->Release();
	}
	return vPos;
}

void DragonPhysicsEngine::UpdateListOfEntitiesAndGenerateContacts(std::vector<BaseGameEntity*>& entities,real dt)
{
	m_cDragonForceEngine->updateForces(dt);
	for(const auto BaseGameEntity : entities)
	{
		BaseGameEntity->GetPhysicsPointer()->Update(dt);
	}
	m_cCollisionEngine->GenerateContacts(entities);
}
std::vector<DragonicPair*> DragonPhysicsEngine::GetContactPairs()
{
	return m_cCollisionEngine->getGenerator()->registry->m_vContactPairs;
}
std::vector<DragonContact*> DragonPhysicsEngine::GetContactsByPairOfEntities(BaseGameEntity* obj1, BaseGameEntity* obj2)
{
	std::vector<DragonContact*> ret;
	for(const auto DragonContact : m_cCollisionEngine->getGenerator()->registry->m_vContacts)
	{
		if(DragonContact->m_aEntitiesContacted[0]==obj1||DragonContact->m_aEntitiesContacted[1]==obj1)
		{
			if(DragonContact->m_aEntitiesContacted[0]==obj2||DragonContact->m_aEntitiesContacted[1]==obj2)
			{
				ret.push_back(DragonContact);
			}
		}
	}
	return ret;
}
void DragonPhysicsEngine::GetTris(std::vector<DragonXVector3*>& verts,std::vector<DragonXVector3*>& indexes,std::vector<DragonTriangle*>& tris)
{
	for (int i = 0;(unsigned)i<indexes.size();i++)
	{
		DragonTriangle* t = new DragonTriangle(*verts[(unsigned)indexes[i]->x],*verts[(unsigned)indexes[i]->y],*verts[(unsigned)indexes[i]->z]);
		tris.push_back(t);
	}
	for (auto DragonXVector3 : verts)
	{
		delete DragonXVector3;
		DragonXVector3=nullptr;
	}
	verts.clear();
	for (auto DragonXVector3 : indexes)
	{
		delete DragonXVector3;
		DragonXVector3=nullptr;
	}
	indexes.clear();
}
void DragonPhysicsEngine::SetUpMesh(DragonMeshCollisionObject* d,ID3DXMesh* mesh)
{
	std::vector<DragonTriangle*> tris;
	GetTris(GetVertexPositionsFromMesh(mesh),GetIndeciesFromMesh(mesh),tris);
	d->m_DragonTris=tris;
	m_MeshPartitioner->Create(tris,tris.size());
	m_MeshPartitioner->GetContacts(d->m_DragonSectors,m_MeshPartitioner);
	d->setHalfSizes(m_MeshPartitioner->m_HalfExtents);
	d->SetRadius(m_MeshPartitioner->m_HalfExtents.GetMagnitude());
	d->CalcInverseInertialTensor();
	m_MeshPartitioner->Release();
	tris.clear();
}