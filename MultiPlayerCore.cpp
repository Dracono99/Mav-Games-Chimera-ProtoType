#include <WinSock2.h>
#include "MultiPlayerCore.h"
#include "BaseGameEntity.h"
#include "MpPacketDefs.h"
#include "MainGameManager.h"
#include "ChimeraGameEngine.h"
#include "MessageSystem.h"
#include "BaseGameplayObject.h"
#include "BaseMpOnlineAIObject.h"
#include <iostream>
#include <fstream>
#include "MainGameStateIDs.h"
#include "MultiPlayerMenuElements.h"
#include "PacketStructs.h"
#include "DragonMeshCollisionObject.h"
#include "GraphicsCore.h"
#include "DragonPhysicsEngine.h"

MultiPlayerCore::MultiPlayerCore()
{
	m_Online=false;
	ResetMoveFlags=true;
	m_MultiPlayerLobby=NULL;
	m_JoinedGameID=(char)-1;
	mNameOfWinner="";
}
MultiPlayerCore::~MultiPlayerCore()
{
	if(m_Online)
	{
		PowerDownMultiplayerCore();
	}
	m_ReceivingThread.~thread();
}
void MultiPlayerCore::SendPacket(char msg[256])
{
	try
	{
		send(Socket,msg,256,NULL);
	}
	catch(...)
	{
		m_Online=false;
	}
}
void MultiPlayerCore::RegisterOnlineEntity(BaseGameEntity* entity)
{
	if(entity->GetGamePlayObject()!=NULL)
	{
		for(auto entities : m_OnlineEntities)
		{
			if (entities->GetGamePlayObject()!=NULL)
			{
				if (entities->GetGamePlayObject()->m_mpOlineEntityAI==entity->GetGamePlayObject()->m_mpOlineEntityAI)
				{
					return;
				}
			}
		}
		m_OnlineEntities.push_back(entity);
	}
}
void MultiPlayerCore::InitializeMultiplayerCore()
{
	// init winsock
	WsaDat;	// winsock data sorta our main directx handle but to winsock
	if(WSAStartup(MAKEWORD(2,2),&WsaDat)!=0)//p1 = version of winsock
	{
		//std::cout<<"Winsock error - winsock initialization failed\r\r";
		WSACleanup();
		//system("PAUSE");
		gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(0,[&](){
			gd3dApp->SwitchGameState(MainGameMenuStateID);
		}));
		return;
	}
	// create socket

	Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);// addressfamily is af net for piv4 afnet6 for ipv6, sock stream uses tcp for internet family and sock dgram sets udp the param is type of socket, protocol this is tcp ip
	if(Socket==INVALID_SOCKET)
	{
		//std::cout<<"winsock error - socket creation failed\r\n";
		WSACleanup();
		//system("PAUSE");
		gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(0,[&](){
			gd3dApp->SwitchGameState(MainGameMenuStateID);
		}));
		return;
	}
	std::ifstream inFile;
	inFile.open("IpAddressPatrick.txt");
	std::string ip;
	inFile >> ip;
	//std::cout<<"enter sever ip\r\n";
	//std::getline(std::cin,ip);
	inFile.close();
	// host ip address set up
	host;
	if((host = gethostbyname(ip.c_str()))==NULL)
	{
		//std::cout<<"failed to resolve hostname.\r\n";
		WSACleanup();
		gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(0,[&](){
			gd3dApp->SwitchGameState(MainGameMenuStateID);
		}));
		//system("PAUSE");
		return;
	}

	// setup socket address stucture
	SockAddr; //
	SockAddr.sin_port=htons(6000);// port number
	SockAddr.sin_family=AF_INET;// atm near as i can tell sets to tcp/ip
	SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);// this is what im sending to

	// attemp to connect 2 server
	if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr))==SOCKET_ERROR)
	{
		//std::cout<<"failed to connect 2 server\r\n";
		WSACleanup();
		//system("PAUSE");
		gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(0,[&](){
			gd3dApp->SwitchGameState(MainGameMenuStateID);
		}));
		return;
	}	
	m_MultiPlayerLobby = new MultiPlayerLobby();
	m_MultiPlayerLobby->AddGame((char)1);
	m_MultiPlayerLobby->AddGame((char)2);
	m_MultiPlayerLobby->AddGame((char)3);
	m_MultiPlayerLobby->AddGame((char)4);

	memset(buffer,0,255);
	int inDataLength=recv(Socket,buffer,255,0);
	IDpacket spkt;
	memcpy(spkt.c,buffer,inDataLength);
	//char id = spkt.p.packetID;
	char id = spkt.p.packetID&packetidMask;
	//id = id<<4;
	//id = id>>4;
	int pID=0;
	pID=(int)id;
	//pID=id;
	if(pID==SP_SL)
	{
		ServerPacket_SyncLobby spsl;
		memcpy(&spsl,buffer,sizeof(ServerPacket_SyncLobby));
		m_ClientID=spsl.clientID;
		short numPinGame =0;
		numPinGame= SPSLGOneMask&spsl.playersInGame;
		m_MultiPlayerLobby->GetMultiPlayerGameDataByID((char)1)->SetNumPlayers((int)numPinGame);
		numPinGame =0;
		numPinGame = spsl.playersInGame;
		numPinGame=numPinGame<<8;
		numPinGame=numPinGame>>12;
		numPinGame= SPSLGOneMask&numPinGame;
		m_MultiPlayerLobby->GetMultiPlayerGameDataByID((char)2)->SetNumPlayers((int)numPinGame);
		numPinGame =0;
		numPinGame = spsl.playersInGame;
		numPinGame=numPinGame<<4;
		numPinGame=numPinGame>>12;
		numPinGame= SPSLGOneMask&numPinGame;
		m_MultiPlayerLobby->GetMultiPlayerGameDataByID((char)3)->SetNumPlayers((int)numPinGame);
		numPinGame =0;
		numPinGame = spsl.playersInGame;
		numPinGame=numPinGame>>12;
		numPinGame= SPSLGOneMask&numPinGame;
		m_MultiPlayerLobby->GetMultiPlayerGameDataByID((char)4)->SetNumPlayers((int)numPinGame);
		int numplayersinlobby=0;
		numplayersinlobby=(int)spsl.playersInLobby;
		m_MultiPlayerLobby->SetNumPlayersInLobby(numplayersinlobby);		
	}
	//m_ClientID=(short)4;
	ClientPacket_JoinServer cpjs;
	cpjs.packetID=(char)0;
	cpjs.clientID=m_ClientID;
	std::string name = "Dracono99\0";
	memcpy(cpjs.clientName,name.c_str(),sizeof(name));
	char sbuffer[256];
	memcpy(sbuffer,&cpjs,sizeof(cpjs));
	SendPacket(sbuffer);	
	u_long iMode=1;
	ioctlsocket(Socket,FIONBIO,&iMode);
	memset(buffer,0,255);
	m_Online=true;
	ResetMoveFlags=true;
	m_ReceivingThread=std::thread([&]()
	{
		//std::string tmsg;
		do
		{
			memset(buffer,0,255);
			int inDataLengthR=recv(Socket,buffer,256,0);
			int nError=WSAGetLastError();
			if(nError!=WSAEWOULDBLOCK&&nError!=0)
			{
				m_Online = false;
			}
			else
			{
				if(inDataLengthR>0)
				{
					IDpacket spkt;
					memcpy(spkt.c,buffer,sizeof(char));
					char id = spkt.p.packetID&packetidMask;				
					int pID=0;
					pID=(int)id;
					switch(pID)
					{
					case CP_JS:
						{
							break;
						}
					case CP_JG:
						{
							break;
						}
					case CP_ME:
						{
							break;
						}
					case CP_FE:
						{
							break;
						}
					case CP_QG:
						{
							break;
						}
					case CP_QS:
						{
							break;
						}
					case SP_SL:
						{
							ServerPacket_SyncLobby spsl;
							memcpy(&spsl,buffer,sizeof(ServerPacket_SyncLobby));
							gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(1.0,[&,spsl](){
								if(m_Online)
								{									
									short numPinGame =0;
									numPinGame= SPSLGOneMask&spsl.playersInGame;									
									m_MultiPlayerLobby->GetMultiPlayerGameDataByID((char)1)->SetNumPlayers((int)numPinGame);
									numPinGame =0;
									numPinGame= SPSLGTwoMask&spsl.playersInGame;
									numPinGame=numPinGame>>4;
									numPinGame=numPinGame&SPSLGOneMask;
									m_MultiPlayerLobby->GetMultiPlayerGameDataByID((char)2)->SetNumPlayers((int)numPinGame);
									numPinGame =0;
									numPinGame= SPSLGThreeMask&spsl.playersInGame;
									numPinGame=numPinGame>>8;
									numPinGame=numPinGame&SPSLGOneMask;
									m_MultiPlayerLobby->GetMultiPlayerGameDataByID((char)3)->SetNumPlayers((int)numPinGame);
									numPinGame =0;
									numPinGame= SPSLGFourMask&spsl.playersInGame;
									numPinGame=numPinGame>>12;
									numPinGame=numPinGame&SPSLGOneMask;
									m_MultiPlayerLobby->GetMultiPlayerGameDataByID((char)4)->SetNumPlayers((int)numPinGame);
									int numplayersinlobby=0;
									numplayersinlobby=(int)spsl.playersInLobby;
									m_MultiPlayerLobby->SetNumPlayersInLobby(numplayersinlobby);
								}
							}));
							break;
						}
					case SP_CJG:
						{
							ServerPacket_ClientJoinGame spcjg;
							memcpy(&spcjg,buffer,sizeof(ServerPacket_ClientJoinGame));							
							gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(1.0,[&,spcjg](){
								if(m_Online)
								{
									PlayerData d;
									d.clientID=spcjg.clientID;
									if (m_MultiPlayerLobby->GetMultiPlayerGameDataByID(spcjg.gameID)!=NULL)
									{
										m_MultiPlayerLobby->GetMultiPlayerGameDataByID(spcjg.gameID)->AddPlayerToGame(d);
									}
								}
							}));
							break;
						}
					case SP_SG:
						{
							ServerPacket_SyncGame sp;
							memcpy(&sp,buffer,sizeof(ServerPacket_SyncGame));
							gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(1.0,[&,sp](){
								if(m_Online)
								{
									if (m_JoinedGameID>0)
									{
										m_MultiPlayerLobby->GetMultiPlayerGameDataByID(m_JoinedGameID)->HandleSyncPacket(sp);
									}									
								}
							}));
							break;
						}
					case SP_ME:
						{
							ServerPacket_MoveEvent sp;
							memcpy(&sp,buffer,sizeof(ServerPacket_MoveEvent));
							gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(1.0,[&,sp](){
								if(m_Online)
								{
									if (m_JoinedGameID>0)
									{
										PlayerData d = m_MultiPlayerLobby->GetMultiPlayerGameDataByID(m_JoinedGameID)->GetPlayerDataByID(sp.clientID);
										char moveFlags = sp.packetID;
										moveFlags=moveFlags>>4;
										d.moveFlags=SPSLGOneMask&moveFlags;
										m_MultiPlayerLobby->GetMultiPlayerGameDataByID(m_JoinedGameID)->SetPlayerDataByID(d);
									}
								}
							}));
							break;
						}
					case SP_FE:
						{
							ServerPacket_FireEvent sp;
							memcpy(&sp,buffer,sizeof(ServerPacket_FireEvent));
							gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(1.0,[&,sp](){
								if(m_Online)
								{
									if (m_JoinedGameID>0)
									{
										m_MultiPlayerLobby->GetMultiPlayerGameDataByID(m_JoinedGameID)->HandleFireEventPacket(sp);
									}
								}
							}));
							break;
						}
					case SP_GO:
						{
							ServerPacket_GameOver sp;
							memcpy(&sp,buffer,sizeof(ServerPacket_GameOver));
							gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(1.0,[&,sp](){
								if(m_Online)
								{
									if (m_JoinedGameID>0)
									{
										mNameOfWinner=sp.winnerName;
										m_MultiPlayerLobby->GetMultiPlayerGameDataByID(m_JoinedGameID)->GameOverEvent();
									}
								}							
							}));
							break;
						}
					case SP_CQG:
						{
							ServerPacket_ClientQuitGame sp;
							memcpy(&sp,buffer,sizeof(ServerPacket_ClientQuitGame));
							gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(1.0,[&,sp](){
								if(m_Online)
								{
									if (m_MultiPlayerLobby->GetMultiPlayerGameDataByID(sp.gameID)!=NULL)
									{
										m_MultiPlayerLobby->GetMultiPlayerGameDataByID(sp.gameID)->RemovePlayerFromGame(sp.clientID);
									}									
								}
							}));
							break;
						}
					default:
						{
							break;
						}
					}
				}
			}
		} while (m_Online==true);
	});
}
void MultiPlayerCore::PowerDownMultiplayerCore()
{
	if (m_Online)
	{
		ClientPacket_QuitServer sp;
		sp.clientID=m_ClientID;
		sp.gameID=m_JoinedGameID;
		sp.packetID=(char)5;
		char c[256];
		memcpy(c,&sp,sizeof(ClientPacket_QuitServer));
		SendPacket(c);
	}
	m_Online=false;
	if(m_ReceivingThread.joinable())
	{
		m_ReceivingThread.join();		
	}
	if(m_MultiPlayerLobby!=NULL)
	{
		delete m_MultiPlayerLobby;
	}

	// shut down socket n tie up loose ends
	shutdown(Socket,SD_SEND);

	// close socket completely
	closesocket(Socket);

	// clean up winsock
	WSACleanup();
}
BaseGameEntity* MultiPlayerCore::GetOnlineEntityById(int id)
{
	for (auto entities : m_OnlineEntities)
	{
		if (entities->GetGamePlayObject()!=NULL)
		{
			if(entities->GetGamePlayObject()->m_ObjectID==id)
			{
				return entities;
			}
		}
	}
	return NULL;
}

int MultiPlayerCore::GetNumFromCharViaBitEncoding(char num)
{
	if(!(num|CHAR8BM))
	{
		return 8;
	}
	else if(!(num|CHAR7BM))
	{
		return 7;
	}
	else if(!(num|CHAR6BM))
	{
		return 6;
	}
	else if(!(num|CHAR5BM))
	{
		return 5;
	}
	else if(!(num|CHAR4BM))
	{
		return 4;
	}
	else if(!(num|CHAR3BM))
	{
		return 3;
	}
	else if(!(num|CHAR2BM))
	{
		return 2;
	}
	else if(!(num|CHAR1BM))
	{
		return 1;
	}
	else if(!(num|CHAR0BM))
	{
		return 0;
	}
	else
	{
		return 0;
	}
}

void MultiPlayerCore::HandleClientJoinGame(char clID,char gameID)
{
	if (m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gameID)->GetPlayerDataByID(clID).clientID==-1)// player doesnt exist in that game
	{
		BaseGameEntity* newPlayer = new BaseGameEntity();
		BaseGameplayObject* bgpo= new BaseGameplayObject();
		bgpo->m_ObjectID=(int)clID;
		newPlayer->SetBaseGamePlayObject(bgpo);
		bgpo->m_Owner=newPlayer;
		DragonMeshCollisionObject* p = new DragonMeshCollisionObject();
		GraphicsSchematic playerShip;
		playerShip.mName="PlayerShipCollision";
		playerShip.mMeshFileName="DarkFighter3_LP.X";
		playerShip.mMeshDiffuseTexName="NULL";
		playerShip.mMeshNormalTexName="NULL";
		playerShip.mMeshSpecTexName="NULL";
		gd3dApp->GetEngine()->GetPhysicsCore()->SetUpMesh(p,gd3dApp->GetEngine()->GetGraphicsCore()->CreateCollisionMesh(playerShip.mMeshFileName));
		newPlayer->SetBasePhysicsEntity(p);
		p->SetMassAndInverseTensor(100.0f);
		//p->m_Position=D3DXVECTOR3(0.0f,500.0f,0.0f);
		GraphicsSchematic playerShipGfx;
		playerShipGfx.mName="PlayerShipGfx";
		playerShipGfx.mMeshFileName="darkfghtr3.X";
		playerShipGfx.mMeshDiffuseTexName="dark_fighter_3_DIF.jpg";
		playerShipGfx.mMeshNormalTexName="dark_fighter_3_BMP.jpg";
		playerShipGfx.mMeshSpecTexName="dark_fighter_3_SPC.jpg";
		gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(newPlayer,playerShipGfx);
		p->m_Position=D3DXVECTOR3(0.0f,0.0f,0.0f);
		PlayerData newPlayerdata;
		newPlayerdata.clientID=clID;
		m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gameID)->AddPlayerToGame(newPlayerdata);
		m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gameID)->GetMapOfEntities().insert(std::make_pair(clID,newPlayer));
	}
}
void MultiPlayerCore::HandleGamePlayerUpdateData(PlayerData pData)
{
	m_MultiPlayerLobby->GetMultiPlayerGameDataByID(m_JoinedGameID)->SetPlayerDataByID(pData);
}