#include "HydraOctrees.h"

int HydraOctree::g_iCurrentNodeLevel = 0;

HydraOctree::~HydraOctree()
{
	Release();
}
//call release when your done with  the hydra system so that you arent holding all these trees in memory
void HydraOctree::Release()
{
	if(m_bIsNode)
	{
		/*for(const auto BaseGameEntity: m_vEntities)
		{
		delete BaseGameEntity;
		}*/
		m_vEntities.clear();
	}
	for(int i=0; i<8; i++)
	{
		if(m_pHydraNodes[i])
		{
			m_pHydraNodes[i]->Release();
			delete m_pHydraNodes[i];
			m_pHydraNodes[i]=NULL;
		}
	}
}
//call create to set up the tree
void HydraOctree::Create(std::vector<BaseGameEntity*>& entities,int numEntities)
{
	m_iNumObjects=numEntities;
	SetData(entities,numEntities);
	CreateNode(entities,numEntities,m_vCenter,m_rDiameter);// recursion begins here
}
//sets up the center of the tree and its range
void HydraOctree::SetData(std::vector<BaseGameEntity*>& entities,int numEntities)
{
	// if theres no entities theres no need to make a tree
	if(entities.size()==0 || numEntities==0)
	{
		return;
	}
	// find the center of the first node
	for(const auto BaseGameEntity : entities)
	{
		if(BaseGameEntity->GetPhysicsPointer()->mI_PhysicsType!=COT_World)
		{
			m_vCenter.x = m_vCenter.x + BaseGameEntity->GetPhysicsPointer()->getPosition().x;
			m_vCenter.y = m_vCenter.y + BaseGameEntity->GetPhysicsPointer()->getPosition().y;
			m_vCenter.z = m_vCenter.z + BaseGameEntity->GetPhysicsPointer()->getPosition().z;
		}
	}
	//center of the tree or um cube well the tree is in a cube shape
	m_vCenter.x=m_vCenter.x/(real)numEntities;
	m_vCenter.y=m_vCenter.y/(real)numEntities;
	m_vCenter.z=m_vCenter.z/(real)numEntities;

	// ok now we need the diameter or i guess sorta distance from the center of the tree cube thing to determine which entities go inside it
	real wMax = 0.0f;//initialize local variable to zero we will use these to figure out our diameter
	real hMax = 0.0f;
	real dMax = 0.0f;
	// now we make some mini variables that we'll be using to help find the afore mentioned diameter
	real lWidth = 0.0f;
	real lHeight = 0.0f;
	real lDepth = 0.0f;

	// now we find the diameter
	for(const auto BaseGameEntity : entities)
	{
		if(BaseGameEntity->GetPhysicsPointer()->mI_PhysicsType!=COT_World)
		{
			lWidth = (real)abs(BaseGameEntity->GetPhysicsPointer()->getPosition().x-m_vCenter.x);
			lHeight = (real)abs(BaseGameEntity->GetPhysicsPointer()->getPosition().y-m_vCenter.y);
			lDepth = (real)abs(BaseGameEntity->GetPhysicsPointer()->getPosition().z-m_vCenter.z);

			if(lWidth>wMax)
			{
				wMax=lWidth;
			}
			if (lHeight>hMax)
			{
				hMax=lHeight;
			}
			if (lDepth>dMax)
			{
				dMax=lDepth;
			}
		}
	}
	//ok we now have the largest radii ? any way radius whatever we need to x by 2 for diameter we all know math
	wMax*=2;
	hMax*=2;
	dMax*=2;
	// ok now lets see which one is biggest which will be our winner for largest diameter
	m_rDiameter=wMax;
	if (hMax>m_rDiameter)
	{
		m_rDiameter=hMax;
	}
	if (dMax>m_rDiameter)
	{
		m_rDiameter=dMax;
	}
	// ok thats all for this method which means that this node has a center and diameter based on the vector or list of entities given
}
// creates nodes, this is called automatically when we need to go a node deeper
void HydraOctree::CreateNode(std::vector<BaseGameEntity*>& entities,int numEntities,DragonXVector3& center, real diameter)
{
	m_vCenter=center;
	m_rDiameter=diameter;

	//now for some important notes on recursion
	// one it means it calls itself
	// this is great for automated type stuff makes our lives easier
	// only one minor issue it usually doesn't know anything about the outside world or what we tell it and passing in a limit might get messy
	// so the hydra solution is a static variable the current node level tells the tree where it is as far as levels
	// now that is not its limit however
	int iMaxNodes = 50;
	// that is change it for different results
	// ill experiment around with it but that is where we set the number of levels
	// for examples sake 1 is 8 subdivisions of the original entities list 2 is 64 yes its a lot but that just means a lot less checks in
	// collision detection which those can get fairly cpu intensive even with just sphere sphere
	// now to avoid unnecessary recursion we need  a certain number of objects to exist before the system will bother with recursion
	// that number is set below
	int iMinNumberOfObjects = 64;//64 gives me a decent frame rate with friction and looks smoothest

	// now lets see what we are going to do
	if((numEntities<iMinNumberOfObjects)||(g_iCurrentNodeLevel >= iMaxNodes))//if its a node
	{
		SetNode(entities,numEntities);
		m_bIsNode=true;
	}
	else//not a node its a leaf or branch or head of hydra point is it holds more data
	{
		//oh umm this is where we sorta sort the entities just telling ya
		m_bIsNode=false;//obviously not a node its a leaf or whatever you wanna call it
		// ok now this is the clever soring bit
		// we are going to make 8 bool pointer arrays these are the same size numbers wise as the number of entities
		//we have going into this node ok well its a leaf now but the point is we are going to check
		//where each entity needs to go and use bools to say yes you go there or no you don't
		// now why bools and then later
		// its easy really sometimes you might have entities that are going to be needing
		// to be in two or more umm sub division thingys because we are dealing with things
		// not particles which means they have radii which could cross the arbitrary divisions
		// of the octree system so
		// to eliminate the possibility of not detecting
		// collisions that should be detected we use bools and pool the entities based on bools and possible collisions
		bool* pBoolArray1 = new bool[numEntities];
		bool* pBoolArray2 = new bool[numEntities];
		bool* pBoolArray3 = new bool[numEntities];
		bool* pBoolArray4 = new bool[numEntities];
		bool* pBoolArray5 = new bool[numEntities];
		bool* pBoolArray6 = new bool[numEntities];
		bool* pBoolArray7 = new bool[numEntities];
		bool* pBoolArray8 = new bool[numEntities];
		//gotta make sure theres nothing in these arrays
		// also side not sets all to null
		// or zero which when read as a boolean is false
		// so this sets these arrays to being full of falses
		ZeroMemory(pBoolArray1, numEntities);
		ZeroMemory(pBoolArray2, numEntities);
		ZeroMemory(pBoolArray3, numEntities);
		ZeroMemory(pBoolArray4, numEntities);
		ZeroMemory(pBoolArray5, numEntities);
		ZeroMemory(pBoolArray6, numEntities);
		ZeroMemory(pBoolArray7, numEntities);
		ZeroMemory(pBoolArray8, numEntities);

		// Loop through all our entities, and allocate to the appropriate
		// cube area.
		int i=0;
		for(const auto BaseGameEntity : entities)
		{
			if(BaseGameEntity->GetPhysicsPointer()->mI_PhysicsType==COT_World&&BaseGameEntity->GetPhysicsPointer()->mI_PrimitiveType==PT_Plane)
			{
				if((m_vCenter*BaseGameEntity->GetPhysicsPointer()->GetNormal())-m_rDiameter<BaseGameEntity->GetPhysicsPointer()->GetDistFromOrigin())
				{
					pBoolArray1[i] = true;
					pBoolArray2[i] = true;
					pBoolArray3[i] = true;
					pBoolArray4[i] = true;
					pBoolArray5[i] = true;
					pBoolArray6[i] = true;
					pBoolArray7[i] = true;
					pBoolArray8[i] = true;
				}
			}
			else if (BaseGameEntity->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray)
			{
				pBoolArray1[i] = true;
				pBoolArray2[i] = true;
				pBoolArray3[i] = true;
				pBoolArray4[i] = true;
				pBoolArray5[i] = true;
				pBoolArray6[i] = true;
				pBoolArray7[i] = true;
				pBoolArray8[i] = true;
			}
			else
			{
				// TOP_FRONT_LEFT
				if( (BaseGameEntity->GetPhysicsPointer()->getPosition().y+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.y) && (BaseGameEntity->GetPhysicsPointer()->getPosition().x-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.x) && (BaseGameEntity->GetPhysicsPointer()->getPosition().z-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.z) )
				{
					pBoolArray1[i] = true;
				}
				// TOP_FRONT_RIGHT
				if( (BaseGameEntity->GetPhysicsPointer()->getPosition().y+BaseGameEntity->GetPhysicsPointer()->GetRadius()>= m_vCenter.y) && (BaseGameEntity->GetPhysicsPointer()->getPosition().x+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.x) && (BaseGameEntity->GetPhysicsPointer()->getPosition().z-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.z) )
				{
					pBoolArray2[i] = true;
				}
				// TOP_BACK_LEFT
				if( (BaseGameEntity->GetPhysicsPointer()->getPosition().y+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.y) && (BaseGameEntity->GetPhysicsPointer()->getPosition().x-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.x) && (BaseGameEntity->GetPhysicsPointer()->getPosition().z+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.z) )
				{
					pBoolArray3[i] = true;
				}
				// TOP_BACK_RIGHT
				if( (BaseGameEntity->GetPhysicsPointer()->getPosition().y+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.y) && (BaseGameEntity->GetPhysicsPointer()->getPosition().x+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.x) && (BaseGameEntity->GetPhysicsPointer()->getPosition().z+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.z) )
				{
					pBoolArray4[i] = true;
				}

				// BOTTOM_FRONT_LEFT
				if( (BaseGameEntity->GetPhysicsPointer()->getPosition().y-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.y) && (BaseGameEntity->GetPhysicsPointer()->getPosition().x-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.x) && (BaseGameEntity->GetPhysicsPointer()->getPosition().z-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.z) )
				{
					pBoolArray5[i] = true;
				}
				// BOTTOM_FRONT_RIGHT
				if( (BaseGameEntity->GetPhysicsPointer()->getPosition().y-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.y) && (BaseGameEntity->GetPhysicsPointer()->getPosition().x+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.x) && (BaseGameEntity->GetPhysicsPointer()->getPosition().z-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.z) )
				{
					pBoolArray6[i] = true;
				}
				// BOTTOM_BACK_LEFT
				if( (BaseGameEntity->GetPhysicsPointer()->getPosition().y-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.y) && (BaseGameEntity->GetPhysicsPointer()->getPosition().x-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.x) && (BaseGameEntity->GetPhysicsPointer()->getPosition().z+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.z) )
				{
					pBoolArray7[i] = true;
				}
				// BOTTOM_BACK_RIGHT
				if( (BaseGameEntity->GetPhysicsPointer()->getPosition().y-BaseGameEntity->GetPhysicsPointer()->GetRadius() <= m_vCenter.y) && (BaseGameEntity->GetPhysicsPointer()->getPosition().x+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.x) && (BaseGameEntity->GetPhysicsPointer()->getPosition().z+BaseGameEntity->GetPhysicsPointer()->GetRadius() >= m_vCenter.z) )
				{
					pBoolArray8[i] = true;
				}
			}
			i++;
		}
		// now we need 2 count how many entities are in each partition this is used for early outs in this recursive process
		int iCount1 = 0;
		int iCount2 = 0;
		int iCount3 = 0;
		int iCount4 = 0;
		int iCount5 = 0;
		int iCount6 = 0;
		int iCount7 = 0;
		int iCount8 = 0;

		for(int i=0; i<numEntities; i++)
		{
			if(pBoolArray1[i]==true)
			{
				iCount1++;
			}
			if(pBoolArray2[i]==true)
			{
				iCount2++;
			}
			if(pBoolArray3[i]==true)
			{
				iCount3++;
			}
			if(pBoolArray4[i]==true)
			{
				iCount4++;
			}
			if(pBoolArray5[i]==true)
			{
				iCount5++;
			}
			if(pBoolArray6[i]==true)
			{
				iCount6++;
			}
			if(pBoolArray7[i]==true)
			{
				iCount7++;
			}
			if(pBoolArray8[i]==true)
			{
				iCount8++;
			}
		}
		// now we make each of the nodes or leafy bits or oh whatever this is where you split up the data
		CreateNodeEnd(entities, numEntities, pBoolArray1, m_vCenter, m_rDiameter, iCount1,Top_Front_Left);
		CreateNodeEnd(entities, numEntities, pBoolArray2, m_vCenter, m_rDiameter, iCount2,Top_Front_Right);
		CreateNodeEnd(entities, numEntities, pBoolArray3, m_vCenter, m_rDiameter, iCount3,Top_Back_Left);
		CreateNodeEnd(entities, numEntities, pBoolArray4, m_vCenter, m_rDiameter, iCount4,Top_Back_Right);

		CreateNodeEnd(entities, numEntities, pBoolArray5, m_vCenter, m_rDiameter, iCount5,Bottom_Front_Left);
		CreateNodeEnd(entities, numEntities, pBoolArray6, m_vCenter, m_rDiameter, iCount6,Bottom_Front_Right);
		CreateNodeEnd(entities, numEntities, pBoolArray7, m_vCenter, m_rDiameter, iCount7,Bottom_Back_Left);
		CreateNodeEnd(entities, numEntities, pBoolArray8, m_vCenter, m_rDiameter, iCount8,Bottom_Back_Right);
		delete pBoolArray1;
		delete pBoolArray2;
		delete pBoolArray3;
		delete pBoolArray4;
		delete pBoolArray5;
		delete pBoolArray6;
		delete pBoolArray7;
		delete pBoolArray8;
	}
}
//when a new node is created this sets up all its data
void HydraOctree::CreateNodeEnd(std::vector<BaseGameEntity*>& entities,int numEntities,bool* pBools,DragonXVector3& center,real diameter,int numEntitiesInNode,int whichNode)
{
	// is there any point in making a node with nothing in it
	if(numEntitiesInNode==0)
	{
		return;// no
	}
	// so we dont

	// now just make a temporary container that will be used to hold the entities that are going into this new tree
	std::vector<BaseGameEntity*> tempEntities;
	//now lets check which of the entities that were passed in need to go into this tree
	int i = 0;// an initialized counter for use with the bool pointer to help us sort
	for(const auto BaseGameEntity : entities)
	{
		if(pBools[i])
		{
			tempEntities.push_back(BaseGameEntity);
		}
		i++;
	}
	// heres where we create a new node
	m_pHydraNodes[whichNode] = new HydraOctree;

	// figure out the new center for this new node
	DragonXVector3 vNewCenter = GetNodeCenter(center,diameter,whichNode);

	g_iCurrentNodeLevel++;
	//make a new node and pass in details
	m_pHydraNodes[whichNode]->CreateNode(tempEntities,numEntitiesInNode,vNewCenter,diameter/2);

	g_iCurrentNodeLevel--;
}
//helper function calculates the center of the new node
DragonXVector3 HydraOctree::GetNodeCenter(DragonXVector3& currentCenter, real diameter, int whichNode)
{
	//ok first off we make our new center n set it 2 overs
	DragonXVector3 vNewCenter = DragonXVector3(0.0f,0.0f,0.0f);
	// now based on which part of the tree we are generating the center for we take the current center + or - the diameter over 4 that
	// gives us a center thats well centered in which ever octant? we need a center for
	switch( whichNode )
	{
	case Top_Front_Left:      // 0
		{
			vNewCenter = DragonXVector3( currentCenter.x - diameter/4, currentCenter.y + diameter/4, currentCenter.z + diameter/4 );
			break;
		}
	case Top_Front_Right:     // 1
		{
			vNewCenter = DragonXVector3( currentCenter.x + diameter/4, currentCenter.y + diameter/4, currentCenter.z + diameter/4 );
			break;
		}
	case Top_Back_Left:       // 2
		{
			vNewCenter = DragonXVector3( currentCenter.x - diameter/4, currentCenter.y + diameter/4, currentCenter.z - diameter/4 );
			break;
		}
	case Top_Back_Right:      // 3
		{
			vNewCenter = DragonXVector3( currentCenter.x + diameter/4, currentCenter.y + diameter/4, currentCenter.z - diameter/4 );
			break;
		}
	case Bottom_Front_Left:   // 4
		{
			vNewCenter = DragonXVector3( currentCenter.x - diameter/4, currentCenter.y - diameter/4, currentCenter.z + diameter/4 );
			break;
		}
	case Bottom_Front_Right:  // 5
		{
			vNewCenter = DragonXVector3( currentCenter.x + diameter/4, currentCenter.y - diameter/4, currentCenter.z + diameter/4 );
			break;
		}
	case Bottom_Back_Left:    // 6
		{
			vNewCenter = DragonXVector3( currentCenter.x - diameter/4, currentCenter.y - diameter/4, currentCenter.z - diameter/4 );
			break;
		}
	case Bottom_Back_Right:   // 7
		{
			vNewCenter = DragonXVector3( currentCenter.x + diameter/4, currentCenter.y - diameter/4, currentCenter.z - diameter/4 );
			break;
		}
	default:
		{
			break;
		}
	}
	return vNewCenter;
}
// sets up the node assigning its entities into its vector this basically is the end of the reccursion
void HydraOctree::SetNode(std::vector<BaseGameEntity*>& entities, int numEntities)
{
	m_bIsNode = true;
	m_iNumObjects = numEntities;
	m_vEntities.clear();
	//memcpy(&m_vEntities,&entities,numEntities);
	m_vEntities = entities;
}
// call this to generate contacts using the partitioned space
void HydraOctree::GetContacts(DragonContactGenerator& contactGen, HydraOctree* pNode)
{
	if (pNode==NULL)
	{
		return;
	}
	if (pNode->m_bIsNode)
	{
		contactGen.GenerateContacts(pNode->m_vEntities);
	}
	else
	{
		GetContacts(contactGen,pNode->m_pHydraNodes[Top_Front_Left]);
		GetContacts(contactGen,pNode->m_pHydraNodes[Top_Front_Right]);
		GetContacts(contactGen,pNode->m_pHydraNodes[Top_Back_Left]);
		GetContacts(contactGen,pNode->m_pHydraNodes[Top_Back_Right]);

		GetContacts(contactGen,pNode->m_pHydraNodes[Bottom_Front_Left]);
		GetContacts(contactGen,pNode->m_pHydraNodes[Bottom_Front_Right]);
		GetContacts(contactGen,pNode->m_pHydraNodes[Bottom_Back_Left]);
		GetContacts(contactGen,pNode->m_pHydraNodes[Bottom_Back_Right]);
	}
}