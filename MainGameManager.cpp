#include "MainGameManager.h"
#include "ChimeraGameEngine.h"
#include "MainGameMenuState.h"
#include "MainGameCampaignState.h"
#include "MainGameStateIDs.h"
#include "MainMultiPlayerGameState.h"
#include "MultiPlayerCore.h"

MainGameManager::MainGameManager(HINSTANCE hInstance,HWND windowHandle, D3DDEVTYPE devType, DWORD requestedVP)
	: D3DApp(hInstance,"Chimera Game Engine Demo",devType,requestedVP,windowHandle)
{
	gd3dApp = this;
	ClosedApp=false;
	mGfxStats = new GfxStats();
	m_instanceHandle = hInstance;
	m_windowHandle = windowHandle;
	m_Engine = new ChimeraGameEngine();
	m_Engine->InitializeMenuSystem();
	m_pStateMachine = new stateMachine<MainGameManager>(this);
	m_vStates.push_back(new MainGameMenuState());
	//m_vStates.push_back(new MainGameCampaignState());
	m_pStateMachine->SetCurrentState(NULL);
	m_pStateMachine->ChangeState(m_vStates[0]);
}
MainGameManager::~MainGameManager()
{
	delete m_Engine;
	m_pStateMachine->CurrentState()->Exit();//must call exit to release audio before deletion of current state as exit is the releaser of the sound files
	delete mGfxStats;
	for(auto states : m_vStates)
	{
		delete states;
	}
	delete m_pStateMachine;
}
stateMachine<MainGameManager>* MainGameManager::GetFsm()
{
	return m_pStateMachine;
}
int MainGameManager::Run()
{
	MSG  msg;
	msg.message = WM_NULL;

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
		{
			if(isAppPaused()&&(m_Engine->GetMultiPlayerCore()->GetOnlineStatus()!=true))
			{
				Sleep(20);
				continue;
			}
			if(!isDeviceLost())
			{
				__int64 currTimeStamp = 0;
				QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
				float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;
				updateScene(0.016f);// 0.016f gets me basically 60fps regardless of actual render rate the code will see 60 fps constant
				if (!ClosedApp)
				{
					m_pStateMachine->CurrentState()->Render();
					prevTimeStamp = currTimeStamp;
				}
			}
		}
	}
	return (int)msg.wParam;
}
void MainGameManager::onLostDevice()
{
	mGfxStats->onLostDevice();
	m_Engine->OnLost();
	GetFsm()->CurrentState()->OnLostDevice();
}
void MainGameManager::onResetDevice()
{
	mGfxStats->onResetDevice();
	m_Engine->OnReset();
	GetFsm()->CurrentState()->OnResetDevice();
}
void MainGameManager::updateScene(float dt)
{
	mGfxStats->update(dt);
	m_Engine->Update(dt);
	if (!ClosedApp)
	{
		GetFsm()->CurrentState()->Update(dt);
	}
}
void MainGameManager::DisplayStats()
{
	mGfxStats->display();
}
GfxStats* MainGameManager::GetStats()
{
	return mGfxStats;
}
ChimeraGameEngine* MainGameManager::GetEngine()
{
	return m_Engine;
}
void MainGameManager::SwitchGameState(int stateID)
{
	int idx;
	switch (stateID)
	{
	case MainCampaignDemoStateID:
		{
			if(DoesGameStateExist(stateID,idx))
			{
				m_pStateMachine->ChangeState(m_vStates[idx]);
				break;
			}
			else
			{
				MainGameCampaignState* m = new MainGameCampaignState();
				m_vStates.push_back(m);
				m_pStateMachine->ChangeState(m);
				break;
			}
		}
	case MainGameMenuStateID:
		{
			if(DoesGameStateExist(stateID,idx))
			{
				m_pStateMachine->ChangeState(m_vStates[idx]);
				break;
			}
			else
			{
				MainGameMenuState* m = new MainGameMenuState();
				m_vStates.push_back(m);
				m_pStateMachine->ChangeState(m);
				break;
			}
		}
	case MainMultiPlayerGameStateID:
		{
			if(DoesGameStateExist(stateID,idx))
			{
				m_pStateMachine->ChangeState(m_vStates[idx]);
				break;
			}
			else
			{
				MainMultiPlayerGameState* m = new MainMultiPlayerGameState();
				m_vStates.push_back(m);
				m_pStateMachine->ChangeState(m);
				break;
			}
		}
	default:
		break;
	}
}
bool MainGameManager::DoesGameStateExist(int stateID,int& i)
{
	for(int unsigned idx = 0; idx < m_vStates.size(); idx++)
	{
		if(m_vStates[idx]->mStateID == stateID)
		{
			i = idx;
			return true;
		}
	}
	return false;
}