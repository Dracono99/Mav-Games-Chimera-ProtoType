#pragma once

#include "BasePhysicsEntity.h"

class PhysicsSphere : public BasePhysicsEntity
{
private:
	DragonXMatrix calcInverseInertialTensor(real rad,real mass);
	real m_Radius;
public:
	PhysicsSphere();
	virtual ~PhysicsSphere(){}
	PhysicsSphere(DragonXVector3 Position);
	PhysicsSphere(real Radius);
	PhysicsSphere(DragonXVector3 Position, real Radius);
	PhysicsSphere(DragonXVector3 Position, real Radius, bool isCollisionResolutionOn);
	PhysicsSphere(DragonXVector3 Position, bool isCollisionDetectionOn);
	virtual real GetRadius();
	void SetRadius(real radius);
	virtual void SetMassAndInverseTensor(real newMass)
	{
		m_Mass=newMass;
		m_InverseMass=1.0f/newMass;
		inverseInertiaTensor=calcInverseInertialTensor(m_Radius,newMass);
	}
};
