#pragma once

#include "PhysicsSphere.h"

class PhysicsOBB : public PhysicsSphere
{
private:
	DragonXVector3 m_HalfSizes;
	DragonXMatrix calcInverseInertialTensor(DragonXVector3 halfExtents,real mass);
public:
	PhysicsOBB();
	PhysicsOBB(DragonXVector3 pos);
	PhysicsOBB(DragonXVector3 pos,DragonXVector3 halfSizes);
	PhysicsOBB(DragonXVector3 pos,DragonXVector3 halfSizes,real mass);
	virtual ~PhysicsOBB()
	{
	}
	DragonXVector3 GetHalfSizes();
	void setHalfSizes(DragonXVector3 halfextents)
	{
		m_HalfSizes=halfextents;
	}
	virtual void SetMassAndInverseTensor(real newMass)
	{
		m_Mass=newMass;
		m_InverseMass=1.0f/newMass;
		inverseInertiaTensor=calcInverseInertialTensor(m_HalfSizes,newMass);
	}
};