#include "PhysicsSphere.h"
#include "BasePhysicsEntity.h"
#include "BaseGameEntity.h"
#include "DragonEnums.h"

PhysicsSphere::PhysicsSphere():BasePhysicsEntity()
{
	mI_PhysicsType=COT_Game;
	mI_PrimitiveType=PT_Sphere;
	m_Radius=1.0;
	inverseInertiaTensor=calcInverseInertialTensor(m_Radius,m_Mass);
}
PhysicsSphere::PhysicsSphere(DragonXVector3 Position):BasePhysicsEntity()
{
	mI_PhysicsType=COT_Game;
	mI_PrimitiveType=PT_Sphere;
	m_Position=Position;
	m_Radius=1.0;
	inverseInertiaTensor=calcInverseInertialTensor(m_Radius,m_Mass);
}
PhysicsSphere::PhysicsSphere(DragonXVector3 Position, bool isCollisionDetectionOn):BasePhysicsEntity()
{
	mI_PhysicsType=COT_Game;
	mI_PrimitiveType=PT_Sphere;
	m_Position=Position;
	m_Radius=1.0;
	m_isCollisionDetectionOn=isCollisionDetectionOn;
	inverseInertiaTensor=calcInverseInertialTensor(m_Radius,m_Mass);
}
PhysicsSphere::PhysicsSphere(real Radius):BasePhysicsEntity()
{
	mI_PhysicsType=COT_Game;
	mI_PrimitiveType=PT_Sphere;
	m_Radius=Radius;
	inverseInertiaTensor=calcInverseInertialTensor(m_Radius,m_Mass);
}
PhysicsSphere::PhysicsSphere(DragonXVector3 Position, real Radius):BasePhysicsEntity()
{
	mI_PhysicsType=COT_Game;
	mI_PrimitiveType=PT_Sphere;
	m_Position=Position;
	m_Radius=Radius;
	inverseInertiaTensor=calcInverseInertialTensor(m_Radius,m_Mass);
}
PhysicsSphere::PhysicsSphere(DragonXVector3 Position, real Radius, bool isCollisionResolutionOn):BasePhysicsEntity()
{
	mI_PhysicsType=COT_Game;
	mI_PrimitiveType=PT_Sphere;
	m_Position=Position;
	m_Radius=Radius;
	m_isCollisionResolutionOn=isCollisionResolutionOn;
	inverseInertiaTensor=calcInverseInertialTensor(m_Radius,m_Mass);
}
real PhysicsSphere::GetRadius()
{
	return m_Radius;
}
void PhysicsSphere::SetRadius(real radius)
{
	m_Radius=radius;
}
DragonXMatrix PhysicsSphere::calcInverseInertialTensor(real rad,real mass)
{
	DragonXMatrix m = DragonXMatrix();
	D3DXMatrixIdentity(&m);
	real i = mass*0.4f*(rad*rad);
	m._11=i;
	m._22=i;
	m._33=i;
	D3DXMatrixInverse(&m,NULL,&m);
	return m;
}