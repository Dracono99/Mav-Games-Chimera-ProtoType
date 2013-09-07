#pragma once

class BaseGameEntity;
struct D3DXVECTOR3;

class MultiPlayerGamePlayObject
{
private:

public:
	MultiPlayerGamePlayObject();
	~MultiPlayerGamePlayObject();
	void SetSeekPoint(D3DXVECTOR3 seek);
	void Update(float dt);
	BaseGameEntity* m_Owner;
};