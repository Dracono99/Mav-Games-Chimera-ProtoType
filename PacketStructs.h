#pragma once

// we'll use a 256 char buffer
// packet ids are 1-16 down the .h file
// using a char packet id
// first 4 bits are movement flags
// forward,back,left,right
// last 4 are packet id 0 through 15
// using binary notation of 0 through 15

////////////////////////////////////////////////
///
///	WARNING:
///	packetID first 4 bits will be 0-15 and determine the packet type based on the IDs below
///	On MOVE EVENT the last 4 bits will be encoded for the movement flags
///
///	_ _ _ _ | _ _ _ _
///	^ v < > | _ I D _
///
////////////////////////////////////////////////

#define CP_JS 0
#define CP_JG 1
#define CP_ME 2
#define CP_FE 3
#define CP_QG 4
#define CP_QS 5
#define SP_SL 6
#define SP_CJG 7
#define SP_SG 8
#define SP_ME 9
#define SP_FE 10
#define SP_GO 11
#define SP_CQG 12
#define SPSLGOneMask 0x000f
#define SPSLGTwoMask 0x00f0
#define SPSLGThreeMask 0x0f00
#define SPSLGFourMask 0xf000
#define packetidMask 0x0f
#define moveFlagsRightMask 0x01
#define moveFlagsLeftMask 0x02
#define moveFlagsBackMask 0x04
#define moveFlagsForwardMask 0x08

struct PlayerData
{
	char clientID;
	char health;
	char moveFlags;
	//extra byte here
	float positionX;
	float positionY;
	float rotation;
};

struct ClientPacket_JoinServer // ID #0
{
	char packetID;
	char clientID;
	char clientName[30];
};

struct ClientPacket_JoinGame // ID #1
{
	char packetID;
	char clientID;
	char gameID;
};

struct ClientPacket_MoveEvent // ID #2
{
	char packetID;
	char clientID;
	char gameID;
};

struct ClientPacket_FireEvent // ID #3
{
	char packetID;
	char clientID;
	char targetID; // if 0 no target
	char gameID;
};

struct ClientPacket_QuitGame // ID #4
{
	char packetID;
	char clientID;
	char gameID;
};

struct ClientPacket_QuitServer // ID #5
{
	char packetID;
	char clientID;
	char gameID;
};

struct ServerPacket_SyncLobby // ID #6
{
	char packetID;
	char clientID;		//Gives the client it's ID
	short playersInGame; //first 4 bits is first game, second is second game, etc. BIT_ENCODED
	char playersInLobby; //number of players connected in the lobby. MAX 255
};

struct ServerPacket_ClientJoinGame // ID #7
{
	char packetID;
	char clientID;
	char gameID;
};

struct ServerPacket_SyncGame // ID #8
{
	char packetID;
	PlayerData data[8];
};

struct ServerPacket_MoveEvent // ID #9
{
	char packetID;
	char clientID;
};

struct ServerPacket_FireEvent // ID #10
{
	char packetID;
	char clientID;
	char targetID; // if 0 no target
};

struct ServerPacket_GameOver // ID #11
{
	char packetID;
	char winnerID;
	char winnerName[30];
};

struct ServerPacket_ClientQuitGame // ID #12
{
	char packetID;
	char clientID;
	char gameID;
};

/*

#ifndef PACKETS_H
#define PACKETS_H

#define CLIENT_JOIN_SERVER			0x0001
#define CLIENT_JOIN_GAME			0x0002
#define CLIENT_MOVE_EVENT			0x0004
#define CLIENT_FIRE_EVENT			0x0008
#define CLIENT_QUIT_GAME			0x0010
#define CLIENT_QUIT_SERVER			0x0020
#define SERVER_SYNC_LOBBY			0x0040
#define SERVER_CLIENT_JOIN_GAME		0x0080
#define SERVER_SYNC_GAME			0x0100
#define SERVER_MOVE_EVENT			0x0200
#define SERVER_FIRE_EVENT			0x0400
#define SERVER_GAME_OVER			0x0800
#define SERVER_CLIENT_QUIT_GAME		0x1000

struct PlayerData
{
short playerID;
int health;
float positionX;
float positionY;
float rotation;
char moveFlags;
};

struct ClientPacket_JoinServer // ID #1
{
short packetID;
short clientID;
char clientName[28];
};

struct ClientPacket_JoinGame // ID #2
{
short packetID;
short clientID;
short gameID;
};

struct ClientPacket_MoveEvent // ID #3
{
short packetID;
short clientID;
char moveflags;
};

struct ClientPacket_FireEvent // ID #4
{
short packetID;
short clientID;
};

struct ClientPacket_QuitGame // ID #5
{
short packetID;
short clientID;
short gameID;
};

struct ClientPacket_QuitServer // ID #6
{
short packetID;
short clientID;
short gameID;
};

struct ServerPacket_SyncLobby // ID #7
{
short packetID;
short clientID;
char playersInGame[4]; //4 games total - 8 players in each game - bit encoded
char playersInLobby[4]; // number of players in the lobby - bit encoded
};

struct ServerPacket_ClientJoinGame // ID #8
{
short packetID;
short clientID;
short gameID;
};

struct ServerPacket_SyncGame // ID #9
{
short packetID;
PlayerData data[8];
};

struct ServerPacket_MoveEvent // ID #10
{
short packetID;
short clientID;
char moveflags;
};

struct ServerPacket_FireEvent // ID #11
{
short packetID;
//short clientID;
PlayerData data;
};

struct ServerPacket_GameOver // ID #12
{
short packetID;
short winnerID;
char winnerName[28];
};

struct ServerPacket_ClientQuitGame // ID #13
{
short packetID;
short clientID;
short gameID;
};

#endif
*/

//struct PlayerData
//{
//	short playerID;
//	int health;
//	float positionX;
//	float positionZ;
//	float rotation;
//	char moveFlags;
//};
//
//#define CP_JSBM (SHORT)-2
//struct ClientPacket_JoinServer // ID #1
//{
//	short packetID;
//	short clientID;
//	char clientName[28];
//};
//
//#define CP_JGBM (SHORT)-3
//struct ClientPacket_JoinGame // ID #2
//{
//	short packetID;
//	short clientID;
//	short gameID;
//};
//
//#define CP_MEBM (SHORT)-4
//struct ClientPacket_MoveEvent // ID #3
//{
//	short packetID;
//	short clientID;
//	char moveflags;
//};
//
//#define CP_FEBM (SHORT)-5
//struct ClientPacket_FireEvent // ID #4
//{
//	short packetID;
//	short clientID;
//};
//
//#define CP_QGBM (SHORT)-6
//struct ClientPacket_QuitGame // ID #5
//{
//	short packetID;
//	short clientID;
//	short gameID;
//};
//
//#define CP_QSBM (SHORT)-7
//struct ClientPacket_QuitServer // ID #6
//{
//	short packetID;
//	short clientID;
//	short gameID;
//};
//
//#define SP_SLBM (SHORT)-8
//struct ServerPacket_SyncLobby // ID #7
//{
//	short packetID;
//	short clientID;
//	char playersInGame[4]; //4 games total - 8 players in each game - bit encoded
//	char playersInLobby[4]; // number of players in the lobby - bit encoded
//};
//
//#define SP_CJGBM (SHORT)-9
//struct ServerPacket_ClientJoinGame // ID #8
//{
//	short packetID;
//	short clientID;
//	short gameID;
//};
//
//#define SP_SGBM (SHORT)-10
//struct ServerPacket_SyncGame // ID #9
//{
//	short packetID;
//	PlayerData data[8];
//};
//
//#define SP_MEBM (SHORT)-11
//struct ServerPacket_MoveEvent // ID #10
//{
//	short packetID;
//	short clientID;
//	char moveflags;
//};
//
//#define SP_FEBM (SHORT)-12
//struct ServerPacket_FireEvent // ID #11
//{
//	short packetID;
//	PlayerData data;
//};
//
//#define SP_GOBM (SHORT)-13
//struct ServerPacket_GameOver // ID #12
//{
//	short packetID;
//	short winnerID;
//	char winnerName[28];
//};
//
//#define SP_CQGBM (SHORT)-14
//struct ServerPacket_ClientQuitGame // ID #13
//{
//	short packetID;
//	short clientID;
//	short gameID;
//};

struct BaseStruct
{
	char packetID;
};

union IDpacket
{
	BaseStruct p;
	char c[256];
};

#define CHAR8BM 0
#define CHAR7BM -128
#define CHAR6BM -64
#define CHAR5BM -32
#define CHAR4BM -16
#define CHAR3BM -8
#define CHAR2BM -4
#define CHAR1BM -2
#define CHAR0BM -1
