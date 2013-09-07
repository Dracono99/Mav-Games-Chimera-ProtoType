#include "PhysicsOBB.h"
#include "DragonEnums.h"

PhysicsOBB::PhysicsOBB():PhysicsSphere()
{
	mI_PrimitiveType=PT_OBB;
	m_HalfSizes=DragonXVector3(0.5f,0.5f,0.5f);
	SetRadius(1.0);
	inverseInertiaTensor=calcInverseInertialTensor(m_HalfSizes,m_Mass);
}
PhysicsOBB::PhysicsOBB(DragonXVector3 pos):PhysicsSphere(pos)
{
	mI_PrimitiveType=PT_OBB;
	m_HalfSizes=DragonXVector3(0.5f,0.5f,0.5f);
	SetRadius(1.0);
	inverseInertiaTensor=calcInverseInertialTensor(m_HalfSizes,m_Mass);
}
PhysicsOBB::PhysicsOBB(DragonXVector3 pos,DragonXVector3 halfSizes):PhysicsSphere(pos)
{
	mI_PrimitiveType=PT_OBB;
	m_HalfSizes=halfSizes;
	SetRadius(m_HalfSizes.GetMagnitude());
	inverseInertiaTensor=calcInverseInertialTensor(m_HalfSizes,m_Mass);
}
PhysicsOBB::PhysicsOBB(DragonXVector3 pos,DragonXVector3 halfSizes,real mass):PhysicsSphere(pos)
{
	mI_PrimitiveType=PT_OBB;
	m_HalfSizes=halfSizes;
	SetRadius(m_HalfSizes.GetMagnitude());
	m_Mass=mass;
	inverseInertiaTensor=calcInverseInertialTensor(m_HalfSizes,m_Mass);
}
DragonXVector3 PhysicsOBB::GetHalfSizes()
{
	return m_HalfSizes;
}
DragonXMatrix PhysicsOBB::calcInverseInertialTensor(DragonXVector3 halfExtents,real mass)
{
	DragonXMatrix m = DragonXMatrix();
	D3DXMatrixIdentity(&m);
	m._11=(real)0.08334*mass*((halfExtents.y*halfExtents.y)+(halfExtents.z*halfExtents.z));
	m._22=(real)0.08334*mass*((halfExtents.x*halfExtents.x)+(halfExtents.z*halfExtents.z));
	m._33=(real)0.08334*mass*((halfExtents.x*halfExtents.x)+(halfExtents.y*halfExtents.y));
	D3DXMatrixInverse(&m,NULL,&m);
	return m;
}