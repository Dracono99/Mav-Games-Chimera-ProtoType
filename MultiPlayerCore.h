#pragma once
#include <conio.h>
#include <thread>
#include <vector>
#include <Windows.h>
#include <process.h>
#pragma comment(lib,"ws2_32.lib")

class BaseGameEntity;
class MultiPlayerLobby;
struct PlayerData;

class MultiPlayerCore
{
private:
	WSADATA WsaDat;
	SOCKET Socket;
	hostent *host;
	SOCKADDR_IN SockAddr;
	char buffer[256];
	bool m_Online;
	std::thread m_ReceivingThread;
	int GetNumFromCharViaBitEncoding(char num);
public:
	MultiPlayerCore();
	~MultiPlayerCore();
	std::vector<BaseGameEntity*> m_OnlineEntities;
	void SendPacket(char msg[256]);
	void RegisterOnlineEntity(BaseGameEntity* entity);
	void InitializeMultiplayerCore();
	void PowerDownMultiplayerCore();
	void HandleClientJoinGame(char clID,char gameID);
	void HandleGamePlayerUpdateData(PlayerData pData);
	BaseGameEntity* GetOnlineEntityById(int id);
	MultiPlayerLobby* m_MultiPlayerLobby;
	char m_ClientID;
	char m_JoinedGameID;
	char m_LastJoinedGameID;
	inline bool GetOnlineStatus()
	{
		return m_Online;
	}
	std::string mNameOfWinner;
	bool m_InLobby;
	bool ResetMoveFlags;
};