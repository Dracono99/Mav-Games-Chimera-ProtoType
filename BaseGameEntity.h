#pragma once
//#include "DragonXMath.h"
#include "BasePhysicsEntity.h"
#include "BaseGraphicsEntity.h"
#include "BaseGameplayObject.h"

class BaseGameEntity
{
private:
	BaseGraphicsEntity* mGraphics;
	BasePhysicsEntity* mPhysics;
	BaseGameplayObject* mGamePlay;
public:
	BaseGameEntity(){
		mGraphics=NULL;
		mPhysics=NULL;
		mGamePlay=NULL;
	}
	BaseGameEntity(BaseGraphicsEntity* graphicsPointer,BasePhysicsEntity* physicsPointer)
	{
		mGraphics=graphicsPointer;
		mPhysics=physicsPointer;
	}
	~BaseGameEntity();
	BaseGraphicsEntity* GetGraphicsPointer()
	{
		return mGraphics;
	}
	BasePhysicsEntity* GetPhysicsPointer()
	{
		return mPhysics;
	}
	BaseGameplayObject* GetGamePlayObject()
	{
		return mGamePlay;
	}
	void SetBaseGraphicsEntity(BaseGraphicsEntity* graphics)
	{
		mGraphics=graphics;
	}
	void SetBasePhysicsEntity(BasePhysicsEntity* physics)
	{
		mPhysics=physics;
	}
	void SetBaseGamePlayObject(BaseGameplayObject* gameplay)
	{
		mGamePlay=gameplay;
	}
};
