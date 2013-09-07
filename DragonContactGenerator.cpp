#include "DragonContactGenerator.h"
#include "BasePhysicsEntity.h"
#include "DragonEnums.h"
#include "d3dApp.h"
#include <cassert>
#include <iostream>

const static real REAL_MAX=9999999.99f;// this is a static const used locally though it is technically a global variable or rather a global const so yes its global scope but its unmodifyable

// this is a helper function that basically performs the dot product of the vector 3 axis passed in with the half sizes of the box passed in
// the dot product resultant being the projection of one vector 3 or in this case the axis of one box on to the axis of the box thats passed in
// the return value is the result of these dot products
static inline real transformToAxis( BaseGameEntity &box, const DragonXVector3 &axis)
{
	return box.GetPhysicsPointer()->GetHalfSizes().x * (real)abs(axis * box.GetPhysicsPointer()->getTransform().getAxisVector(0)) +
		box.GetPhysicsPointer()->GetHalfSizes().y * (real)abs(axis * box.GetPhysicsPointer()->getTransform().getAxisVector(1)) +
		box.GetPhysicsPointer()->GetHalfSizes().z * (real)abs(axis * box.GetPhysicsPointer()->getTransform().getAxisVector(2));
}
// this is a helper function i wrote for use with kdops but as they are still in developement its currently used
static inline void clostestPtPointOBB(DragonXVector3& p,BaseGameEntity* obb, DragonXVector3& q)
{
	DragonXVector3 d = p - obb->GetPhysicsPointer()->getPosition();
	q = obb->GetPhysicsPointer()->getPosition();
	DragonXVector3 he = obb->GetPhysicsPointer()->GetHalfSizes();
	//for each obb axis

	real dist = d * obb->GetPhysicsPointer()->getTransform().getAxisVector(0);
	// if the dist is further than the box extents clamp it
	if(dist> he.x)
	{
		dist = he.x;
	}
	if(dist < -he.x)
	{
		dist = -he.x;
	}
	q+=dist * obb->GetPhysicsPointer()->getTransform().getAxisVector(0);

	dist = d * obb->GetPhysicsPointer()->getTransform().getAxisVector(1);
	// if the dist is further than the box extents clamp it
	if(dist> he.y)
	{
		dist = he.y;
	}
	if(dist < -he.y)
	{
		dist = -he.y;
	}
	q+=dist * obb->GetPhysicsPointer()->getTransform().getAxisVector(1);

	dist = d * obb->GetPhysicsPointer()->getTransform().getAxisVector(2);
	// if the dist is further than the box extents clamp it
	if(dist> he.z)
	{
		dist = he.z;
	}
	if(dist < -he.z)
	{
		dist = -he.z;
	}
	q+=dist * obb->GetPhysicsPointer()->getTransform().getAxisVector(2);
}

/**
* This function checks if the two boxes overlap
* along the given axis. The final parameter toCentre
* is used to pass in the vector between the boxes center
* points, to avoid having to recalculate it each time.
*/
static inline bool overlapOnAxis(
	BaseGameEntity &one,
	BaseGameEntity &two,
	const DragonXVector3 &axis,
	const DragonXVector3 & toCentre
	)
{
	// project the half size of one onto axis
	real oneProject = transformToAxis(one,axis);
	real twoProject = transformToAxis(two,axis);

	// project this onto the axis
	real dist = (real)abs(toCentre * axis);

	// check for overlap
	return (dist <= oneProject + twoProject);
}
// obb obb collision test is done here do to its use of a large number of helper functions and so to keep the other code cleaner
#define TEST_OVERLAP(axis) overlapOnAxis(one,two,(axis),toCentre)

bool ObbObbTest(BaseGameEntity &one,BaseGameEntity &two)
{
	DragonXVector3 toCentre = two.GetPhysicsPointer()->getPosition() - one.GetPhysicsPointer()->getPosition();

	return(
		// check box ones axis first
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(0)) &&
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(1)) &&
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(2)) &&

		// now twos
		TEST_OVERLAP(two.GetPhysicsPointer()->getTransform().getAxisVector(0)) &&
		TEST_OVERLAP(two.GetPhysicsPointer()->getTransform().getAxisVector(1)) &&
		TEST_OVERLAP(two.GetPhysicsPointer()->getTransform().getAxisVector(2)) &&

		// and now the cross products
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(0) % two.GetPhysicsPointer()->getTransform().getAxisVector(0)) &&
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(0) % two.GetPhysicsPointer()->getTransform().getAxisVector(1)) &&
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(0) % two.GetPhysicsPointer()->getTransform().getAxisVector(2)) &&
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(1) % two.GetPhysicsPointer()->getTransform().getAxisVector(0)) &&
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(1) % two.GetPhysicsPointer()->getTransform().getAxisVector(1)) &&
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(1) % two.GetPhysicsPointer()->getTransform().getAxisVector(2)) &&
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(2) % two.GetPhysicsPointer()->getTransform().getAxisVector(0)) &&
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(2) % two.GetPhysicsPointer()->getTransform().getAxisVector(1)) &&
		TEST_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(2) % two.GetPhysicsPointer()->getTransform().getAxisVector(2))
		);
}
#undef TEST_OVERLAP

// returns true if coplanar test is needed
static inline bool ComputeIntervals(real vv0,real vv1,real vv2,real d0,real d1,real d2,real d0d1,real d0d2,real& a,real& b, real& c,real& x0, real& x1)
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

static inline real DistPointPlane(DragonXVector3& p, DragonXVector3& planeNormal,real planeDist)
{
	return p*planeNormal-planeDist;
}

static inline bool EdgeEdgeTest(int i0,int i1,real ax,real ay,DragonXVector3& v0, DragonXVector3& u0, DragonXVector3& u1)
{
	real bx = u0[i0]-u1[i0];
	real by = u0[i1]-u1[i1];
	real cx = v0[i0]-u0[i0];
	real cy = v0[i1]-u0[i1];
	real f = ay*bx-ax*by;
	real d = by*cx-bx*cy;
	if((f>0.0f&&d<=f)||(f<0&&d<=0&&d>=f))
	{
		real e = ax*cy-ay*cx;
		if(f>0)
		{
			if(e>=0&&e<=f)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(e<=0&&e>=f)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
}

static inline bool EdgeAgainstTriEdges(int i0,int i1,DragonXVector3& v0,DragonXVector3& v1, DragonXVector3& u0,DragonXVector3& u1, DragonXVector3& u2)
{
	real ax = v1[i0]-v0[i0];
	real ay = v1[i0] - v0[i1];

	if(EdgeEdgeTest(i0,i1,ax,ay,v0,u0,u1))
	{
		return true;
	}
	else if(EdgeEdgeTest(i0,i1,ax,ay,v0,u1,u2))
	{
		return true;
	}
	else if(EdgeEdgeTest(i0,i1,ax,ay,v0,u2,u0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

static inline bool PointInTri(int i0, int i1, DragonXVector3& v0,DragonXVector3& u0,DragonXVector3& u1, DragonXVector3& u2)
{
	real a = u1[i1]-u0[i1];
	real b=-(u1[i0]-u0[i0]);
	real c=-a*u0[i0]-b*u0[i1];
	real d0=a*v0[i0]+b*v0[i1]+c;

	a = u2[i1]-u1[i1];
	b=-(u2[i0]-u1[i0]);
	c=-a*u1[i0]-b*u1[i1];
	real d1=a*v0[i0]+b*v0[i1]+c;

	a = u0[i1]-u2[i1];
	b=-(u0[i0]-u2[i0]);
	c=-a*u2[i0]-b*u2[i1];
	real d2=a*v0[i0]+b*v0[i1]+c;

	if(d0*d1>0.0f)
	{
		if(d0*d2>0.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

static inline bool CoplanarTriTest(DragonXVector3& norm,DragonXVector3& tri1a,DragonXVector3& tri1b,DragonXVector3& tri1c,DragonXVector3& tri2a,DragonXVector3& tri2b,DragonXVector3& tri2c)
{
	real a0 = (real)abs(norm.x);
	real a1 = (real)abs(norm.y);
	real a2 = (real)abs(norm.z);
	int i0 = 0;
	int i1 = 0;

	if(a0>a1)
	{
		if(a0>a2)
		{
			i0=1;
			i1=2;
		}
		else
		{
			i0=0;
			i1=1;
		}
	}
	else
	{
		if(a2>a1)
		{
			i0=0;
			i1=1;
		}
		else
		{
			i0=0;
			i1=2;
		}
	}

	if(EdgeAgainstTriEdges(i0,i1,tri1a,tri1b,tri2a,tri2b,tri2c))
	{
		return true;
	}
	else if(EdgeAgainstTriEdges(i0,i1,tri1b,tri1c,tri2a,tri2b,tri2c))
	{
		return true;
	}
	else if(EdgeAgainstTriEdges(i0,i1,tri1c,tri1a,tri2a,tri2b,tri2c))
	{
		return true;
	}
	else if(PointInTri(i0,i1,tri1a,tri2a,tri2b,tri2c))
	{
		return true;
	}
	else if(PointInTri(i0,i1,tri2a,tri1a,tri1b,tri1c))
	{
		return true;
	}
	else
	{
		return false;
	}
}

DragonContactGenerator::DragonContactGenerator()
{
	m_iLimit=250;//Anthony said a 250 contact limit
	m_numPairs=0;
	m_tolerence=0.0f;
	registry = new DragonContactRegistry();
}
DragonContactGenerator::DragonContactGenerator(int limit)
{
	m_iLimit=limit;
	m_numPairs=0;
	m_tolerence=0.1f;
	registry = new DragonContactRegistry();
}
DragonContactGenerator::~DragonContactGenerator()
{
	delete registry;
}
void DragonContactGenerator::ZeroContacts()
{
	m_numPairs=0;
}

void DragonContactGenerator::GenerateContacts(std::vector<BaseGameEntity*>& entities)
{
	//m_vContacts.clear();
	for(int i = 0;(unsigned)i<entities.size()-1;i++)
	{
		for(int j = i+1;(unsigned)j<entities.size();j++)
		{
			if(m_numPairs<m_iLimit)
			{
				if(entities[i]->GetPhysicsPointer()->m_isCollisionDetectionOn&&entities[j]->GetPhysicsPointer()->m_isCollisionDetectionOn)
				{
					switch(collisionType(entities[i],entities[j]))
					{
					case CT_SphereSphere:
						{
							if(SphereSphereCollisionTest(entities[i],entities[j]))
							{
								m_numPairs+=registry->AddNaturalContactPair(CT_SphereSphere,entities[i],entities[j]);
								break;
							}
							break;
						}
					case CT_SpherePlane:
						{
							if(SpherePlaneCollisionTest(entities[i],entities[j]))
							{
								m_numPairs+=registry->AddNaturalContactPair(CT_SpherePlane,entities[i],entities[j]);
								break;
							}
							break;
						}
					case CT_SphereOBB:
						{
							if(SphereSphereCollisionTest(entities[i],entities[j]))
							{
								if(SphereOBBCollisionTest(entities[i],entities[j]))
								{
									m_numPairs+=registry->AddNaturalContactPair(CT_SphereOBB,entities[i],entities[j]);
									break;
								}
							}
							break;
						}
					case CT_ObbPlane:
						{
							if(SpherePlaneCollisionTest(entities[i],entities[j]))
							{
								if(ObbPlaneCollisionTest(entities[i],entities[j]))
								{
									m_numPairs+=registry->AddNaturalContactPair(CT_ObbPlane,entities[i],entities[j]);
									break;
								}
							}
							break;
						}
					case CT_ObbObb:
						{
							if(SphereSphereCollisionTest(entities[i],entities[j]))
							{
								if(ObbObbCollisionTest(entities[i],entities[j]))
								{
									m_numPairs+=registry->AddNaturalContactPair(CT_ObbObb,entities[i],entities[j]);
									//std::cout<<"obb obb collision detected\r\n";// ok took some code from the old mantis main n now have a console window we can use in debugging
									break;
								}
							}
							break;
						}
					case CT_RaySphere:
						{
							if(RaySphereCollisionTest(entities[i],entities[j]))
							{
								m_numPairs+=registry->AddRayContactPair(CT_RaySphere,entities[i],entities[j]);
								break;
							}
							break;
						}
					case CT_RayOBB:
						{
							if(RaySphereCollisionTest(entities[i],entities[j]))
							{
								if (RayObbCollisionTest(entities[i],entities[j]))
								{
									m_numPairs+=registry->AddRayContactPair(CT_RayOBB,entities[i],entities[j]);
									break;
								}
							}
							break;
						}
					case CT_MeshSphere:
						{
							if(SphereSphereCollisionTest(entities[i],entities[j]))
							{
								if (SphereOBBCollisionTest(entities[i],entities[j]))
								{
									if (MeshSphereCollisionTest(entities[i],entities[j]))
									{
										gd3dApp->GetStats()->addVertices(1);
										m_numPairs+=registry->AddNaturalContactPair(CT_MeshSphere,entities[i],entities[j]);
										break;
									}
								}
							}
							break;
						}
					case CT_MeshOBB:
						{
							if(SphereSphereCollisionTest(entities[i],entities[j]))
							{
								if (ObbObbCollisionTest(entities[i],entities[j]))
								{
									if (MeshObbCollisionTest(entities[i],entities[j]))
									{
										gd3dApp->GetStats()->addVertices(1);
										m_numPairs+=registry->AddNaturalContactPair(CT_MeshOBB,entities[i],entities[j]);
										break;
									}
								}
							}
							break;
						}
					case CT_MeshMesh:
						{
							if(SphereSphereCollisionTest(entities[i],entities[j]))
							{
								if(ObbObbCollisionTest(entities[i],entities[j]))
								{
									if(MeshMeshCollisionTest(entities[i],entities[j]))
									{
										gd3dApp->GetStats()->addVertices(1);
										m_numPairs+=registry->AddNaturalContactPair(CT_MeshMesh,entities[i],entities[j]);
										break;
									}
								}
							}
							break;
						}
					case CT_MeshRay:
						{
							if(RaySphereCollisionTest(entities[i],entities[j]))
							{
								if(RayObbCollisionTest(entities[i],entities[j]))
								{
									if(MeshRay(entities[i],entities[j]))
									{
										gd3dApp->GetStats()->addVertices(1);
										m_numPairs+=registry->AddRayContactPair(CT_MeshRay,entities[i],entities[j]);
										break;
									}
								}
							}
							break;
						}
					default:
						{
							break;
						}
					}
				}
			}
		}
	}
}

int DragonContactGenerator::collisionType(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if((obj1->GetPhysicsPointer()->mI_PhysicsType==COT_World)&&(obj2->GetPhysicsPointer()->mI_PhysicsType==COT_World))
	{
		return -1;
	}
	if((!obj1->GetPhysicsPointer()->getAwake())&&(!obj2->GetPhysicsPointer()->getAwake()))
	{
		return -1;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere))
	{
		return CT_SphereSphere;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Plane)||(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Plane)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere))
	{
		return CT_SpherePlane;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB)||(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere))
	{
		return CT_SphereOBB;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Plane)||(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Plane)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB))
	{
		return CT_ObbPlane;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB))
	{
		return CT_ObbObb;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere)||(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray))
	{
		return CT_RaySphere;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB)||(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray))
	{
		return CT_RayOBB;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere)||(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh))
	{
		return CT_MeshSphere;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB)||(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh))
	{
		return CT_MeshOBB;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh))
	{
		return CT_MeshMesh;
	}
	else if((obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray)||(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray)&&(obj2->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh))
	{
		return CT_MeshRay;
	}
	else
	{
		return -1;
	}
}
std::vector<DragonContact*> DragonContactGenerator::GetContacts()
{
	return registry->m_vContacts;
}
int DragonContactGenerator::GetLimit()
{
	return m_iLimit;
}
void DragonContactGenerator::SetLimit(int limit)
{
	m_iLimit=limit;
}
bool DragonContactGenerator::SphereSphereCollisionTest(DragonXVector3 pos1,real radius1,DragonXVector3 pos2, real radius2)
{
	return (pos1-pos2).GetMagSquared() < (radius1+radius2)*(radius1+radius2);
}
bool DragonContactGenerator::SphereSphereCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	return (obj1->GetPhysicsPointer()->getPosition() - obj2->GetPhysicsPointer()->getPosition()).GetMagSquared() <((obj1->GetPhysicsPointer()->GetRadius()+obj2->GetPhysicsPointer()->GetRadius())*(obj1->GetPhysicsPointer()->GetRadius()+obj2->GetPhysicsPointer()->GetRadius()));
}
// collision utility
bool DragonContactGenerator::SpherePlaneCollisionTest(DragonXVector3 pos,real radius,DragonXVector3 normal,real distFromOrigin)
{
	return (normal*pos-radius)<=(distFromOrigin);
}
// internal collision test
bool DragonContactGenerator::SpherePlaneCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere||obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB)//||obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_KDOP)
	{
		if((obj2->GetPhysicsPointer()->GetNormal() * obj1->GetPhysicsPointer()->getPosition() - obj1->GetPhysicsPointer()->GetRadius()) <= obj2->GetPhysicsPointer()->GetDistFromOrigin())
		{
			real dist = obj2->GetPhysicsPointer()->GetNormal() * obj1->GetPhysicsPointer()->getPosition() - obj2->GetPhysicsPointer()->GetDistFromOrigin();
			if(dist*dist>obj1->GetPhysicsPointer()->GetRadius()*obj1->GetPhysicsPointer()->GetRadius())
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		if((obj1->GetPhysicsPointer()->GetNormal() * obj2->GetPhysicsPointer()->getPosition() - obj2->GetPhysicsPointer()->GetRadius()) <= obj1->GetPhysicsPointer()->GetDistFromOrigin())
		{
			real dist = obj1->GetPhysicsPointer()->GetNormal() * obj2->GetPhysicsPointer()->getPosition() - obj1->GetPhysicsPointer()->GetDistFromOrigin();
			if(dist*dist>obj2->GetPhysicsPointer()->GetRadius()*obj2->GetPhysicsPointer()->GetRadius())
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
}
// sphere obb
bool DragonContactGenerator::SphereOBBCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere)//if object 1 is the sphere
	{
		DragonXVector3 center = obj1->GetPhysicsPointer()->getPosition();
		DragonXVector3 relCenter = obj2->GetPhysicsPointer()->getTransform().TransformInverse(center);
		if(abs(relCenter.x) - obj1->GetPhysicsPointer()->GetRadius() > obj2->GetPhysicsPointer()->GetHalfSizes().x ||
			abs(relCenter.y) - obj1->GetPhysicsPointer()->GetRadius() > obj2->GetPhysicsPointer()->GetHalfSizes().y ||
			abs(relCenter.z) - obj1->GetPhysicsPointer()->GetRadius() > obj2->GetPhysicsPointer()->GetHalfSizes().z)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else //obj2 is the sphere and obj1 is the obb
	{
		DragonXVector3 center = obj2->GetPhysicsPointer()->getPosition();
		DragonXVector3 relCenter = obj1->GetPhysicsPointer()->getTransform().TransformInverse(center);
		if(abs(relCenter.x) - obj2->GetPhysicsPointer()->GetRadius() > obj1->GetPhysicsPointer()->GetHalfSizes().x ||
			abs(relCenter.y) - obj2->GetPhysicsPointer()->GetRadius() > obj1->GetPhysicsPointer()->GetHalfSizes().y ||
			abs(relCenter.z) - obj2->GetPhysicsPointer()->GetRadius() > obj1->GetPhysicsPointer()->GetHalfSizes().z)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

//obb plane
bool DragonContactGenerator::ObbPlaneCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB||obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_KDOP)//if obj1 is an obb do this
	{
		real projectedRadius = transformToAxis(*obj1,obj2->GetPhysicsPointer()->GetNormal());
		//real projectedRadius = obj1->GetPhysicsPointer()->GetRadius();
		real boxDistance = (obj2->GetPhysicsPointer()->GetNormal() * obj1->GetPhysicsPointer()->getPosition()) - projectedRadius;
		if(boxDistance>= obj2->GetPhysicsPointer()->GetDistFromOrigin()-m_tolerence)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else//else obj2 is the obb and obj1 is the plane
	{
		real projectedRadius = transformToAxis(*obj2,obj1->GetPhysicsPointer()->GetNormal());
		//real projectedRadius = obj2->GetPhysicsPointer()->GetRadius();
		real boxDistance = (obj1->GetPhysicsPointer()->GetNormal() * obj2->GetPhysicsPointer()->getPosition()) - projectedRadius;
		if(boxDistance>= obj1->GetPhysicsPointer()->GetDistFromOrigin()-m_tolerence)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}
// obb obb
bool DragonContactGenerator::ObbObbCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	return ObbObbTest(*obj1,*obj2);
}

bool DragonContactGenerator::RaySphereCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if (obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray)//if obj1 is the ray
	{
		DragonXVector3 m=obj1->GetPhysicsPointer()->getPosition()-obj2->GetPhysicsPointer()->getPosition();
		real b = m*obj1->GetPhysicsPointer()->GetNormal();
		real c = m*m-obj2->GetPhysicsPointer()->GetRadius()*obj2->GetPhysicsPointer()->GetRadius();

		if (c>0.0f&&b>0.0f)
		{
			return false;
		}

		real discr = b*b-c;
		if(discr<0.0f)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else// obj2 is the ray
	{
		DragonXVector3 m=obj2->GetPhysicsPointer()->getPosition()-obj1->GetPhysicsPointer()->getPosition();
		real b = m*obj2->GetPhysicsPointer()->GetNormal();
		real c = (m*m)-(obj1->GetPhysicsPointer()->GetRadius()*obj1->GetPhysicsPointer()->GetRadius());

		if (c>0.0f&&b>0.0f)
		{
			return false;
		}

		real discr = b*b-c;
		if(discr<0.0f)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

bool DragonContactGenerator::RayObbCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	real EPSILON = 0.001f;
	if (obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray)//if obj1 is ray
	{
		real tmin=-999999999.9f;
		real tmax=9999999999.9f;
		DragonXVector3 p = obj2->GetPhysicsPointer()->TransformMatrix.TransformInverse(obj1->GetPhysicsPointer()->getPosition());
		DragonXVector3 d = obj2->GetPhysicsPointer()->TransformMatrix.TransformInverseDirection(obj1->GetPhysicsPointer()->GetNormal());
		if(abs(d.x)<EPSILON)// for x
		{
			if(p.x<-obj2->GetPhysicsPointer()->GetHalfSizes().x||p.x>obj2->GetPhysicsPointer()->GetHalfSizes().x)
			{
				return false;
			}
		}
		else
		{
			real ood = 1.0f / d.x;
			real t1 = (-obj2->GetPhysicsPointer()->GetHalfSizes().x - p.x)*ood;
			real t2 = (obj2->GetPhysicsPointer()->GetHalfSizes().x - p.x)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return false;
			}
		}
		if(abs(d.y)<EPSILON)// for y
		{
			if(p.y<-obj2->GetPhysicsPointer()->GetHalfSizes().y||p.y>obj2->GetPhysicsPointer()->GetHalfSizes().y)
			{
				return false;
			}
		}
		else
		{
			real ood = 1.0f / d.y;
			real t1 = (-obj2->GetPhysicsPointer()->GetHalfSizes().y - p.y)*ood;
			real t2 = (obj2->GetPhysicsPointer()->GetHalfSizes().y - p.y)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return false;
			}
		}
		if(abs(d.z)<EPSILON)// for z
		{
			if(p.z<-obj2->GetPhysicsPointer()->GetHalfSizes().z||p.z>obj2->GetPhysicsPointer()->GetHalfSizes().z)
			{
				return false;
			}
		}
		else
		{
			real ood = 1.0f / d.z;
			real t1 = (-obj2->GetPhysicsPointer()->GetHalfSizes().z - p.z)*ood;
			real t2 = (obj2->GetPhysicsPointer()->GetHalfSizes().z - p.z)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return false;
			}
		}
		return true;
	}
	else// obj2 is the ray
	{
		real tmin=-999999999.9f;
		real tmax=9999999999.9f;
		DragonXVector3 p = obj1->GetPhysicsPointer()->TransformMatrix.TransformInverse(obj2->GetPhysicsPointer()->getPosition());
		DragonXVector3 d = obj1->GetPhysicsPointer()->TransformMatrix.TransformInverseDirection(obj2->GetPhysicsPointer()->GetNormal());
		if(abs(d.x)<EPSILON)// for x
		{
			if(p.x<-obj1->GetPhysicsPointer()->GetHalfSizes().x||p.x>obj1->GetPhysicsPointer()->GetHalfSizes().x)
			{
				return false;
			}
		}
		else
		{
			real ood = 1.0f / d.x;
			real t1 = (-obj1->GetPhysicsPointer()->GetHalfSizes().x - p.x)*ood;
			real t2 = (obj1->GetPhysicsPointer()->GetHalfSizes().x - p.x)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return false;
			}
		}
		if(abs(d.y)<EPSILON)// for y
		{
			if(p.y<-obj1->GetPhysicsPointer()->GetHalfSizes().y||p.y>obj1->GetPhysicsPointer()->GetHalfSizes().y)
			{
				return false;
			}
		}
		else
		{
			real ood = 1.0f / d.y;
			real t1 = (-obj1->GetPhysicsPointer()->GetHalfSizes().y - p.y)*ood;
			real t2 = (obj1->GetPhysicsPointer()->GetHalfSizes().y - p.y)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return false;
			}
		}
		if(abs(d.z)<EPSILON)// for z
		{
			if(p.z<-obj1->GetPhysicsPointer()->GetHalfSizes().z||p.z>obj1->GetPhysicsPointer()->GetHalfSizes().z)
			{
				return false;
			}
		}
		else
		{
			real ood = 1.0f / d.z;
			real t1 = (-obj1->GetPhysicsPointer()->GetHalfSizes().z - p.z)*ood;
			real t2 = (obj1->GetPhysicsPointer()->GetHalfSizes().z - p.z)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return false;
			}
		}
		return true;
	}
}

DragonXVector3 DragonContactGenerator::ClosestPtPointTriangle(DragonXVector3& p,DragonXVector3& a,DragonXVector3& b,DragonXVector3& c)
{
	// check if p is in vertex region outside a
	DragonXVector3 ab = b-a;
	DragonXVector3 ac = c-a;
	DragonXVector3 ap = p-a;
	real d1 = ab*ap;
	real d2 = ac*ap;
	if(d1<=0.0f&&d2<=0.0f)
	{
		return a;
	}

	// check b
	DragonXVector3 bp = p-b;
	real d3 = ab*bp;
	real d4=ac*bp;
	if(d3>=0.0f&&d4<=d3)
	{
		return b;
	}

	// check edge region ab
	real vc = d1*d4-d3*d2;
	if(vc<=0.0f&&d1>=0.0f&&d3<=0.0f)
	{
		real v = d1/(d1-d3);
		return a+v*ab;
	}

	// check c
	DragonXVector3 cp = p-c;
	real d5=ab*cp;
	real d6=ac*cp;
	if(d6>=0.0f&&d5<=d6)
	{
		return c;
	}

	// check p on ac
	real vb = d5*d2-d1*d6;
	if(vb<=0.0f&&d2>=0.0f&&d6<=0.0f)
	{
		real w = d2/(d2-d6);
		return a+w*ac;
	}

	// check p on bc
	real va = d3*d6-d5*d4;
	if(va<=0.0f&&(d4-d3)>=0.0f&&(d5-d6)>=0.0f)
	{
		real w = (d4-d3)/((d4-d3)+(d5-d6));
		return b + w*(c-b);
	}

	// p is in the face region
	real denom = 1.0f/(va+vb+vc);
	real v = vb*denom;
	real w = vc * denom;
	return a+ab*v+ac*w;
}

bool DragonContactGenerator::TriangleSphereCollisionTest(DragonTriangle* tri,BaseGameEntity* mesh,BaseGameEntity* sphere)
{
	DragonXVector3 p = ClosestPtPointTriangle(sphere->GetPhysicsPointer()->getPosition(),mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertA()),mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertB()),mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertC()));
	//DragonXVector3 p = ClosestPtPointTriangle(sphere->GetPhysicsPointer()->getPosition(),mesh->GetPhysicsPointer()->getPointInWorldSpace(mesh->GetPhysicsPointer()->getDirectionInLocalSpace(tri->GetVertA())),mesh->GetPhysicsPointer()->getPointInWorldSpace(mesh->GetPhysicsPointer()->getDirectionInLocalSpace(tri->GetVertB())),mesh->GetPhysicsPointer()->getPointInWorldSpace(mesh->GetPhysicsPointer()->getDirectionInLocalSpace(tri->GetVertC())));
	p-=sphere->GetPhysicsPointer()->getPosition();
	if (p*p<=sphere->GetPhysicsPointer()->GetRadius()*sphere->GetPhysicsPointer()->GetRadius())
	{
		return true;
	}
	else
	{
		return false;
	}
}

// mesh sphere test
bool DragonContactGenerator::MeshSphereCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh)// if obj1 is the mesh
	{
		for (auto DragonSector : obj1->GetPhysicsPointer()->GetSectors())
		{
			if(SphereSphereCollisionTest(obj1->GetPhysicsPointer()->getPointInWorldSpace(DragonSector->GetPosition()),DragonSector->GetRadius(),obj2->GetPhysicsPointer()->getPosition(),obj2->GetPhysicsPointer()->GetRadius()))
			{
				for (auto DragonTriangle : DragonSector->GetDragonTris())
				{
					if (TriangleSphereCollisionTest(DragonTriangle,obj1,obj2))
					{
						return true;/// fix the mesh partitioning octree its positions are wrong
					}
				}
			}
		}
		return false;
	}
	else // otherwise obj2 is the mesh
	{
		for (auto DragonSector : obj2->GetPhysicsPointer()->GetSectors())
		{
			if(SphereSphereCollisionTest(obj2->GetPhysicsPointer()->getPointInWorldSpace(DragonSector->GetPosition()),DragonSector->GetRadius(),obj1->GetPhysicsPointer()->getPosition(),obj1->GetPhysicsPointer()->GetRadius()))
			{
				for (auto DragonTriangle : DragonSector->GetDragonTris())
				{
					if (TriangleSphereCollisionTest(DragonTriangle,obj2,obj1))
					{
						return true;
					}
				}
			}
		}
		return false;
	}
}

static inline real GetRadiusForObbTrTest(DragonXVector3& halfExtents,DragonXVector3& u0,DragonXVector3& u1,DragonXVector3& u2,DragonXVector3& axis)
{
	return halfExtents.x*(real)abs(u0*axis)+halfExtents.y*(real)abs(u1*axis)+halfExtents.z*(real)abs(u2*axis);
}
static inline real getMax(real zed, real ein, real zwei)
{
	real ret=-9999999999.9f;
	if(zed>ret)
	{
		ret=zed;
	}
	if(ein>ret)
	{
		ret = ein;
	}
	if(zwei>ret)
	{
		ret = zwei;
	}
	return ret;
}

static inline real getMin(real zed, real ein, real zwei)
{
	real ret=9999999999.9f;
	if(zed<ret)
	{
		ret=zed;
	}
	if(ein<ret)
	{
		ret = ein;
	}
	if(zwei<ret)
	{
		ret = zwei;
	}
	return ret;
}

static inline real getMax(real zed, real ein)
{
	if(zed>ein)
	{
		return zed;
	}
	else
	{
		return ein;
	}
}

static inline real getMin(real zed, real ein)
{
	if(zed<ein)
	{
		return zed;
	}
	else
	{
		return ein;
	}
}

bool DragonContactGenerator::MeshObbCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh)//if obj1 is the mesh
	{
		DragonXVector3 localCenter = obj1->GetPhysicsPointer()->getPointInLocalSpace(obj2->GetPhysicsPointer()->getPosition());
		DragonXVector3 u0 = obj2->GetPhysicsPointer()->getTransform().getAxisVector(0);
		DragonXVector3 u1 = obj2->GetPhysicsPointer()->getTransform().getAxisVector(1);
		DragonXVector3 u2 = obj2->GetPhysicsPointer()->getTransform().getAxisVector(2);
		/*DragonXVector3 u0 = obj1->GetPhysicsPointer()->getDirectionInLocalSpace(obj2->GetPhysicsPointer()->getTransform().getAxisVector(0));
		DragonXVector3 u1 = obj1->GetPhysicsPointer()->getDirectionInLocalSpace(obj2->GetPhysicsPointer()->getTransform().getAxisVector(1));
		DragonXVector3 u2 = obj1->GetPhysicsPointer()->getDirectionInLocalSpace(obj2->GetPhysicsPointer()->getTransform().getAxisVector(2));*/

		for (auto DragonSector : obj1->GetPhysicsPointer()->GetSectors())
		{
			if(SphereSphereCollisionTest(obj1->GetPhysicsPointer()->getPointInWorldSpace(DragonSector->GetPosition()),DragonSector->GetRadius(),obj2->GetPhysicsPointer()->getPosition(),obj2->GetPhysicsPointer()->GetRadius()))
			{
				for (auto DragonTriangle : DragonSector->GetDragonTris())
				{
					if (TriangleSphereCollisionTest(DragonTriangle,obj1,obj2))
					{
						if(TriangleObbCollisionTest(DragonTriangle,obj1,obj2))
						{
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	else// obj2 is the mesh
	{
		DragonXVector3 localCenter = obj2->GetPhysicsPointer()->getPointInLocalSpace(obj1->GetPhysicsPointer()->getPosition());
		DragonXVector3 u0 = obj1->GetPhysicsPointer()->getTransform().getAxisVector(0);
		DragonXVector3 u1 = obj1->GetPhysicsPointer()->getTransform().getAxisVector(1);
		DragonXVector3 u2 = obj1->GetPhysicsPointer()->getTransform().getAxisVector(2);
		/*DragonXVector3 u0 = obj2->GetPhysicsPointer()->getDirectionInLocalSpace(obj1->GetPhysicsPointer()->getTransform().getAxisVector(0));
		DragonXVector3 u1 = obj2->GetPhysicsPointer()->getDirectionInLocalSpace(obj1->GetPhysicsPointer()->getTransform().getAxisVector(1));
		DragonXVector3 u2 = obj2->GetPhysicsPointer()->getDirectionInLocalSpace(obj1->GetPhysicsPointer()->getTransform().getAxisVector(2));*/

		for (auto DragonSector : obj2->GetPhysicsPointer()->GetSectors())
		{
			if(SphereSphereCollisionTest(obj2->GetPhysicsPointer()->getPointInWorldSpace(DragonSector->GetPosition()),DragonSector->GetRadius(),obj1->GetPhysicsPointer()->getPosition(),obj1->GetPhysicsPointer()->GetRadius()))
			{
				for (auto DragonTriangle : DragonSector->GetDragonTris())
				{
					if (TriangleSphereCollisionTest(DragonTriangle,obj2,obj1))
					{
						if(TriangleObbCollisionTest(DragonTriangle,obj2,obj1))
						{
							return true;
						}
					}
				}
			}
		}
		return false;
	}
}

bool DragonContactGenerator::MeshMeshCollisionTest(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	for(unsigned i = 0; i < obj1->GetPhysicsPointer()->GetSectors().size();i++)
	{
		for(unsigned j =0;j<obj2->GetPhysicsPointer()->GetSectors().size();j++)
		{
			if(SphereSphereCollisionTest(obj1->GetPhysicsPointer()->getPointInWorldSpace(obj1->GetPhysicsPointer()->GetSectors()[i]->GetPosition()),obj1->GetPhysicsPointer()->GetSectors()[i]->GetRadius(),obj2->GetPhysicsPointer()->getPointInWorldSpace(obj2->GetPhysicsPointer()->GetSectors()[j]->GetPosition()),obj2->GetPhysicsPointer()->GetSectors()[j]->GetRadius()))
			{
				for(auto tri1 : obj1->GetPhysicsPointer()->GetSectors()[i]->GetDragonTris())
				{
					for(auto tri2 : obj2->GetPhysicsPointer()->GetSectors()[j]->GetDragonTris())
					{
						if(TestTriTri(tri1,obj1,tri2,obj2))
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool DragonContactGenerator::TriangleObbCollisionTest(DragonTriangle* tri, DragonXVector3& localCenter,DragonXVector3& halfExtents,DragonXVector3& u0,DragonXVector3& u1,DragonXVector3& u2)
{
	DragonXVector3 f0=tri->GetVertB()-tri->GetVertA();
	f0.invert();
	DragonXVector3 f1=tri->GetVertC()-tri->GetVertB();
	f1.invert();
	DragonXVector3 f2=tri->GetVertA()-tri->GetVertC();
	f2.invert();

	// cross product axis tests
	DragonXVector3 axis = u0%f0;
	real p0 = tri->GetVertA()*axis;
	real p1 = tri->GetVertB()*axis;
	real p2 = tri->GetVertC()*axis;
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2),0.0f)>=GetRadiusForObbTrTest(halfExtents,u0,u1,u2,axis))
	{
		return false;
	}

	axis = u0%f1;
	p0 = tri->GetVertA()*axis;
	p1 = tri->GetVertB()*axis;
	p2 = tri->GetVertC()*axis;
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2),0.0f)>=GetRadiusForObbTrTest(halfExtents,u0,u1,u2,axis))
	{
		return false;
	}

	axis = u0%f2;
	p0 = tri->GetVertA()*axis;
	p1 = tri->GetVertB()*axis;
	p2 = tri->GetVertC()*axis;
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2),0.0f)>=GetRadiusForObbTrTest(halfExtents,u0,u1,u2,axis))
	{
		return false;
	}

	axis = u1%f0;
	p0 = tri->GetVertA()*axis;
	p1 = tri->GetVertB()*axis;
	p2 = tri->GetVertC()*axis;
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2),0.0f)>=GetRadiusForObbTrTest(halfExtents,u0,u1,u2,axis))
	{
		return false;
	}

	axis = u1%f1;
	p0 = tri->GetVertA()*axis;
	p1 = tri->GetVertB()*axis;
	p2 = tri->GetVertC()*axis;
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2),0.0f)>=GetRadiusForObbTrTest(halfExtents,u0,u1,u2,axis))
	{
		return false;
	}

	axis = u1%f2;
	p0 = tri->GetVertA()*axis;
	p1 = tri->GetVertB()*axis;
	p2 = tri->GetVertC()*axis;
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2),0.0f)>=GetRadiusForObbTrTest(halfExtents,u0,u1,u2,axis))
	{
		return false;
	}

	axis = u2%f0;
	p0 = tri->GetVertA()*axis;
	p1 = tri->GetVertB()*axis;
	p2 = tri->GetVertC()*axis;
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2),0.0f)>=GetRadiusForObbTrTest(halfExtents,u0,u1,u2,axis))
	{
		return false;
	}

	axis = u2%f1;
	p0 = tri->GetVertA()*axis;
	p1 = tri->GetVertB()*axis;
	p2 = tri->GetVertC()*axis;
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2),0.0f)>=GetRadiusForObbTrTest(halfExtents,u0,u1,u2,axis))
	{
		return false;
	}

	axis = u2%f2;
	p0 = tri->GetVertA()*axis;
	p1 = tri->GetVertB()*axis;
	p2 = tri->GetVertC()*axis;
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2),0.0f)>=GetRadiusForObbTrTest(halfExtents,u0,u1,u2,axis))
	{
		return false;
	}

	if(tri->GetMaxX()<localCenter.x-halfExtents.x||tri->GetMinX()>localCenter.x+halfExtents.x)
	{
		return false;
	}

	if(tri->GetMaxY()<localCenter.y-halfExtents.y||tri->GetMinY()>localCenter.y+halfExtents.y)
	{
		return false;
	}

	if(tri->GetMaxZ()<localCenter.z-halfExtents.z||tri->GetMinZ()>localCenter.z+halfExtents.z)
	{
		return false;
	}

	if((real)abs((tri->GetNormal()*localCenter)-(tri->GetNormal()*tri->GetVertA()))<=GetRadiusForObbTrTest(halfExtents,u0,u1,u2,tri->GetNormal()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DragonContactGenerator::TriangleObbCollisionTest(DragonTriangle* tri,BaseGameEntity* mesh, BaseGameEntity* obb)
{
	DragonXVector3 vertA = obb->GetPhysicsPointer()->getPointInLocalSpace(mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertA()));
	DragonXVector3 vertB = obb->GetPhysicsPointer()->getPointInLocalSpace(mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertB()));
	DragonXVector3 vertC = obb->GetPhysicsPointer()->getPointInLocalSpace(mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertC()));
	DragonXVector3 norm = tri->GetNormal(vertA,vertB,vertC);

	DragonXVector3 f0=vertB-vertA;
	//f0.invert();
	DragonXVector3 f1=vertC-vertB;
	//f1.invert();
	DragonXVector3 f2=vertA-vertC;
	//f2.invert();

	// cross product axis tests
	DragonXVector3 axis = obb->GetPhysicsPointer()->getTransform().getAxisVector(0)%f0;
	real p0 = vertA*axis;
	real p1 = vertB*axis;
	real p2 = vertC*axis;
	if(getMax(-1.0f*getMax(p0,p2),getMin(p0,p2))>transformToAxis(*obb,axis))
	{
		return false;
	}

	axis = obb->GetPhysicsPointer()->getTransform().getAxisVector(0)%f1;
	p0 = vertA*axis;
	p1 = vertB*axis;
	p2 = vertC*axis;
	if(getMax(-1.0f*getMax(p0,p2),getMin(p0,p2))>transformToAxis(*obb,axis))
	{
		return false;
	}

	axis = obb->GetPhysicsPointer()->getTransform().getAxisVector(0)%f2;
	p0 = vertA*axis;
	p1 = vertB*axis;
	p2 = vertC*axis;
	if(getMax(-1.0f*getMax(p1,p2),getMin(p1,p2))>transformToAxis(*obb,axis))
	{
		return false;
	}

	axis = obb->GetPhysicsPointer()->getTransform().getAxisVector(1)%f0;
	p0 = vertA*axis;
	p1 = vertB*axis;
	p2 = vertC*axis;
	if(getMax(-1.0f*getMax(p0,p2),getMin(p0,p2))>transformToAxis(*obb,axis))
	{
		return false;
	}

	axis = obb->GetPhysicsPointer()->getTransform().getAxisVector(1)%f1;
	p0 = vertA*axis;
	p1 = vertB*axis;
	p2 = vertC*axis;
	if(getMax(-1.0f*getMax(p0,p2),getMin(p0,p2))>transformToAxis(*obb,axis))
	{
		return false;
	}

	axis = obb->GetPhysicsPointer()->getTransform().getAxisVector(1)%f2;
	p0 = vertA*axis;
	p1 = vertB*axis;
	p2 = vertC*axis;
	if(getMax(-1.0f*getMax(p0,p1),getMin(p0,p1))>transformToAxis(*obb,axis))
	{
		return false;
	}

	axis = obb->GetPhysicsPointer()->getTransform().getAxisVector(2)%f0;
	p0 = vertA*axis;
	p1 = vertB*axis;
	p2 = vertC*axis;
	if(getMax(-1.0f*getMax(p0,p2),getMin(p0,p2))>transformToAxis(*obb,axis))
	{
		return false;
	}

	axis = obb->GetPhysicsPointer()->getTransform().getAxisVector(2)%f1;
	p0 = vertA*axis;
	p1 = vertB*axis;
	p2 = vertC*axis;
	if(getMax(-1.0f*getMax(p0,p1),getMin(p0,p1))>transformToAxis(*obb,axis))
	{
		return false;
	}

	axis = obb->GetPhysicsPointer()->getTransform().getAxisVector(2)%f2;
	p0 = vertA*axis;
	p1 = vertB*axis;
	p2 = vertC*axis;
	if(getMax(-1.0f*getMax(p1,p2),getMin(p1,p2))>transformToAxis(*obb,axis))
	{
		return false;
	}

	if(getMax(vertA.x,vertB.x,vertC.x)<-1.0f*obb->GetPhysicsPointer()->GetHalfSizes().x||getMin(vertA.x,vertB.x,vertC.x)>obb->GetPhysicsPointer()->GetHalfSizes().x)
	{
		return false;
	}

	if(getMax(vertA.y,vertB.y,vertC.y)<-1.0f*obb->GetPhysicsPointer()->GetHalfSizes().y||getMin(vertA.y,vertB.y,vertC.y)>obb->GetPhysicsPointer()->GetHalfSizes().y)
	{
		return false;
	}

	if(getMax(vertA.z,vertB.z,vertC.z)<-1.0f*obb->GetPhysicsPointer()->GetHalfSizes().z||getMin(vertA.z,vertB.z,vertC.z)>obb->GetPhysicsPointer()->GetHalfSizes().z)
	{
		return false;
	}

	if((real)abs(norm*obb->GetPhysicsPointer()->getPointInLocalSpace(obb->GetPhysicsPointer()->getPosition()))-(norm*vertA)<=transformToAxis(*obb,norm))
	{
		return true;// note seemed to work better with out the transformation of the normal maybe     NOTE i think i need to transform the normals to mesh local direction then world direction
	}
	else
	{
		return false;
	}
}

bool DragonContactGenerator::TestTriTri(DragonTriangle* tri1,BaseGameEntity* mesh1,DragonTriangle* tri2,BaseGameEntity* mesh2)
{
	//create local temp verts 4 each triangle in world space
	// tri 1
	DragonXVector3 tri1A = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA());
	DragonXVector3 tri1B = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertB());
	DragonXVector3 tri1C = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC());
	//tri2
	DragonXVector3 tri2A = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA());
	DragonXVector3 tri2B = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertB());
	DragonXVector3 tri2C = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC());
	DragonXVector3 tri1Norm = tri1->GetNormal(tri1A,tri1B,tri1C);
	DragonXVector3 tri2Norm = tri2->GetNormal(tri2A,tri2B,tri2C);
	real du0=0.0f;
	real du1=0.0f;
	real du2=0.0f;
	real dv0=0.0f;
	real dv1=0.0f;
	real dv2 = 0.0f;

	if(!TestTriVertsAgainstPlane(tri1A,tri1B,tri1C,tri2Norm,tri2Norm*tri2C,du0,du1,du2))
	{
		return false;
	}
	////pen depths of each vert of tri1 into tri 2 0 corespends to verta 1 to b...
	real du0du1=du0*du1;
	real du0du2=du0*du2;
	//if(du0du1>0.0f&&du0du2>0.0f)
	//{
	//	return false;
	//}

	if(!TestTriVertsAgainstPlane(tri2A,tri2B,tri2C,tri1Norm,tri1Norm*tri1C,dv0,dv1,dv2))
	{
		return false;
	}
	//// same here just switched
	real dv0dv1=dv0*dv1;
	real dv0dv2=dv0*dv2;

	//if(dv0dv1>0.0f&&dv0dv2>0.0f)
	//{
	//	return false;
	//}

	DragonXVector3 d = tri1Norm%tri2Norm;
	real max = (real)abs(d[0]);
	int index = 0;
	real bb = (real)abs(d[1]);
	real cc = (real)abs(d[2]);
	if(bb>max)
	{
		max=bb;
		index=1;
	}

	if(cc>max)
	{
		max=cc;
		index=2;
	}

	real vp0 = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA())[index];
	real vp1 = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertB())[index];
	real vp2 = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC())[index];

	real up0 = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA())[index];
	real up1 = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertB())[index];
	real up2 = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC())[index];

	real a,b,c,x0,x1;
	if(ComputeIntervals(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv1,a,b,c,x0,x1))
	{
		if(CoplanarTriTest(tri1Norm,mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertB()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertB()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC())))
		{
			return true;
		}
	}

	real D,e,f,y0,y1;
	if(ComputeIntervals(up0,up1,up2,du0,du1,du2,du0du1,du0du2,D,e,f,y0,y1))
	{
		if(CoplanarTriTest(tri1Norm,mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertB()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertB()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC())))
		{
			return true;
		}
	}
	// now compute the edges
	DragonXVector3 tri1ba = tri1B-tri1A;
	DragonXVector3 tri1cb = tri1C-tri1B;
	DragonXVector3 tri1ac = tri1A-tri1C;
	//tri2
	DragonXVector3 tri2ba = tri2B-tri2A;
	DragonXVector3 tri2cb = tri2C-tri2B;
	DragonXVector3 tri2ac = tri2A-tri2C;

	real pu0,pu1,pu2,pv0,pv1,pv2;

	// 9 cross product axis tests
	// set 1
	DragonXVector3 testAxis = tri1ba%tri2ba;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)<getMin(pu0,pu1,pu2))
		{
			return false;
		}
		if(getMin(pv0,pv1,pv2)>getMax(pu0,pu1,pu2))
		{
			return false;
		}
	}

	testAxis = tri1ba%tri2cb;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)<getMin(pu0,pu1,pu2))
		{
			return false;
		}
		if(getMin(pv0,pv1,pv2)>getMax(pu0,pu1,pu2))
		{
			return false;
		}
	}

	testAxis = tri1ba%tri2ac;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)<getMin(pu0,pu1,pu2))
		{
			return false;
		}
		if(getMin(pv0,pv1,pv2)>getMax(pu0,pu1,pu2))
		{
			return false;
		}
	}
	//set2
	testAxis = tri1cb%tri2ba;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)<getMin(pu0,pu1,pu2))
		{
			return false;
		}
		if(getMin(pv0,pv1,pv2)>getMax(pu0,pu1,pu2))
		{
			return false;
		}
	}

	testAxis = tri1cb%tri2cb;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)<getMin(pu0,pu1,pu2))
		{
			return false;
		}
		if(getMin(pv0,pv1,pv2)>getMax(pu0,pu1,pu2))
		{
			return false;
		}
	}

	testAxis = tri1cb%tri2ac;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)<getMin(pu0,pu1,pu2))
		{
			return false;
		}
		if(getMin(pv0,pv1,pv2)>getMax(pu0,pu1,pu2))
		{
			return false;
		}
	}
	// set3
	testAxis = tri1ac%tri2ba;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)<getMin(pu0,pu1,pu2))
		{
			return false;
		}
		if(getMin(pv0,pv1,pv2)>getMax(pu0,pu1,pu2))
		{
			return false;
		}
	}

	testAxis = tri1ac%tri2cb;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)<getMin(pu0,pu1,pu2))
		{
			return false;
		}
		if(getMin(pv0,pv1,pv2)>getMax(pu0,pu1,pu2))
		{
			return false;
		}
	}

	testAxis = tri1ac%tri2ac;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)<getMin(pu0,pu1,pu2))
		{
			return false;
		}
		if(getMin(pv0,pv1,pv2)>getMax(pu0,pu1,pu2))
		{
			return false;
		}
	}
	return true;
}

bool DragonContactGenerator::TestTriVertsAgainstPlane(DragonXVector3& vertA,DragonXVector3& vertB,DragonXVector3& vertC,DragonXVector3& planeNormal,real planeDist,real& d0,real& d1,real& d2)
{
	d0 = DistPointPlane(vertA,planeNormal,planeDist);
	d1 = DistPointPlane(vertB,planeNormal,planeDist);
	d2 = DistPointPlane(vertC,planeNormal,planeDist);

	if(d0>0&&d1>0&&d2>0)
	{
		return false;
	}
	else if(d0<0&&d1<0&&d2<0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool DragonContactGenerator::MeshRay(BaseGameEntity* obj1, BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh)// if obj1 is the mesh
	{
		for (auto DragonSector : obj1->GetPhysicsPointer()->GetSectors())
		{
			if(SectorRayCollisionDetection(obj1->GetPhysicsPointer()->getPointInWorldSpace(DragonSector->GetPosition()),DragonSector->GetRadius(),obj2->GetPhysicsPointer()->getPosition(),obj2->GetPhysicsPointer()->GetNormal()))
			{
				for (auto DragonTriangle : DragonSector->GetDragonTris())
				{
					if (TriangleRayCollisionTest(obj1->GetPhysicsPointer()->getPointInWorldSpace(DragonTriangle->GetVertA()),obj1->GetPhysicsPointer()->getPointInWorldSpace(DragonTriangle->GetVertB()),obj1->GetPhysicsPointer()->getPointInWorldSpace(DragonTriangle->GetVertC()),obj2->GetPhysicsPointer()->getPosition(),obj2->GetPhysicsPointer()->GetNormal()))
					{
						return true;
					}
				}
			}
		}
		return false;
	}
	else // otherwise obj2 is the mesh
	{
		for (auto DragonSector : obj2->GetPhysicsPointer()->GetSectors())
		{
			if(SectorRayCollisionDetection(obj2->GetPhysicsPointer()->getPointInWorldSpace(DragonSector->GetPosition()),DragonSector->GetRadius(),obj1->GetPhysicsPointer()->getPosition(),obj1->GetPhysicsPointer()->GetNormal()))
			{
				for (auto DragonTriangle : DragonSector->GetDragonTris())
				{
					if (TriangleRayCollisionTest(obj2->GetPhysicsPointer()->getPointInWorldSpace(DragonTriangle->GetVertA()),obj2->GetPhysicsPointer()->getPointInWorldSpace(DragonTriangle->GetVertB()),obj2->GetPhysicsPointer()->getPointInWorldSpace(DragonTriangle->GetVertC()),obj1->GetPhysicsPointer()->getPosition(),obj1->GetPhysicsPointer()->GetNormal()))
					{
						return true;
					}
				}
			}
		}
		return false;
	}
}

bool DragonContactGenerator::SectorRayCollisionDetection(DragonXVector3& spherePos,real rad,DragonXVector3& rayOrigin,DragonXVector3& rayDirection)
{
	DragonXVector3 m = rayOrigin-spherePos;
	real c = (m*m)-(rad*rad);

	if(c<=0.0f)
	{
		return true;
	}

	real b = m*rayDirection;

	if(b>0.0f)
	{
		return false;
	}

	real disc = b*b-c;

	if(disc < 0.0f)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool DragonContactGenerator::TriangleRayCollisionTest(DragonXVector3& triVertA,DragonXVector3& triVertB,DragonXVector3& triVertC,DragonXVector3& rayPos,DragonXVector3& rayDir)
{
	DragonXVector3 u = triVertB-triVertA;
	DragonXVector3 v = triVertC-triVertA;
	DragonXVector3 n = u%v;

	DragonXVector3 w0 = rayPos-triVertA;
	real a = -1.0f*(n*w0);
	real b = n*rayDir;
	if((real)abs(b)<0.00005f)
	{
		if(a!=0)
		{
			return false;
		}
	}

	real r = a/b;
	if(r<0.0f)
	{
		return false;
	}

	DragonXVector3 I = rayPos + r * rayDir;
	real uu,uv,vv,wu,wv,d;
	uu = u*u;
	uv=u*v;
	vv=v*v;
	DragonXVector3 w = I - triVertA;
	wu=w*u;
	wv=w*v;
	d = uv*uv-uu*vv;

	real s,t;
	s = (uv*wv-vv*wu)/d;
	if(s<0.0f||s>1.0f)
	{
		return false;
	}
	t = (uv*wu-uu*wv)/d;
	if(t<0.0f||(s+t)>1.0f)
	{
		return false;
	}

	return true;
}