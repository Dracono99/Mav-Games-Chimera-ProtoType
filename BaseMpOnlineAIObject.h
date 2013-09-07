#pragma once
#include "d3dUtil.h"

class BaseGameEntity;

class BaseMpOnlineAIObject
{
private:

public:
	BaseMpOnlineAIObject();
	~BaseMpOnlineAIObject();
	BaseGameEntity* m_Owner;
	D3DXVECTOR3 m_TargetLocation;
	void UpdateOnlineEntity(float dt);
	void SetMpLocation(float xPos,float zPos);
};