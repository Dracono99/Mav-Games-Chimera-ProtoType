#pragma once

#include "DragonContact.h"
#include "DragonContactRegistry.h"
#include <vector>

class DragonContactGenerator
{
private:
	// tollerence is used mostly to get obbs to stack it will essentially reduce jitters
	// trade off of course being accuracy to get things to stack you have to allow
	// some interpenitration not alot but its still penitration usually you cant see it
	// but i have done some freeze frames and seen some visible penitration
	real m_tolerence;//this is the tolerance to give some wiggle room for collisions
	// this is the maximum number of pairs of entities that can be added per frame
	int m_iLimit;
	// this is the current number od pairs that have been added this frame
	int m_numPairs;
public:
	// the constructor or default constructor i should say
	// it only sets up some values such as limit which is set at default to 250
	// restitution to atm 0.20 though i mess around with this value
	// kinda often just to see what happens
	// lastly the tolorence is set at 0.2 you could mess with it
	// i dont but atm its also not fully implimented
	// only used in obb stuff as its primary function
	// when first implimented was to aid in stacking
	DragonContactGenerator();
	// a or i guess the only parameterized constructor its only taking and setting the limit
	// the other values remain the defaults
	DragonContactGenerator(int limit);
	// destructor this will clear the internal vector of contacts
	// possibly worth mentioning that although i may use list and vector interchangeably
	// in text i do know they are 2 entirely different containers and i use vector basically exclusively
	// however i will likely revise and update my code to employ various
	// c++11 standards and use the associated stls and algorithms
	// as well as possibly lambdas should i find a need or use of them
	// at the time of writing i cant think of one aside from possibly implimenting them in the resolver
	// algorithm but i haven't done it yet
	~DragonContactGenerator();
	// this is the main contact generator algorithm
	// it will take a vector of entities by refference NOTE: i do refference
	// mostly for performance reasons
	// and this will basically fill the internal vector of contacts with any
	// collisions it finds
	void GenerateContacts(std::vector<BaseGameEntity*>& entities);
	// this will return a vector of contatcs
	//NOTE: this returns the vector by value which is not the most
	// efficient method of return possible and will likely be updated
	// and modified for performance in the final build
	std::vector<DragonContact*> GetContacts();
	// now this will return the current limit of contacts
	// no real reason but its here if needed
	int GetLimit();
	// this will set the limit if it is needed it be set
	void SetLimit(int limit);
	// the utilities that are below are tools for use later
	// or rather tools that can be used by other things or classes
	// else where in the overall application however these are not used in the physics core it self
	// as it has its own internal methods for collision detection
	//collision detection utility pass in the key variables
	// this will take 2 centers and 2 radiai and return true or false
	// based on if the data passed in returns true or false so if the objects the
	// data passed in represent are contacting then it will return true
	bool SphereSphereCollisionTest(DragonXVector3 pos1,real radius1,DragonXVector3 pos2, real radius2);
	//internal collision detection
	// this will internally determine collision based on the objects
	// and return true if the objects are contacting
	bool SphereSphereCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// collision utility
	// basically same deal as sphere sphere utility but for sphere plane
	// i dont have any more utilities like that created
	// beyond this one atm but ill likely make more
	// as needed
	bool SpherePlaneCollisionTest(DragonXVector3 pos,real radius,DragonXVector3 normal,real distFromOrigin);
	// internal collision test
	// the remaining tests are all just booleans if the passed in objects
	// are colliding based on the object which they represtent will determine
	// if they are colliding and will return the result true or false
	// in easy terms or an example rather if plane and sphere are coliding it will return true if not
	// then false same principle for all of these methods
	bool SpherePlaneCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2);
	//obb collision
	// sphere obb
	bool SphereOBBCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2);
	//obb plane
	bool ObbPlaneCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// obb obb
	bool ObbObbCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// ray sphere collision test
	bool RaySphereCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// ray obb collision test
	bool RayObbCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// mesh sphere test
	bool MeshSphereCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// mesh obb collision detection
	bool MeshObbCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// triangle sphere test
	bool TriangleSphereCollisionTest(DragonTriangle* tri,BaseGameEntity* mesh,BaseGameEntity* sphere);
	// triangle obb test
	bool TriangleObbCollisionTest(DragonTriangle* tri, DragonXVector3& localCenter,DragonXVector3& halfExtents,DragonXVector3& u0,DragonXVector3& u1,DragonXVector3& u2);
	// triangle obb test
	bool TriangleObbCollisionTest(DragonTriangle* tri,BaseGameEntity* mesh, BaseGameEntity* obb);
	// closest point on triangle to point
	DragonXVector3 ClosestPtPointTriangle(DragonXVector3& p,DragonXVector3& a,DragonXVector3& b,DragonXVector3& c);
	// this is used in tri tri collision detection
	bool TestTriTri(DragonTriangle* tri1,BaseGameEntity* mesh1,DragonTriangle* tri2,BaseGameEntity* mesh2);
	// tri plane returns false if all three verts are on the same side of the plane
	bool TestTriVertsAgainstPlane(DragonXVector3& vertA,DragonXVector3& vertB,DragonXVector3& vertC,DragonXVector3& planeNormal,real planeDist,real& d0,real& d1,real& d2);
	// mesh mesh test
	bool MeshMeshCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// mesh ray rest
	bool MeshRay(BaseGameEntity* obj1, BaseGameEntity* obj2);
	// param sphere ray collision detection
	bool SectorRayCollisionDetection(DragonXVector3& spherePos,real rad,DragonXVector3& rayOrigin,DragonXVector3& rayDirection);
	// tri ray collision detection
	bool TriangleRayCollisionTest(DragonXVector3& triVertA,DragonXVector3& triVertB,DragonXVector3& triVertC,DragonXVector3& rayPos,DragonXVector3& rayDir);
	// that is till you reach this one
	// this is a helper function that will determine the type of contact
	// the collision detector should test for the return type int is actually an
	// enum which is defined in my enum header file
	// under my templates filter i just always think enums n templates go together in the sort
	// of helper category
	int collisionType(BaseGameEntity* obj1,BaseGameEntity* obj2);
	// this is used to clear the list of contacts note it doesnt delete them only clears garbage data as the contact resolver deletes them after it has finished resolving
	void ZeroContacts();
	DragonContactRegistry* registry;
};
