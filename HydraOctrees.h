#pragma once

#include "DragonXMath.h"
#include "BaseGameEntity.h"
#include "DragonEnums.h"
#include <vector>
#include "DragonContactGenerator.h"

class HydraOctree
{
protected:
	static int						g_iCurrentNodeLevel;	// class level variable used to keep from going too deep
	int								m_iNumObjects;			// the number of entities in the tree
	std::vector<BaseGameEntity*>	m_vEntities;			// vector of entities in the tree
	DragonXVector3					m_vCenter;				// center of the tree
	real							m_rDiameter;			// diameter of the tree
	bool							m_bIsNode;				// bool to tell whether or not its a node or leaf true 4 node false 4 leaf
	HydraOctree*					m_pHydraNodes[8];		// the 8 node array used if its a node
private:

public:
	HydraOctree()//default constructor sets everything to null or 0
	{
		m_iNumObjects=0;
		m_vCenter=DragonXVector3(0.0f,0.0f,0.0f);
		m_rDiameter=0.0f;
		ZeroMemory(m_pHydraNodes, sizeof(m_pHydraNodes));
	}
	~HydraOctree();
	void Release();
	void Create(std::vector<BaseGameEntity*>& entities,int numEntities);		//call create to set up the tree
	void SetData(std::vector<BaseGameEntity*>& entities,int numEntities);		//sets up the center of the tree and its range
	void CreateNode(std::vector<BaseGameEntity*>& entities,int numEntities,DragonXVector3& center, real diameter);	// creates nodes
	void CreateNodeEnd(std::vector<BaseGameEntity*>& entities,int numEntities,bool* pBools,DragonXVector3& center,real diameter,int numEntitiesInLeaf,int whichNode);//when a new node is created this sets up all its data
	DragonXVector3 GetNodeCenter(DragonXVector3& currentCenter, real diameter, int whichNode);			//helper function calculates the center of the new node
	void SetNode(std::vector<BaseGameEntity*>& entities, int numEntities);		// sets up the node assigning its entities into its vector
	void GetContacts(DragonContactGenerator& contactGen, HydraOctree* pNode);	// call this to generate contacts using the partitioned space
};