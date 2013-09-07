#include "MainGameMenuState.h"
#include "MainGameManager.h"
#include "ChimeraGameEngine.h"
#include "MenuFSM.h"
#include "MainGameStateIDs.h"
#include "MenuStateIds.h"

MainGameMenuState::MainGameMenuState()
{
	mStateID=MainGameMenuStateID;
}

MainGameMenuState::~MainGameMenuState()
{
}

void MainGameMenuState::Enter()
{
	gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(MainMenuState);
}

void MainGameMenuState::Update(float dt)
{
	gd3dApp->GetEngine()->GetMenuSystem()->Update(dt);
}

void MainGameMenuState::Render()
{
	gd3dApp->GetEngine()->GetMenuSystem()->Render();
}

void MainGameMenuState::OnLostDevice()
{
}

void MainGameMenuState::OnResetDevice()
{
}

void MainGameMenuState::Exit()
{
}