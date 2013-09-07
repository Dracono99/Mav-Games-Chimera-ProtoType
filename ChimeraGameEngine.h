#pragma once

class DragonPhysicsEngine;
class EngineMessageSystem;
class Menu;
class InputCore;
class AudioCore;
class GraphicsCore;
class GamePlayCore;
class MultiPlayerCore;

class ChimeraGameEngine
{
private:
	DragonPhysicsEngine* m_DragonPhysicsCore;
	EngineMessageSystem* m_MessageSystem;
	Menu* m_EngineMenuSystem;
	InputCore* m_InputCore;
	AudioCore* m_AudioCore;
	GraphicsCore* m_GraphicsCore;
	GamePlayCore* m_GamePlayCore;
	MultiPlayerCore* m_MultiPlayerCore;
public:
	ChimeraGameEngine();
	~ChimeraGameEngine();
	void InitializeMenuSystem();
	void InitializePhysics();
	void PowerDownPhysics();
	DragonPhysicsEngine* GetPhysicsCore();
	EngineMessageSystem* GetMessageSystem();
	Menu* GetMenuSystem();
	InputCore* GetInputCore();
	AudioCore* GetAudioCore();
	GraphicsCore* GetGraphicsCore();
	GamePlayCore* GetGamePlayCore();
	MultiPlayerCore* GetMultiPlayerCore();
	void Update(float dt);
	void OnLost();
	void OnReset();
};