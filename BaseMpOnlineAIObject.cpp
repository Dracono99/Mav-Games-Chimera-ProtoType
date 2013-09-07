#include "BaseMpOnlineAIObject.h"
#include "BaseGameEntity.h"

BaseMpOnlineAIObject::BaseMpOnlineAIObject()
{
	m_Owner=NULL;
	m_TargetLocation = D3DXVECTOR3(0.0f,0.0f,0.0f);
}

BaseMpOnlineAIObject::~BaseMpOnlineAIObject()
{
}

void BaseMpOnlineAIObject::SetMpLocation(float xPos,float zPos)
{
	m_TargetLocation.x=xPos;
	m_TargetLocation.z=zPos;
}

void BaseMpOnlineAIObject::UpdateOnlineEntity(float dt)
{
	D3DXVECTOR3 normDir = (m_TargetLocation-m_Owner->GetPhysicsPointer()->m_Position);
	float mag = D3DXVec3LengthSq(&normDir);
	D3DXVec3Normalize(&normDir,&normDir);
	m_Owner->GetPhysicsPointer()->addForce((normDir*mag*dt));
}