#pragma once
#include <map>
#include <vector>
#include <Windows.h>

struct PlayerData;
class BaseGameEntity;
struct ServerPacket_SyncGame;
struct ServerPacket_FireEvent;
struct ServerPacket_GameOver;

class MultiPlayerGame
{
private:
	CRITICAL_SECTION m_GameDataCS;
	int m_NumPlayersInGame;
	std::map<char ,PlayerData > m_GamePlayerData;
	std::map<char ,BaseGameEntity*> m_Entities;
	char mGameID;
	void RemovePlayerFromVector(BaseGameEntity* player);
public:
	MultiPlayerGame();
	MultiPlayerGame(char id);
	~MultiPlayerGame();
	PlayerData GetPlayerDataByID(char id);
	void SetPlayerDataByID(PlayerData data);
	void AddPlayerToGame(PlayerData newPlayer);
	void RemovePlayerFromGame(char playerID);
	std::string GetGameData();
	void SetNumPlayers(int numPlayers);
	void SetGameID(char id);
	std::map<char ,BaseGameEntity*> GetMapOfEntities();
	bool mGameInSession;
	void InitializeGame(ServerPacket_SyncGame data);
	void AddPlayerToGameInSession(PlayerData newPlayerD);
	void InitializePlayerData(PlayerData data);
	void HandleSyncPacket(ServerPacket_SyncGame packet);
	void HandleFireEventPacket(ServerPacket_FireEvent pkt);
	void UpdateGame(float dt);
	void GameOverEvent();
	std::vector<BaseGameEntity*> m_GameEntities;
	void ClearGameEntities();
	PlayerData GetPlayerDataByNumber(int playerNumber);
};

class MultiPlayerLobby
{
private:
	CRITICAL_SECTION m_LobbyDataCS;
	CRITICAL_SECTION m_LobbyPlayerDataCS;
	std::map<char, MultiPlayerGame> m_GameData;
	int m_NumPlayersInLobby;
	std::map<char, PlayerData> m_LobbyPLayers;
public:
	MultiPlayerLobby();
	~MultiPlayerLobby();
	std::string GetLobbyData();
	void RegisterPlayer(PlayerData player);
	void RemovePlayer(char playerID);
	void AddGame(char id);
	void RemoveGame(char id);
	MultiPlayerGame* GetMultiPlayerGameDataByID(char id);
	std::map<char, MultiPlayerGame> GetCopyOfGameData();
	void SetNumPlayersInLobby(int numPlayers);
};