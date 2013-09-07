#pragma once

#include "d3dApp.h"
#include <vector>

class MainGameManager : public D3DApp
{
public:
	MainGameManager(HINSTANCE hInstance,HWND windowHandle, D3DDEVTYPE devType, DWORD requestedVP);
	HINSTANCE m_instanceHandle;
	HWND m_windowHandle;
	stateMachine<MainGameManager>* GetFsm();
	~MainGameManager();
	int Run();
	void onLostDevice();
	void onResetDevice();
	void updateScene(float dt);
	void DisplayStats();
	void SwitchGameState(int stateID);
	bool DoesGameStateExist(int stateID,int& i);
	ChimeraGameEngine* GetEngine();
	GfxStats* GetStats();
	bool ClosedApp;
private:
	ChimeraGameEngine* m_Engine;
	std::vector<state*> m_vStates;
	GfxStats* mGfxStats;
	stateMachine<MainGameManager>* m_pStateMachine;
};
