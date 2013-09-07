#pragma once

#include "DragonContact.h"
#include "DragonContactResolver.h"
#include <vector>

//this class will hold the master list of contacts
// as well as enable contacts to be added manually be it cable or rod
// or whatever
// it will also hold a simler list that just has two entities per element this is the registry
// its purpose is to identify and hold all entities that are currently in contact these are DragonicPairs
struct DragonicPair
{
	int mContactregistryType;// this is simply natural contact or if not then it will be handled differently
	int mCollisionType;//this will be set by the collision detector to allow for no checking of object types for contact generation
	BaseGameEntity* obj1;//just the objects in the pair
	BaseGameEntity* obj2;
};

class DragonContactRegistry
{
private:
	real m_Restitution;
	//real m_tolerence;
	unsigned int m_iLimit;
public:
	// default constructor
	DragonContactRegistry();
	//destructor
	~DragonContactRegistry();
	// vector of pairs, this is a master list of pairs
	std::vector<DragonicPair*> m_vContactPairs;
	// vector of contacts, master list of contacts
	std::vector<DragonContact*> m_vContacts;
	// ray contacts are not to be resolved but a point of contact is needed so
	// we have a separate vector of contacts for them that is never resolved but rather
	// simply holds information
	std::vector<DragonContact*> m_RayContacts;
	// we dont want multiple pairs that contain the same entities so
	// we use an add method to check the pair against the current list and only add it
	// if no matching pair is found  natural meaning this will
	// be used by the collision detector we may wish to add other non natural pairs
	// such as rods which will need to be done dealt with differently
	int AddNaturalContactPair(int,BaseGameEntity*, BaseGameEntity*);
	// we will use a new method for adding ray contacts
	int AddRayContactPair(int,BaseGameEntity*,BaseGameEntity*);
	// this holds the master list of contacts so we will just take in a contact resolver
	// and after it has resolved the contacts we will update the list
	// removing the fixed contacts and keeping the not fixed ones
	void ResolveContacts(DragonContactResolver*,real);
	// this will update the contact list
	void UpdateContactList(real);
	// this will generate the contacts for each pair
	void GenerateContact(int,BaseGameEntity*,BaseGameEntity*);
	// this clears n deletes the vector of contacts
	void ClearContacts();
	// this clears n deletes the contactPairs
	void ClearPairs();
	// these are methods for generating contacts all done internally
	// this is for sphere sphere
	void GenerateContactSphereSphere(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// plane sphere
	void GenerateContactSpherePlane(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// sphere obb
	void GenerateContactSphereOBB(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// obb plane
	void GenerateContactOBBPlane(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// obb obb
	void GenerateContactOBBOBB(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// ray sphere
	void GenerateContactRaySphere(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// ray obb
	void GenerateContactRayObb(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// mesh sphere
	void GenerateContactMeshSphere(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// sphere tri
	void GenerateContactSphereTri(BaseGameEntity* mesh,BaseGameEntity* sphere,DragonTriangle* tri);
	// gen contact mesh obb
	void GenerateContactMeshObb(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// obb tri
	void GenerateContactObbTri(BaseGameEntity* mesh,BaseGameEntity* obb,DragonTriangle* tri);
	// gen contact mesh mesh
	void GenerateContactsMeshMesh(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// gen contacts tri tri
	void GenerateContactsTriTri(BaseGameEntity* mesh1,DragonTriangle* tri1,BaseGameEntity* mesh2,DragonTriangle* tri2);
	// quick test for sector ray
	bool SectorRayCollisionDetection(DragonXVector3& spherePos,real rad,DragonXVector3& rayOrigin,DragonXVector3& rayDirection);
	// gen contact mesh ray
	void GenerateContactsMeshRay(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// tri ray contact gen
	void GenerateContactsRayTri(BaseGameEntity* mesh, DragonTriangle* tri,BaseGameEntity* ray);
	// and some helpers for obb obb
	// this generates the contact for point face
	void fillPointFaceBoxBox(BaseGameEntity &one,BaseGameEntity &two,const DragonXVector3 &toCentre,unsigned best,real pen);
	// this calls the above along with edge edge contact generation
	void ObbObbContactGen(BaseGameEntity &one,BaseGameEntity &two);
	// because this retains contact info
	// we need a way to remove contacts
	void RemoveAllContactsFrom(BaseGameEntity* obj);
	// for ease of erasing we will make it possible to remove multiple
	void RemoveContactsFromMultiple(std::vector<BaseGameEntity*>& list);
	// clears the ray contacts
	inline void ClearRayContacts()
	{
		for(const auto DragonContact : m_RayContacts)
		{
			delete DragonContact;
		}
		m_RayContacts.clear();
	}
	//tri contact gen
	inline bool ComputeIntervals(real vv0,real vv1,real vv2,real d0,real d1,real d2,real d0d1,real d0d2,real& a,real& b, real& c,real& x0, real& x1)
	{
		if(d0d1>0.0f)
		{
			a=vv2; b=(vv0-vv2)*d2;c=(vv1-vv2)*d2;x0=d2-d0;x1=d2-d1;
			return false;
		}
		else if(d0d2>0.0f)
		{
			a=vv1;b=(vv0-vv1)*d1;c=(vv2-vv1)*d1;x0=d1-d0;x1=d1-d2;
			return false;
		}
		else if(d1*d2>0.0f||d0!=0.0f)
		{
			a=vv0;b=(vv1-vv0)*d0;c=(vv2-vv0)*d0;x0=d0-d1;x1=d0-d2;
			return false;
		}
		else if(d1!=0.0f)
		{
			a=vv1;b=(vv0-vv1)*d1;c=(vv2-vv1)*d1;x0=d1-d0;x1=d1-d2;
			return false;
		}
		else if(d2!=0.0f)
		{
			a=vv2;b=(vv0-vv2)*d2;c=(vv1-vv2)*d2;x0=d2-d0;x1=d2-d1;
			return false;
		}
		else
		{
			return true;
		}
	}
};
