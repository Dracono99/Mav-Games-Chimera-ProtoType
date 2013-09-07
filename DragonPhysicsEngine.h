#pragma once

#include "BaseGameEntity.h"
#include <vector>
#include "DragonCollisionEngine.h"
#include "DragonForceEngine.h"
#include "MeshCollisionOctreePartitioner.h"

class DragonPhysicsEngine
{
private:
	bool m_bCollisionDetectionOn;
	bool m_bCollisionResolutionOn;
	MeshOctreePartitioner* m_MeshPartitioner;
	std::vector<BaseGameEntity*> m_vEntities;
	DragonForceEngine* m_cDragonForceEngine;
	DragonCollisionEngine* m_cCollisionEngine;
public:
	DragonPhysicsEngine();
	~DragonPhysicsEngine();
	void turnOffCollisionDetection()
	{
		m_bCollisionDetectionOn=false;
	}
	void turnOnCollisionDetection()
	{
		m_bCollisionDetectionOn=true;
	}
	void turnCollisionResolutionOff()
	{
		m_bCollisionResolutionOn=false;
	}
	void turnCollisionResolutionOn()
	{
		m_bCollisionResolutionOn=true;
	}
	void update(real dt);
	void takeInEntities(std::vector<BaseGameEntity*> entities);
	void updateListOfEntities(std::vector<BaseGameEntity*>& entities,real dt);
	std::vector<DragonContact*> UpdateVectorOfEntitiesAndReturnVectorOfContacts(std::vector<BaseGameEntity*> entities,real dt);
	void ResolveContacts(real dt);
	void addEntity(BaseGameEntity* newEntity);
	void clearEntities();
	void RemoveEntity(BaseGameEntity* entity);
	DragonForceEngine* GetDragonForceEngine()
	{
		return m_cDragonForceEngine;
	}
	std::vector<DragonXVector3*> GetVertexPositionsFromMesh(ID3DXMesh* mesh);
	std::vector<DragonXVector3*> GetIndeciesFromMesh(ID3DXMesh* mesh);
	void UpdateListOfEntitiesAndGenerateContacts(std::vector<BaseGameEntity*>& entities,real dt);
	std::vector<DragonicPair*> GetContactPairs();
	std::vector<DragonContact*> GetContactsByPairOfEntities(BaseGameEntity* obj1, BaseGameEntity* obj2);
	std::vector<DragonContact*> GetRayContacts()
	{
		return m_cCollisionEngine->getGenerator()->registry->m_RayContacts;
	}
	void GetTris(std::vector<DragonXVector3*>& verts,std::vector<DragonXVector3*>& indexes,std::vector<DragonTriangle*>& tris);
	void SetUpMesh(DragonMeshCollisionObject* d,ID3DXMesh* mesh);
	std::vector<DragonContact*> GetContacts()
	{
		return m_cCollisionEngine->getGenerator()->registry->m_vContacts;
	}
	inline void ClearContacts()
	{
		m_cCollisionEngine->ClearContacts();
	}
};
