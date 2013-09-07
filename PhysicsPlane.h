#pragma once

#include "BasePhysicsEntity.h"

class PhysicsPlane : public BasePhysicsEntity
{
private:
	DragonXVector3 m_Normal;
	real m_DistFormOrigin;
public:
	PhysicsPlane();
	PhysicsPlane(DragonXVector3 normal,real distFromOrigin);
	~PhysicsPlane();
	DragonXVector3 GetNormal();
	real GetDistFromOrigin();
};