#include "PhysicsPlane.h"
#include "DragonEnums.h"

PhysicsPlane::PhysicsPlane():BasePhysicsEntity()
{
	m_Normal=DragonXVector3(0.0f,1.0f,0.0f);
	m_DistFormOrigin=0.0f;
	//this position is used for debugging and rendering purposes planes do not actually have positions
	//but this way you can know where abouts the plane is
	// need to check some revisions on planes
	// in which if the direction of the normal is toward the origin
	// then the distance is negitive
	m_Position=m_Normal*m_DistFormOrigin;
	m_isAwake=false;
	mI_PhysicsType=COT_World;
	mI_PrimitiveType=PT_Plane;
	m_InverseMass=0.0f;
}
PhysicsPlane::PhysicsPlane(DragonXVector3 normal,real distFromOrigin):BasePhysicsEntity()
{
	m_Normal=normal;
	m_Normal.Normalize();
	m_DistFormOrigin=distFromOrigin;
	m_Position=m_Normal*m_DistFormOrigin;
	m_isAwake=false;
	mI_PhysicsType=COT_World;
	mI_PrimitiveType=PT_Plane;
	m_InverseMass=0.0f;
}
PhysicsPlane::~PhysicsPlane()
{
}
DragonXVector3 PhysicsPlane::GetNormal()
{
	return m_Normal;
}
real PhysicsPlane::GetDistFromOrigin()
{
	return m_DistFormOrigin;
}