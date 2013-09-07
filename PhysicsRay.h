#pragma once

#include "BasePhysicsEntity.h"

class PhysicsRay : public BasePhysicsEntity
{
private:

public:
	PhysicsRay();
	~PhysicsRay();
	DragonXVector3 GetNormal();
};