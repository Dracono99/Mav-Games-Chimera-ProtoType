#pragma once

enum ClientPacketIDs
{
	CPI_Fire,
	CPI_Roate,
	CPI_Move,
	CPI_ThrusterFired,
	CPI_RegisterHit,
	CPI_RequestServerState,
	CPI_JoinGameLobby,
	CPI_LeaveGameLobby,
	CPI_LeaveServer,
};

enum ServerPacketIDs
{
	SPI_FireEventEcho,
	SPI_PlayerRotateEcho,
	SPI_PlayerMoveEcho,
	SPI_PlayerThrusterFiredEcho,
	SPI_RegisterHitEcho,
};

struct PacketTypeOne{
	short ID;
	float xVal, zVal;
};

union PacketTypeOneUt
{
	PacketTypeOne p;
	char c[256];
};

struct ClientPacketFireEvent // p1
{
	int mPacketID;//CPI_Fire
	bool mLeft;
	float xDir;
	float yDir;
	float zDir;
	float xPos;
	float yPos;
	float zPos;
};

struct ServerPacket_FireEventEcho
{
	int mPacketID;//SPI_FireEventEcho
	int mShooterID;
	bool mLeft;
	float xDir;
	float yDir;
	float zDir;
	float xPos;
	float yPos;
	float zPos;
};

struct ClientPacket_Rotate // p2
{
	int mPacketID;//CPI_Roate
	float xDir;// these values will be from the torque as applied by the player
	float yDir;
	float zDir;
};

struct ServerPacket_PlayerRotateEcho
{
	int mPacketID;//SPI_PlayerRotateEcho
	int mPlayerID;
	float xDir;// these values will be from the torque as applied by the player
	float yDir;
	float zDir;
};

struct ClientPacket_Move // p3
{
	int mPacketID;// CPI_Move
	float xForce;// these will be the linear forces as applied by the player
	float yForce;
	float zForce;
};

struct ServerPacket_PlayerMoveEcho
{
	int mPacketID;//SPI_PlayerMoveEcho
	int mPlayerID;
	float xForce;
	float yForce;
	float zForce;
};

struct ClientPacket_ThrusterFired // p4
{
	int mPacketID;//CPI_ThrusterFired
	float xDir;// whenever a thruster is fired this message is sent
	float yDir;
	float zDir;
	float Mag;
};

struct ServerPacket_ThrusterFiredEcho
{
	int mPacketID;//SPI_PlayerThrusterFiredEcho
	int mPlayerID;
	float xDir;
	float yDir;
	float zDir;
	float Mag;
};

struct ClientPacket_RegisterHit // p5
{
	int mPacketID;//CPI_RegisterHit
	int mShooterID;
	int mPlayerHitID;
};

struct ServerPacket_RegisterHitEcho
{
	int mPacketID;//SPI_RegisterHitEcho
	int mShooterID;
	int mPlayerHitID;
};

struct ClientPacket_RequestServerState // p6
{
	int mPacketID;//CPI_RequestServerState
};

struct ServerPacket_ServerState
{
	int mPacketID;
	int mPlayersOnServer;
	// personal project put on hold work on class project
};

struct ClientPacket_JoinGameLobby // p7
{
	int mPacketID;//CPI_JoinGameLobby
	int mLobbyID;
};

struct ClientPacket_LeaveGameLobby //p8
{
	int mPacketID;//CPI_LeaveGameLobby
	int mLobbyID;
};

struct ClientPacket_LeaveServer // p9
{
	int mPacketID;//CPI_LeaveServer
};

union ClientPacketUnion
{
	ClientPacketFireEvent p1;
	ClientPacket_Rotate p2;
	ClientPacket_Move p3;
	ClientPacket_ThrusterFired p4;
	ClientPacket_RegisterHit p5;
	ClientPacket_RequestServerState p6;
	ClientPacket_JoinGameLobby p7;
	ClientPacket_LeaveGameLobby p8;
	ClientPacket_LeaveServer p9;
	char c[256];
};
