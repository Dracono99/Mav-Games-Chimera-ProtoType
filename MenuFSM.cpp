#include "MenuFSM.h"
#include "MainMenu.h"
#include "MainOptionsMenu.h"
#include "MenuStateIds.h"
#include "MainAudioSettingsMenu.h"
#include "GameMenu.h"
#include "GamePauseMenu.h"
#include "MainMultiPlayerMenuState.h"
#include "MultiplayerGameWaitingState.h"
#include "MultiPlayerGamePlayMenu.h"
#include "MultiplayerGamePlayPauseMenu.h"
#include "MultiplayerGameOverMenu.h"

Menu::Menu()
{
	m_pStateMachine = new stateMachine<Menu>(this);
	state* mainMenu = new MainMenu();
	m_vMenuSates.push_back(mainMenu);
	m_pStateMachine->ChangeState(mainMenu);
}
Menu::~Menu()
{
	for (auto states : m_vMenuSates)
	{
		delete states;
		states=NULL;
	}
	delete m_pStateMachine;
}

bool Menu::doesMenuStateExist(int menuState,int& i)
{
	for(int unsigned idx = 0; idx < m_vMenuSates.size(); idx++)
	{
		if(m_vMenuSates[idx]->mStateID == menuState)
		{
			i = idx;
			return true;
		}
	}
	return false;
}

void Menu::SwitchMenuState(int menuStateId)
{
	int idx = 0;

	switch(menuStateId)
	{
	case MainMenuState:
		{
			if(doesMenuStateExist(menuStateId,idx))
			{
				m_pStateMachine->ChangeState(m_vMenuSates[idx]);
			}
			else
			{
				state* mainMenu = new MainMenu();
				m_vMenuSates.push_back(mainMenu);
				m_pStateMachine->ChangeState(mainMenu);
			}
			break;
		}
	case MainOptionsMenuState:
		{
			if(doesMenuStateExist(menuStateId,idx))
			{
				m_pStateMachine->ChangeState(m_vMenuSates[idx]);
			}
			else
			{
				state* mainOptionsMenu = new MainOptionsMenu();
				m_vMenuSates.push_back(mainOptionsMenu);
				m_pStateMachine->ChangeState(mainOptionsMenu);
			}
			break;
		}
	case MainAudioSettingsMenuState:
		{
			if(doesMenuStateExist(menuStateId,idx))
			{
				m_pStateMachine->ChangeState(m_vMenuSates[idx]);
			}
			else
			{
				state* mainOptionsMenu = new MainAudioSettingsMenu();
				m_vMenuSates.push_back(mainOptionsMenu);
				m_pStateMachine->ChangeState(mainOptionsMenu);
			}
			break;
		}
	case GamePlayMenu:
		{
			if(doesMenuStateExist(menuStateId,idx))
			{
				m_pStateMachine->ChangeState(m_vMenuSates[idx]);
			}
			else
			{
				state* mainOptionsMenu = new GameMenu();
				m_vMenuSates.push_back(mainOptionsMenu);
				m_pStateMachine->ChangeState(mainOptionsMenu);
			}
			break;
		}
	case GamePauseMenuState:
		{
			if(doesMenuStateExist(menuStateId,idx))
			{
				m_pStateMachine->ChangeState(m_vMenuSates[idx]);
			}
			else
			{
				state* mainOptionsMenu = new GamePauseMenu();
				m_vMenuSates.push_back(mainOptionsMenu);
				m_pStateMachine->ChangeState(mainOptionsMenu);
			}
			break;
		}
	case MainMultiPlayerMenuStateID:
		{
			if(doesMenuStateExist(menuStateId,idx))
			{
				m_pStateMachine->ChangeState(m_vMenuSates[idx]);
			}
			else
			{
				state* mainOptionsMenu = new MainMultiPlayerMenuState();
				m_vMenuSates.push_back(mainOptionsMenu);
				m_pStateMachine->ChangeState(mainOptionsMenu);
			}
			break;
		}
	case MultiplayerGameWaitingStateID:
		{
			if(doesMenuStateExist(menuStateId,idx))
			{
				m_pStateMachine->ChangeState(m_vMenuSates[idx]);
			}
			else
			{
				state* mainOptionsMenu = new MPwaitingGameMenu();
				m_vMenuSates.push_back(mainOptionsMenu);
				m_pStateMachine->ChangeState(mainOptionsMenu);
			}
			break;
		}
	case MultiplayerGameplayMenuStateID:
		{
			if(doesMenuStateExist(menuStateId,idx))
			{
				m_pStateMachine->ChangeState(m_vMenuSates[idx]);
			}
			else
			{
				state* mainOptionsMenu = new MultiplayerGamePlayMenu();
				m_vMenuSates.push_back(mainOptionsMenu);
				m_pStateMachine->ChangeState(mainOptionsMenu);
			}
			break;
		}
	case MultiplayerGamePlayPauseMenuStateID:
		{
			if(doesMenuStateExist(menuStateId,idx))
			{
				m_pStateMachine->ChangeState(m_vMenuSates[idx]);
			}
			else
			{
				state* mainOptionsMenu = new MultiplayerGamePlayPauseMenu();
				m_vMenuSates.push_back(mainOptionsMenu);
				m_pStateMachine->ChangeState(mainOptionsMenu);
			}
			break;
		}
	case MultiplayerGameOverStateID:
		{
			if(doesMenuStateExist(menuStateId,idx))
			{
				m_pStateMachine->ChangeState(m_vMenuSates[idx]);
			}
			else
			{
				state* mainOptionsMenu = new MultiplayerGameOverMenu();
				m_vMenuSates.push_back(mainOptionsMenu);
				m_pStateMachine->ChangeState(mainOptionsMenu);
			}
			break;
		}
	default:
		{
			break;
		}
	};
}

void Menu::Enter()
{
	m_pStateMachine->CurrentState()->Enter();
}
void Menu::Update( float dt)
{
	m_pStateMachine->CurrentState()->Update(dt);
}
void Menu::Render()
{
	m_pStateMachine->CurrentState()->Render();// draws the gui elements
}
void Menu::OnResetDevice()
{
	for (auto menus : m_vMenuSates)
	{
		menus->OnResetDevice();
	}
}
void Menu::OnLostDevice()
{
	for (auto menus : m_vMenuSates)
	{
		menus->OnLostDevice();
	}
}
void Menu::Exit()
{
	m_pStateMachine->CurrentState()->Exit();
}