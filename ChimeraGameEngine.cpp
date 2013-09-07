#include "ChimeraGameEngine.h"
#include "DragonPhysicsEngine.h"
#include "MessageSystem.h"
#include "MenuFSM.h"
#include "InputCore.h"
#include "AudioCore.h"
#include "GraphicsCore.h"
#include "GamePlayCore.h"
#include "MultiPlayerCore.h"

ChimeraGameEngine::ChimeraGameEngine()
{
	m_DragonPhysicsCore=new DragonPhysicsEngine();
	m_MessageSystem=new EngineMessageSystem();
	m_InputCore=new InputCore();
	m_AudioCore=new AudioCore();
	m_GraphicsCore=new GraphicsCore();
	m_GamePlayCore=new GamePlayCore();
	m_MultiPlayerCore=new MultiPlayerCore();
}
ChimeraGameEngine::~ChimeraGameEngine()
{
	if (m_DragonPhysicsCore!=NULL)
	{
		delete m_DragonPhysicsCore;
		m_DragonPhysicsCore=NULL;
	}
	delete m_MessageSystem;
	delete m_InputCore;
	delete m_GamePlayCore;
	delete m_EngineMenuSystem;
	delete m_AudioCore;
	delete m_MultiPlayerCore;
	if (m_GraphicsCore!=NULL)
	{
		delete m_GraphicsCore;
	}
}

DragonPhysicsEngine* ChimeraGameEngine::GetPhysicsCore()
{
	return m_DragonPhysicsCore;
}

EngineMessageSystem* ChimeraGameEngine::GetMessageSystem()
{
	return m_MessageSystem;
}

Menu* ChimeraGameEngine::GetMenuSystem()
{
	return m_EngineMenuSystem;
}

InputCore* ChimeraGameEngine::GetInputCore()
{
	return m_InputCore;
}

AudioCore* ChimeraGameEngine::GetAudioCore()
{
	return m_AudioCore;
}

void ChimeraGameEngine::InitializeMenuSystem()
{
	m_EngineMenuSystem=new Menu();
}

GraphicsCore* ChimeraGameEngine::GetGraphicsCore()
{
	return m_GraphicsCore;
}

MultiPlayerCore* ChimeraGameEngine::GetMultiPlayerCore()
{
	return m_MultiPlayerCore;
}

void ChimeraGameEngine::Update(float dt)
{
	m_AudioCore->Update();
	m_InputCore->Update();
	//m_EngineMenuSystem->Update(dt);
	m_MessageSystem->DispatchMessages();
}
void ChimeraGameEngine::OnLost()
{
	m_GraphicsCore->OnLost();
	m_EngineMenuSystem->OnLostDevice();
}
void ChimeraGameEngine::OnReset()
{
	m_GraphicsCore->OnReset();
	m_EngineMenuSystem->OnResetDevice();
}
void ChimeraGameEngine::InitializePhysics()
{
	m_DragonPhysicsCore=new DragonPhysicsEngine();
}
void ChimeraGameEngine::PowerDownPhysics()
{
	if (m_DragonPhysicsCore!=NULL)
	{
		delete m_DragonPhysicsCore;
		m_DragonPhysicsCore=NULL;
	}
}
GamePlayCore* ChimeraGameEngine::GetGamePlayCore()
{
	return m_GamePlayCore;
}