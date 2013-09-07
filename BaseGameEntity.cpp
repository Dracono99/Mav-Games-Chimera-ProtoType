#include "BaseGameEntity.h"
//#include "BaseGraphicsEntity.h"
//#include "BasePhysicsEntity.h"

BaseGameEntity::~BaseGameEntity()
{
	if (mGraphics!=NULL)
	{
		delete mGraphics;
	}
	if (mPhysics!=NULL)
	{
		delete mPhysics;
	}
	if (mGamePlay!=NULL)
	{
		delete mGamePlay;
	}
}