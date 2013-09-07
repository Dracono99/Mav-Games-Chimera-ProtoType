#pragma once
#include "FSMnStates.h"
#include <vector>
class FreeForm3DCamera;
class BaseGameEntity;

class MainGameCampaignState : public state
{
private:
	bool m_GamePaused;
	bool m_FreeCameraToggle;
	bool mUpdateImposters;
	FreeForm3DCamera* m_Camera;
	BaseGameEntity* m_Player;
	BaseGameEntity* m_Galaxy;
	std::vector<BaseGameEntity*> m_Entities;
	void FireShip();
public:
	MainGameCampaignState();
	~MainGameCampaignState();
	void Enter();
	void Update( float dt);
	void Render();
	void OnResetDevice();
	void OnLostDevice();
	void Exit();
	bool mCanFire;
};