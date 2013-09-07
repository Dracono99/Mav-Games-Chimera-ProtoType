#pragma once
#include "FSMnStates.h"

class MainGameMenuState : public state
{
private:

public:
	MainGameMenuState();
	~MainGameMenuState();
	void Enter();
	void Update( float dt);
	void Render();
	void OnResetDevice();
	void OnLostDevice();
	void Exit();
};