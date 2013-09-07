#pragma once
#include "FSMnStates.h"
#include <vector>
class FreeForm3DCamera;
class BaseGameEntity;

class MainMultiPlayerGameState : public state
{
private:
	bool m_GamePaused;
	bool m_GameInSession;
	bool m_MoveForward;
	bool m_MoveForwardState;
	bool m_MoveBackward;
	bool m_MoveBackwardState;
	bool m_MoveRight;
	bool m_MoveRightState;
	bool m_MoveLeft;
	bool m_MoveLeftState;
	bool m_canFire;
	FreeForm3DCamera* m_Camera;
	BaseGameEntity* m_Player;
	BaseGameEntity* m_Galaxy;
	std::vector<BaseGameEntity*> m_Entities;
public:
	MainMultiPlayerGameState();
	~MainMultiPlayerGameState();
	void PlayerFire();
	void Enter();
	void Update( float dt);
	void Render();
	void OnResetDevice();
	void OnLostDevice();
	void Exit();
};