#pragma once

#include "DragonXMath.h"
#include "DragonEnums.h"
#include <vector>
#include "DragonMeshCollisionObject.h"

class MeshOctreePartitioner
{
protected:
	static int						g_iCurrentNodeLevel;	// class level variable used to keep from going too deep
	int								m_iNumObjects;			// the number of entities in the tree
	std::vector<DragonTriangle*>	m_vEntities;			// vector of entities in the tree
	DragonXVector3					m_vCenter;				// center of the tree

	bool							m_bIsNode;				// bool to tell whether or not its a node or leaf true 4 node false 4 leaf
	MeshOctreePartitioner*			m_pHydraNodes[8];		// the 8 node array used if its a node
private:

public:
	DragonXVector3					m_HalfExtents;			// half extents
public:

	MeshOctreePartitioner()//default constructor sets everything to null or 0
	{
		m_iNumObjects=0;
		m_vCenter=DragonXVector3(0.0f,0.0f,0.0f);
		m_HalfExtents=DragonXVector3(0.0f,0.0f,0.0f);
		ZeroMemory(m_pHydraNodes, sizeof(m_pHydraNodes));
	}
	~MeshOctreePartitioner();
	void Release();
	void Create(std::vector<DragonTriangle*>& entities,int numEntities);		//call create to set up the tree
	void SetData(std::vector<DragonTriangle*>& entities,int numEntities);		//sets up the center of the tree and its range
	void CreateNode(std::vector<DragonTriangle*>& entities,int numEntities,DragonXVector3& center, DragonXVector3& HalfExtents);	// creates nodes
	void CreateNodeEnd(std::vector<DragonTriangle*>& entities,int numEntities,bool* pBools,DragonXVector3& center,DragonXVector3& HalfExtents,int numEntitiesInLeaf,int whichNode);//when a new node is created this sets up all its data
	DragonXVector3 GetNodeCenter(DragonXVector3& currentCenter, DragonXVector3& HalfExtents, int whichNode);			//helper function calculates the center of the new node
	void SetNode(std::vector<DragonTriangle*>& entities, int numEntities);		// sets up the node assigning its entities into its vector
	void GetContacts(std::vector<DragonSector*>& mesh, MeshOctreePartitioner* pNode);	// call this to generate contacts using the partitioned space
};