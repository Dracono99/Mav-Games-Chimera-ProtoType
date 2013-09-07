#include "MultiPlayerMenuElements.h"
#include "BaseGameEntity.h"
#include "PacketStructs.h"
#include "PhysicsSphere.h"
#include "GraphicsCore.h"
#include "MainGameManager.h"
#include "ChimeraGameEngine.h"
#include "BaseMpOnlineAIObject.h"
#include "MultiPlayerCore.h"
#include "MenuStateIds.h"
#include "MenuFSM.h"
#include <string>

MultiPlayerGame::MultiPlayerGame()
{
	InitializeCriticalSection(&m_GameDataCS);
	m_NumPlayersInGame=0;
	mGameInSession=false;
}
MultiPlayerGame::MultiPlayerGame(char id)
{
	mGameID=id;
	InitializeCriticalSection(&m_GameDataCS);
	m_NumPlayersInGame=0;
	mGameInSession=false;
}
MultiPlayerGame::~MultiPlayerGame()
{
	DeleteCriticalSection(&m_GameDataCS);
	/*for (auto ents : m_Entities)
	{
	delete ents.second;
	}*/
}
PlayerData MultiPlayerGame::GetPlayerDataByID(char id)
{
	PlayerData data;
	data.clientID=(char)-1;
	EnterCriticalSection(&m_GameDataCS);
	auto res = m_GamePlayerData.find(id);
	if(res==m_GamePlayerData.end())
	{
		LeaveCriticalSection(&m_GameDataCS);
		return data;
	}
	data=res->second;
	LeaveCriticalSection(&m_GameDataCS);
	return data;
}
void MultiPlayerGame::SetPlayerDataByID(PlayerData data)
{
	EnterCriticalSection(&m_GameDataCS);
	auto res = m_GamePlayerData.find(data.clientID);
	if (res==m_GamePlayerData.end())
	{
		LeaveCriticalSection(&m_GameDataCS);
		AddPlayerToGame(data);
		return;
	}
	else
	{
		res->second=data;
		LeaveCriticalSection(&m_GameDataCS);
		return;
	}
}
void MultiPlayerGame::AddPlayerToGame(PlayerData newPlayer)
{
	if(mGameInSession)
	{
		AddPlayerToGameInSession(newPlayer);
	}
	else
	{
		EnterCriticalSection(&m_GameDataCS);
		auto res = m_GamePlayerData.find(newPlayer.clientID);
		if (res == m_GamePlayerData.end())
		{
			m_GamePlayerData.insert(std::make_pair(newPlayer.clientID,newPlayer));
		}
		//m_NumPlayersInGame++;
		LeaveCriticalSection(&m_GameDataCS);
	}
}
void MultiPlayerGame::RemovePlayerFromVector(BaseGameEntity* player)
{
	std::vector<BaseGameEntity*> keepers;
	for(auto entity : m_GameEntities)
	{
		if (entity!=player)
		{
			keepers.push_back(entity);
		}
	}
	m_GameEntities.clear();
	m_GameEntities=keepers;
}
void MultiPlayerGame::RemovePlayerFromGame(char playerID)
{
	EnterCriticalSection(&m_GameDataCS);
	auto res = m_GamePlayerData.find(playerID);
	if(res == m_GamePlayerData.end())
	{
		LeaveCriticalSection(&m_GameDataCS);
		return;
	}
	else
	{
		m_GamePlayerData.erase(res);
		if (mGameInSession)
		{
			auto res2 = m_Entities.find(playerID);
			if (res2!=m_Entities.end())
			{
				RemovePlayerFromVector(res2->second);
				m_Entities.erase(res2);
			}
		}
		//m_NumPlayersInGame--;
		LeaveCriticalSection(&m_GameDataCS);
		return;
	}
}
std::string MultiPlayerGame::GetGameData()
{
	std::string gameid = std::to_string((int)mGameID);
	std::string numPlayers = std::to_string(m_NumPlayersInGame);
	std::string retval = "Game ID "+gameid+" # "+numPlayers+" of 8";
	return retval;
}
void MultiPlayerGame::SetNumPlayers(int numPlayers)
{
	m_NumPlayersInGame=numPlayers;
}
void MultiPlayerGame::SetGameID(char id)
{
	mGameID=id;
}
std::map<char ,BaseGameEntity*> MultiPlayerGame::GetMapOfEntities()
{
	return m_Entities;
}
void MultiPlayerGame::AddPlayerToGameInSession(PlayerData newPlayerD)
{
	EnterCriticalSection(&m_GameDataCS);
	auto res = m_GamePlayerData.find(newPlayerD.clientID);
	if (res == m_GamePlayerData.end())
	{
		m_GamePlayerData.insert(std::make_pair(newPlayerD.clientID,newPlayerD));
		BaseGameEntity* newPlayer = new BaseGameEntity();
		BaseGameplayObject* bgpo= new BaseGameplayObject();
		bgpo->m_ObjectID=(int)newPlayerD.clientID;
		bgpo->m_HitEffectApplicaple=true;
		newPlayer->SetBaseGamePlayObject(bgpo);
		bgpo->m_mpOlineEntityAI = new BaseMpOnlineAIObject();
		bgpo->m_mpOlineEntityAI->m_Owner=newPlayer;
		bgpo->m_Owner=newPlayer;
		//gd3dApp->GetEngine()->GetMultiPlayerCore()->RegisterOnlineEntity(newPlayer);
		PhysicsSphere* c = new PhysicsSphere();
		newPlayer->SetBasePhysicsEntity(c);
		c->SetMassAndInverseTensor(1.0f);
		c->m_isCollisionDetectionOn=false;
		float theta = newPlayerD.rotation;
		DragonXQuaternion Yaw(0.0f,1.0f*sin(0.5f*theta),0.0f,cos(0.5f*theta));
		c->m_Position=D3DXVECTOR3(newPlayerD.positionX,0.0f,newPlayerD.positionY);
		c->m_QuatRot=Yaw;
		GraphicsSchematic playerShipGfx;
		playerShipGfx.mName="PlayerShipGfx";
		playerShipGfx.mMeshFileName="TankMP.X";
		playerShipGfx.mMeshDiffuseTexName="MaPZone[TankTexSet_diffuse].png";
		playerShipGfx.mMeshNormalTexName="MaPZone[TankTexSet_bump].png";
		playerShipGfx.mMeshSpecTexName="TankTexSetSpec.bmp";
		gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(newPlayer,playerShipGfx);
		m_Entities.insert(std::make_pair(newPlayerD.clientID,newPlayer));
		m_GameEntities.push_back(newPlayer);
	}
	else
	{
		BaseGameEntity* newPlayer = new BaseGameEntity();
		BaseGameplayObject* bgpo= new BaseGameplayObject();
		bgpo->m_ObjectID=(int)newPlayerD.clientID;
		newPlayer->SetBaseGamePlayObject(bgpo);
		bgpo->m_mpOlineEntityAI = new BaseMpOnlineAIObject();
		bgpo->m_HitEffectApplicaple=true;
		bgpo->m_mpOlineEntityAI->m_Owner=newPlayer;
		bgpo->m_Owner=newPlayer;
		//gd3dApp->GetEngine()->GetMultiPlayerCore()->RegisterOnlineEntity(newPlayer);
		PhysicsSphere* c = new PhysicsSphere();
		newPlayer->SetBasePhysicsEntity(c);
		c->SetMassAndInverseTensor(1.0f);
		c->m_isCollisionDetectionOn=false;
		float theta = newPlayerD.rotation;
		DragonXQuaternion Yaw(0.0f,1.0f*sin(0.5f*theta),0.0f,cos(0.5f*theta));
		c->m_Position=D3DXVECTOR3(newPlayerD.positionX,0.0f,newPlayerD.positionY);
		c->m_QuatRot=Yaw;
		GraphicsSchematic playerShipGfx;
		playerShipGfx.mName="PlayerShipGfx";
		playerShipGfx.mMeshFileName="TankMP.X";
		playerShipGfx.mMeshDiffuseTexName="MaPZone[TankTexSet_diffuse].png";
		playerShipGfx.mMeshNormalTexName="MaPZone[TankTexSet_bump].png";
		playerShipGfx.mMeshSpecTexName="TankTexSetSpec.bmp";
		gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(newPlayer,playerShipGfx);
		m_Entities.insert(std::make_pair(newPlayerD.clientID,newPlayer));
		m_GameEntities.push_back(newPlayer);
	}
	//m_NumPlayersInGame++;
	LeaveCriticalSection(&m_GameDataCS);
}
void MultiPlayerGame::ClearGameEntities()
{
	m_GamePlayerData.clear();
	m_Entities.clear();
	for (auto ents : m_GameEntities)
	{
		delete ents;
	}
	m_GameEntities.clear();
}
void MultiPlayerGame::InitializePlayerData(PlayerData data)
{
	EnterCriticalSection(&m_GameDataCS);
	auto res = m_GamePlayerData.find(data.clientID);
	if (res == m_GamePlayerData.end())
	{
		m_GamePlayerData.insert(std::make_pair(data.clientID,data));
		auto res2 = m_Entities.find(data.clientID);
		if (res2 == m_Entities.end())
		{
			BaseGameEntity* newPlayer = new BaseGameEntity();
			BaseGameplayObject* bgpo= new BaseGameplayObject();
			bgpo->m_ObjectID=(int)data.clientID;
			bgpo->m_HitEffectApplicaple=true;
			newPlayer->SetBaseGamePlayObject(bgpo);
			bgpo->m_mpOlineEntityAI = new BaseMpOnlineAIObject();
			bgpo->m_mpOlineEntityAI->m_Owner=newPlayer;
			bgpo->m_Owner=newPlayer;
			//gd3dApp->GetEngine()->GetMultiPlayerCore()->RegisterOnlineEntity(newPlayer);
			PhysicsSphere* c = new PhysicsSphere();
			newPlayer->SetBasePhysicsEntity(c);
			c->SetMassAndInverseTensor(1.0f);
			c->m_isCollisionDetectionOn=false;
			float theta = data.rotation;
			DragonXQuaternion Yaw(0.0f,1.0f*sin(0.5f*theta),0.0f,cos(0.5f*theta));
			c->m_Position=D3DXVECTOR3(data.positionX,0.0f,data.positionY);
			c->m_QuatRot=Yaw;
			GraphicsSchematic playerShipGfx;
			playerShipGfx.mName="PlayerShipGfx";
			playerShipGfx.mMeshFileName="TankMP.X";
			playerShipGfx.mMeshDiffuseTexName="MaPZone[TankTexSet_diffuse].png";
			playerShipGfx.mMeshNormalTexName="MaPZone[TankTexSet_bump].png";
			playerShipGfx.mMeshSpecTexName="TankTexSetSpec.bmp";
			gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(newPlayer,playerShipGfx);
			m_Entities.insert(std::make_pair(data.clientID,newPlayer));
			m_GameEntities.push_back(newPlayer);
		}
	}
	else
	{
		auto res2 = m_Entities.find(data.clientID);
		if (res2 == m_Entities.end())
		{
			BaseGameEntity* newPlayer = new BaseGameEntity();
			BaseGameplayObject* bgpo= new BaseGameplayObject();
			bgpo->m_ObjectID=(int)data.clientID;
			bgpo->m_HitEffectApplicaple=true;
			newPlayer->SetBaseGamePlayObject(bgpo);
			bgpo->m_mpOlineEntityAI = new BaseMpOnlineAIObject();
			bgpo->m_mpOlineEntityAI->m_Owner=newPlayer;
			bgpo->m_Owner=newPlayer;
			//gd3dApp->GetEngine()->GetMultiPlayerCore()->RegisterOnlineEntity(newPlayer);
			PhysicsSphere* c = new PhysicsSphere();
			newPlayer->SetBasePhysicsEntity(c);
			c->SetMassAndInverseTensor(1.0f);
			c->m_isCollisionDetectionOn=false;
			float theta = data.rotation;
			DragonXQuaternion Yaw(0.0f,1.0f*sin(0.5f*theta),0.0f,cos(0.5f*theta));
			c->m_Position=D3DXVECTOR3(data.positionX,0.0f,data.positionY);
			c->m_QuatRot=Yaw;
			GraphicsSchematic playerShipGfx;
			playerShipGfx.mName="PlayerShipGfx";
			playerShipGfx.mMeshFileName="TankMP.X";
			playerShipGfx.mMeshDiffuseTexName="MaPZone[TankTexSet_diffuse].png";
			playerShipGfx.mMeshNormalTexName="MaPZone[TankTexSet_bump].png";
			playerShipGfx.mMeshSpecTexName="TankTexSetSpec.bmp";
			gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(newPlayer,playerShipGfx);
			m_Entities.insert(std::make_pair(data.clientID,newPlayer));
			m_GameEntities.push_back(newPlayer);
		}
	}
	//m_NumPlayersInGame++;
	LeaveCriticalSection(&m_GameDataCS);
}
void MultiPlayerGame::InitializeGame(ServerPacket_SyncGame data)
{
	for(int i = 0 ; i <8 ; i++)
	{
		if(data.data[i].clientID>0)
		{
			InitializePlayerData(data.data[i]);
		}
	}
	BaseGameEntity* m_Galaxy=new BaseGameEntity();
	BaseGameplayObject* worldObj = new BaseGameplayObject();
	worldObj->m_HitEffectApplicaple=false;
	m_Galaxy->SetBaseGamePlayObject(worldObj);
	PhysicsSphere* c = new PhysicsSphere();
	c->m_isCollisionDetectionOn=false;
	c->m_Position.y=517.2f;
	GraphicsSchematic galaxy;
	galaxy.mName="TerrainSchem";
	galaxy.mMeshFileName="TankMP_Terrain.X";
	//galaxy.mMeshFileName="LowishPolyHeavyArms.X";
	galaxy.mMeshDiffuseTexName="MaPZone[BigDesertTerrain_diffuse].png";
	galaxy.mMeshNormalTexName="MaPZone[BigDesertTerrain_norm].png";
	galaxy.mMeshSpecTexName="BIgBlackTex.png";
	gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(m_Galaxy,galaxy);
	m_Galaxy->SetBasePhysicsEntity(c);
	m_GameEntities.push_back(m_Galaxy);

	BaseGameEntity* m_Desert=new BaseGameEntity();
	BaseGameplayObject* worldObjDes = new BaseGameplayObject();
	worldObjDes->m_HitEffectApplicaple=false;
	m_Desert->SetBaseGamePlayObject(worldObjDes);
	PhysicsSphere* cD = new PhysicsSphere();
	cD->m_isCollisionDetectionOn=false;	
	GraphicsSchematic DesertSky;
	DesertSky.mName="DesertSkySchem";
	DesertSky.mMeshFileName="BigGalaxy.X";
	//galaxy.mMeshFileName="LowishPolyHeavyArms.X";
	DesertSky.mMeshDiffuseTexName="MaPZone[DesertSkyTexSet_diffuse].bmp";
	DesertSky.mMeshNormalTexName="MaPZone[DesertSkyTexSet_bump].bmp";
	DesertSky.mMeshSpecTexName="BIgBlackTex - Copy.png";
	gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(m_Desert,DesertSky);
	m_Desert->SetBasePhysicsEntity(cD);
	m_GameEntities.push_back(m_Desert);
	gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(GamePlayMenu);
}
void MultiPlayerGame::HandleSyncPacket(ServerPacket_SyncGame packet)
{
	if(mGameInSession)
	{
		for(int i = 0 ; i <8 ; i++)
		{
			if(packet.data[i].clientID>0)
			{
				SetPlayerDataByID(packet.data[i]);
			}
		}
		mGameInSession=true;
	}
	else
	{
		mGameInSession=true;
		InitializeGame(packet);
		gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(MultiplayerGameplayMenuStateID);
	}
}
void MultiPlayerGame::HandleFireEventPacket(ServerPacket_FireEvent pkt)
{
	BaseGameEntity* b = new BaseGameEntity();
	BaseGameplayObject* projctle = new BaseGameplayObject();
	projctle->m_HitEffectApplicaple=false;
	b->SetBaseGamePlayObject(projctle);
	PhysicsSphere* p = new PhysicsSphere(5.0f);
	//DragonMeshCollisionObject* p = new DragonMeshCollisionObject();
	//gd3dApp->GetEngine()->GetPhysicsCore()->SetUpMesh(p,gd3dApp->GetEngine()->GetGraphicsCore()->CreateCollisionMesh("BulletProjectile.X"));
	b->SetBasePhysicsEntity(p);
	gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObjectWithRegisteredSchematic(b,"BulletSchematic");
	auto res=m_Entities.find(pkt.clientID);
	if (res!=m_Entities.end())
	{
		p->m_QuatRot=res->second->GetPhysicsPointer()->m_QuatRot;
		p->m_Position=res->second->GetPhysicsPointer()->getPointInWorldSpace(D3DXVECTOR3(0.0f,1.7f,7.0f));
		p->m_LinearVelocity=res->second->GetPhysicsPointer()->m_QuatRot.Forward()*80.0f;
	}
	m_GameEntities.push_back(b);
	if(!pkt.targetID)
	{
		// some form of hit effect happens here
	}
}
PlayerData MultiPlayerGame::GetPlayerDataByNumber(int playerNumber)
{
	PlayerData nulRet;
	nulRet.clientID=-1;
	nulRet.health=0;
	int i = 1;
	for(auto plyr : m_GamePlayerData)
	{
		if(i==playerNumber)
		{
			return plyr.second;
		}
		i++;
	}
	return nulRet;
}
void MultiPlayerGame::UpdateGame(float dt)
{
	for(auto entity : m_Entities)
	{
		float x=0;
		float z=0;
		float theta = 0;
		char moveFlags=(char)0;
		moveFlags=m_GamePlayerData.find((char)entity.second->GetGamePlayObject()->m_ObjectID)->second.moveFlags;
		theta = m_GamePlayerData.find((char)entity.second->GetGamePlayObject()->m_ObjectID)->second.rotation;
		x = m_GamePlayerData.find((char)entity.second->GetGamePlayObject()->m_ObjectID)->second.positionX;
		z = m_GamePlayerData.find((char)entity.second->GetGamePlayObject()->m_ObjectID)->second.positionY;
		entity.second->GetPhysicsPointer()->m_Position.x=x;
		entity.second->GetPhysicsPointer()->m_Position.z=z;
		entity.second->GetPhysicsPointer()->m_QuatRot=DragonXQuaternion(0.0f,1.0f*sin(0.5f*theta),0.0f,cos(0.5f*theta));
		//entity.second->GetGamePlayObject()->m_mpOlineEntityAI->SetMpLocation(x,z);
		//entity.second->GetGamePlayObject()->m_mpOlineEntityAI->UpdateOnlineEntity(dt);
		/*DragonXQuaternion Yaw(0.0f,1.0f*sin(0.5f*theta),0.0f,cos(0.5f*theta));
		float magDif = entity.second->GetPhysicsPointer()->m_QuatRot.Forward()*Yaw.Forward();
		DragonXVector3 dir = entity.second->GetPhysicsPointer()->m_QuatRot.Forward()%Yaw.Forward();
		dir.Normalize();
		dir=dir*magDif*-1.0f;
		entity.second->GetPhysicsPointer()->addTorque(dir);*/
	/*	if((moveFlags&moveFlagsRightMask)==moveFlagsRightMask)
		{
			entity.second->GetPhysicsPointer()->m_RotationalVelocity.y=0.5235987756f;
		}
		else if((moveFlags&moveFlagsLeftMask)!=moveFlagsLeftMask)
		{
			entity.second->GetPhysicsPointer()->m_RotationalVelocity.y=0.0f;
			entity.second->GetPhysicsPointer()->m_AngularForceAccum=DragonXVector3(0.0f,0.0f,0.0f);
		}
		if((moveFlags&moveFlagsLeftMask)==moveFlagsLeftMask)
		{
			entity.second->GetPhysicsPointer()->m_RotationalVelocity.y=-0.5235987756f;
		}
		else if((moveFlags&moveFlagsRightMask)!=moveFlagsRightMask)
		{
			entity.second->GetPhysicsPointer()->m_RotationalVelocity.y=0.0f;
			entity.second->GetPhysicsPointer()->m_AngularForceAccum=DragonXVector3(0.0f,0.0f,0.0f);
		}
		if((moveFlags&moveFlagsBackMask)==moveFlagsBackMask)
		{
			entity.second->GetPhysicsPointer()->m_LinearVelocity=entity.second->GetPhysicsPointer()->m_QuatRot.Forward()*-1.0f;
		}
		else if((moveFlags&moveFlagsForwardMask)!=moveFlagsForwardMask)
		{
			entity.second->GetPhysicsPointer()->m_LinearVelocity=DragonXVector3(0.0f,0.0f,0.0f);
			entity.second->GetPhysicsPointer()->m_LinearForceAccum=DragonXVector3(0.0f,0.0f,0.0f);
		}
		if((moveFlags&moveFlagsForwardMask)==moveFlagsForwardMask)
		{
			entity.second->GetPhysicsPointer()->m_LinearVelocity=entity.second->GetPhysicsPointer()->m_QuatRot.Forward()*1.0f;
		}
		else if((moveFlags&moveFlagsBackMask)!=moveFlagsBackMask)
		{
			entity.second->GetPhysicsPointer()->m_LinearVelocity=DragonXVector3(0.0f,0.0f,0.0f);
			entity.second->GetPhysicsPointer()->m_LinearForceAccum=DragonXVector3(0.0f,0.0f,0.0f);
		}*/
	}
}
void MultiPlayerGame::GameOverEvent()
{
	mGameInSession=false;
	ClearGameEntities();
	gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(MultiplayerGameOverStateID);
}

MultiPlayerLobby::MultiPlayerLobby()
{
	InitializeCriticalSection(&m_LobbyDataCS);
	m_NumPlayersInLobby=0;
	InitializeCriticalSection(&m_LobbyPlayerDataCS);
}
MultiPlayerLobby::~MultiPlayerLobby()
{
	DeleteCriticalSection(&m_LobbyDataCS);
	DeleteCriticalSection(&m_LobbyPlayerDataCS);
	for(auto games : m_GameData)
	{
		games.second.ClearGameEntities();
	}
}
std::string MultiPlayerLobby::GetLobbyData()
{
	std::string numPlayers = std::to_string(m_NumPlayersInLobby);
	std::string cap = "Players In Open Lobby ";
	std::string retval = cap+numPlayers+"\0";
	numPlayers.clear();
	cap.clear();
	return retval;
}
void MultiPlayerLobby::RegisterPlayer(PlayerData player)
{
	EnterCriticalSection(&m_LobbyPlayerDataCS);
	m_LobbyPLayers.insert(std::make_pair(player.clientID,player));
	//m_NumPlayersInLobby++;
	LeaveCriticalSection(&m_LobbyPlayerDataCS);
}
void MultiPlayerLobby::RemovePlayer(char playerID)
{
	EnterCriticalSection(&m_LobbyPlayerDataCS);
	auto res = m_LobbyPLayers.find(playerID);
	if(res == m_LobbyPLayers.end())
	{
		LeaveCriticalSection(&m_LobbyPlayerDataCS);
		return;
	}
	else
	{
		m_LobbyPLayers.erase(res);
		//m_NumPlayersInLobby--;
		LeaveCriticalSection(&m_LobbyPlayerDataCS);
		return;
	}
}
void MultiPlayerLobby::AddGame(char id)
{
	MultiPlayerGame game(id);
	EnterCriticalSection(&m_LobbyDataCS);
	m_GameData.insert(std::make_pair(id,game));
	LeaveCriticalSection(&m_LobbyDataCS);
}
void MultiPlayerLobby::RemoveGame(char id)
{
	EnterCriticalSection(&m_LobbyDataCS);
	auto res = m_GameData.find(id);
	if(res == m_GameData.end())
	{
		LeaveCriticalSection(&m_LobbyDataCS);
		return;
	}
	else
	{
		m_GameData.erase(res);
		LeaveCriticalSection(&m_LobbyDataCS);
		return;
	}
}
MultiPlayerGame* MultiPlayerLobby::GetMultiPlayerGameDataByID(char id)
{
	//EnterCriticalSection(&m_LobbyDataCS);
	auto res = m_GameData.find(id);
	if (res==m_GameData.end())
	{
		return NULL;
	}
	else
	{
		return &res->second;
	}
	//LeaveCriticalSection(&m_LobbyDataCS);
}

std::map<char, MultiPlayerGame> MultiPlayerLobby::GetCopyOfGameData()
{
	return m_GameData;
}

void MultiPlayerLobby::SetNumPlayersInLobby(int numPlayers)
{
	m_NumPlayersInLobby=numPlayers;
}