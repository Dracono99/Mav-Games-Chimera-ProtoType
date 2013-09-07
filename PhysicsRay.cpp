#include "PhysicsRay.h"
#include "DragonEnums.h"

PhysicsRay::PhysicsRay():BasePhysicsEntity()
{
	mI_PhysicsType=COT_Game;
	mI_PrimitiveType=PT_Ray;
	inverseInertiaTensor=DragonXMatrix().getInverse();
}
PhysicsRay::~PhysicsRay()
{
}
DragonXVector3 PhysicsRay::GetNormal()
{
	return DragonXVector3(2.0f*(m_QuatRot.x*m_QuatRot.z+m_QuatRot.w*m_QuatRot.y),
		2.0f*(m_QuatRot.y*m_QuatRot.z-m_QuatRot.w*m_QuatRot.x),
		1.0f-2.0f*(m_QuatRot.x*m_QuatRot.x+m_QuatRot.y*m_QuatRot.y));
}