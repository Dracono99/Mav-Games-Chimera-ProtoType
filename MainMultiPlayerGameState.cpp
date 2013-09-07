#include "MultiPlayerCore.h"
#include "MultiPlayerMenuElements.h"
#include "MainMultiPlayerGameState.h"
#include "MainGameManager.h"
#include "FreeForm3DCamera.h"
#include "BaseGameEntity.h"
#include "DragonMeshCollisionObject.h"
#include "DragonPhysicsEngine.h"
#include "GraphicsCore.h"
#include "ChimeraGameEngine.h"
#include "InputCore.h"
#include "MainGameStateIDs.h"
#include "MenuFSM.h"
#include "MenuStateIds.h"
#include "BaseGameplayObject.h"
#include "GamePlayCore.h"
#include "BaseMpOnlineAIObject.h"
#include "MultiPlayerCore.h"
#include "MpPacketDefs.h"
#include "PacketStructs.h"
#include "MessageSystem.h"
#include <iostream>

MainMultiPlayerGameState::MainMultiPlayerGameState()
{
	m_Camera=NULL;
	m_MoveForward=false;
	m_MoveForwardState=false;
	m_MoveBackward=false;
	m_MoveBackwardState=false;
	m_MoveRight=false;
	m_MoveRightState=false;
	m_MoveLeft=false;
	m_canFire=true;
	m_MoveLeftState=false;
	m_Player=NULL;
	mStateID=MainMultiPlayerGameStateID;
	m_Camera = new FreeForm3DCamera(D3DXVECTOR3(0.0f,30.0f,-50.0f));
	// begin deffinition of bullets
	GraphicsSchematic Bullets;
	Bullets.mName="BulletSchematic";
	Bullets.mMeshFileName="TankBullet.X";
	Bullets.mMeshDiffuseTexName="MaPZone[TankBullet_diffuse].png";
	Bullets.mMeshNormalTexName="MaPZone[TankBullet_bump].png";
	Bullets.mMeshSpecTexName="TankBulletSpec.bmp";
	gd3dApp->GetEngine()->GetGraphicsCore()->RegisterGraphicsSchematic(Bullets);
	// end definition of bullets
	//m_Player = new BaseGameEntity();
	//BaseGameplayObject* bgpo= new BaseGameplayObject();
	//bgpo->m_HitEffectApplicaple=true;
	//m_Player->SetBaseGamePlayObject(bgpo);
	//bgpo->m_Owner=m_Player;
	//DragonMeshCollisionObject* p = new DragonMeshCollisionObject();
	//p->m_isCollisionDetectionOn=false;
	//GraphicsSchematic playerShip;
	//playerShip.mName="PlayerShipCollision";
	//playerShip.mMeshFileName="DarkFighter3_LP.X";
	//playerShip.mMeshDiffuseTexName="NULL";
	//playerShip.mMeshNormalTexName="NULL";
	//playerShip.mMeshSpecTexName="NULL";
	//gd3dApp->GetEngine()->GetPhysicsCore()->SetUpMesh(p,gd3dApp->GetEngine()->GetGraphicsCore()->CreateCollisionMesh(playerShip.mMeshFileName));
	//m_Player->SetBasePhysicsEntity(p);
	//GraphicsSchematic playerShipGfx;
	//playerShipGfx.mName="PlayerShipGfx";
	//playerShipGfx.mMeshFileName="darkfghtr3.X";
	//playerShipGfx.mMeshDiffuseTexName="dark_fighter_3_DIF.jpg";
	//playerShipGfx.mMeshNormalTexName="dark_fighter_3_BMP.jpg";
	//playerShipGfx.mMeshSpecTexName="dark_fighter_3_SPC.jpg";
	//gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(m_Player,playerShipGfx);
	//p->m_Position=D3DXVECTOR3(0.0f,0.0f,0.0f);
	//m_Entities.push_back(m_Player);
	//m_Galaxy=new BaseGameEntity();
	//BaseGameplayObject* worldObj = new BaseGameplayObject();
	//worldObj->m_HitEffectApplicaple=false;
	//m_Galaxy->SetBaseGamePlayObject(worldObj);
	//PhysicsSphere* c = new PhysicsSphere();
	//c->m_isCollisionDetectionOn=false;
	//GraphicsSchematic galaxy;
	//galaxy.mName="GalaxySchem";
	//galaxy.mMeshFileName="VeryBigSkyDome.X";
	//galaxy.mMeshDiffuseTexName="BigSpaceTex.jpg";
	//galaxy.mMeshNormalTexName="BigWhiteTex.png";
	//galaxy.mMeshSpecTexName="BigBlackTex.png";
	//gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(m_Galaxy,galaxy);
	//m_Galaxy->SetBasePhysicsEntity(c);
	//m_Entities.push_back(m_Galaxy);
	//GraphicsSchematic Bullets;
	//Bullets.mName="BulletSchematic";
	//Bullets.mMeshFileName="BulletProjectile.X";
	//Bullets.mMeshDiffuseTexName="MaPZone[Fire_diffuse].png";
	//Bullets.mMeshNormalTexName="MaPZone[Fire_Norm].png";
	//Bullets.mMeshSpecTexName="BigWhiteTex.png";
	//gd3dApp->GetEngine()->GetGraphicsCore()->RegisterGraphicsSchematic(Bullets);
	//GraphicsSchematic EnemyShip;
	//EnemyShip.mName="EnemyShipMeshSchem";
	//EnemyShip.mMeshFileName="BattleShip_5U_po.X";
	//EnemyShip.mMeshDiffuseTexName="battleship_5U_DIF.jpg";
	//EnemyShip.mMeshNormalTexName="battleship_5U_BMP.jpg";
	//EnemyShip.mMeshSpecTexName="battleship_5U_SPC.jpg";
	//gd3dApp->GetEngine()->GetGraphicsCore()->RegisterGraphicsSchematic(EnemyShip);
	//BaseGameEntity* en = new BaseGameEntity();
	//BaseGameplayObject* engpo = new BaseGameplayObject();
	//engpo->m_ObjectID=0;
	//engpo->m_HitEffectApplicaple=false;
	//engpo->m_mpOlineEntityAI = new BaseMpOnlineAIObject();
	//engpo->m_mpOlineEntityAI->m_Owner=en;
	//DragonMeshCollisionObject* encmp = new DragonMeshCollisionObject();
	//encmp->m_isCollisionDetectionOn=false;
	////PhysicsSphere* encmp = new PhysicsSphere(50.0f);
	//encmp->setInverseMass(1.0f/1000.0f);
	//encmp->setMass(1000.0f);
	//en->SetBaseGamePlayObject(engpo);
	//gd3dApp->GetEngine()->GetPhysicsCore()->SetUpMesh(encmp,gd3dApp->GetEngine()->GetGraphicsCore()->CreateCollisionMesh("BattleShip_5U_LP.X"));
	//en->SetBasePhysicsEntity(encmp);
	//gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObjectWithRegisteredSchematic(en,"EnemyShipMeshSchem");
	//encmp->m_Position=DragonXVector3(0.0f,0.0f,0.0f);
	//gd3dApp->GetEngine()->GetMultiPlayerCore()->RegisterOnlineEntity(en);
	//m_Entities.push_back(en);
}
MainMultiPlayerGameState::~MainMultiPlayerGameState()
{
	if (m_Camera!=NULL)
	{
		delete m_Camera;
	}
	for (auto entities : m_Entities)
	{
		delete entities;
	}
}
void MainMultiPlayerGameState::Enter()
{
	m_MoveForward=false;
	m_MoveForwardState=false;
	m_MoveBackward=false;
	m_MoveBackwardState=false;
	m_MoveRight=false;
	m_MoveRightState=false;
	m_MoveLeft=false;
	m_canFire=true;
	m_MoveLeftState=false;
	m_GamePaused=false;
	gd3dApp->GetEngine()->GetMultiPlayerCore()->InitializeMultiplayerCore();
	if(gd3dApp->GetEngine()->GetMultiPlayerCore()->GetOnlineStatus())
	{
		gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(MainMultiPlayerMenuStateID);
	}
}
void MainMultiPlayerGameState::Exit()
{
	m_GamePaused=true;
	if(gd3dApp->GetEngine()->GetMultiPlayerCore()->GetOnlineStatus())
	{
		gd3dApp->GetEngine()->GetMultiPlayerCore()->PowerDownMultiplayerCore();
	}
}
void MainMultiPlayerGameState::OnLostDevice()
{
}
void MainMultiPlayerGameState::OnResetDevice()
{
}
void MainMultiPlayerGameState::Update(float dt)
{
	if(!gd3dApp->GetEngine()->GetMultiPlayerCore()->GetOnlineStatus())
	{
		gd3dApp->SwitchGameState(MainGameMenuStateID);
	}
	bool sendmsg = false;
	//float x=m_Player->GetPhysicsPointer()->m_Position.x;
	//float z=m_Player->GetPhysicsPointer()->m_Position.z;
	if (gd3dApp->GetEngine()->GetMultiPlayerCore()->ResetMoveFlags)
	{
		m_MoveForward=false;
		m_MoveForwardState=false;
		m_MoveBackward=false;
		m_MoveBackwardState=false;
		m_MoveRight=false;
		m_MoveRightState=false;
		m_MoveLeft=false;
		m_canFire=true;
		m_MoveLeftState=false;
		gd3dApp->GetEngine()->GetMultiPlayerCore()->ResetMoveFlags=false;
	}
	if (gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID>0)
	{
		if(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->mGameInSession)
		{			
			if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_StartButtonPress)
			{
				if(m_GamePaused)
				{
					m_GamePaused=false;
					gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(MultiplayerGameplayMenuStateID);
				}
				else
				{
					m_GamePaused=true;
					gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(MultiplayerGamePlayPauseMenuStateID);
				}
			}

			if(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftStickActive)// player straife
			{
				m_Camera->MoveRight(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftStick_X_Val*dt/1000);
				m_Camera->MoveForward(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftStick_Y_Val*dt/1000);
			}

			if(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightStickActive)// player pitch n roll
			{
				m_Camera->Yaw(1.0f*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightStick_X_Val*dt/25000);
				m_Camera->Pitch(-1.0f*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightStick_Y_Val*dt/25000);
			}
			if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftTriggerActive)
			{
				m_Camera->MoveUp(-1.0f*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftTRiggerValue*dt/10);
			}
			if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightTriggerActive)
			{
				m_Camera->MoveUp(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightTriggerValue*dt/10);
			}
			if(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadUpButtonDown)
			{
				m_MoveForward=true;
				if (m_MoveForward!=m_MoveForwardState)
				{
					m_MoveForwardState=m_MoveForward;
					BaseGameEntity* player=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetMapOfEntities().find(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID)->second;
					//player->GetPhysicsPointer()->m_LinearVelocity=player->GetPhysicsPointer()->m_QuatRot.Forward();
					sendmsg=true;
				}
			}
			else
			{
				m_MoveForward=false;
				if (m_MoveForward!=m_MoveForwardState)
				{
					m_MoveForwardState=m_MoveForward;
					sendmsg=true;
				}
			}
			if(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadDownButtonDown)
			{
				m_MoveBackward=true;
				if (m_MoveBackward!=m_MoveBackwardState)
				{
					m_MoveBackwardState=m_MoveBackward;
					BaseGameEntity* player=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetMapOfEntities().find(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID)->second;
					//player->GetPhysicsPointer()->m_LinearVelocity=player->GetPhysicsPointer()->m_QuatRot.Forward()*-1.0f;
					sendmsg=true;
				}
			}
			else
			{
				m_MoveBackward=false;
				if (m_MoveBackward!=m_MoveBackwardState)
				{
					m_MoveBackwardState=m_MoveBackward;
					sendmsg=true;
				}
			}
			if(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadRightButtonDown)
			{
				m_MoveRight=true;
				if (m_MoveRight!=m_MoveRightState)
				{
					m_MoveRightState=m_MoveRight;
					BaseGameEntity* player=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetMapOfEntities().find(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID)->second;
					//player->GetPhysicsPointer()->m_RotationalVelocity.y=1.047197551f;
					sendmsg=true;
				}
			}
			else
			{
				m_MoveRight=false;
				if (m_MoveRight!=m_MoveRightState)
				{
					m_MoveRightState=m_MoveRight;
					sendmsg=true;
				}
			}
			if(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadLeftButtonDown)
			{
				m_MoveLeft=true;
				if (m_MoveLeft!=m_MoveLeftState)
				{
					m_MoveLeftState=m_MoveLeft;
					BaseGameEntity* player=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetMapOfEntities().find(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID)->second;
					//player->GetPhysicsPointer()->m_RotationalVelocity.y=-1.047197551f;
					sendmsg=true;
				}
			}
			else
			{
				m_MoveLeft=false;
				if (m_MoveLeft!=m_MoveLeftState)
				{
					m_MoveLeftState=m_MoveLeft;
					sendmsg=true;
				}
			}
			if ((m_MoveBackwardState==false)&&(m_MoveForwardState==false))
			{
				BaseGameEntity* player=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetMapOfEntities().find(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID)->second;
				//player->GetPhysicsPointer()->m_LinearVelocity=DragonXVector3(0.0f,0.0f,0.0f);
				//player->GetPhysicsPointer()->m_LinearForceAccum=DragonXVector3(0.0f,0.0f,0.0f);
			}
			if ((m_MoveLeftState==false)&&(m_MoveRightState==false))
			{
				BaseGameEntity* player=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetMapOfEntities().find(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID)->second;
				//player->GetPhysicsPointer()->m_RotationalVelocity.y=0.0f;
				//player->GetPhysicsPointer()->m_AngularForceAccum=DragonXVector3(0.0f,0.0f,0.0f);
			}
			if(sendmsg)
			{
				ClientPacket_MoveEvent cp;
				char moveFlags=(char)0;
				if (m_MoveLeftState)
				{
					moveFlags=moveFlags|moveFlagsLeftMask;
				}
				if (m_MoveRightState)
				{
					moveFlags=moveFlags|moveFlagsRightMask;
				}
				if (m_MoveBackwardState)
				{
					moveFlags=moveFlags|moveFlagsBackMask;
				}
				if (m_MoveForwardState)
				{
					moveFlags=moveFlags|moveFlagsForwardMask;
				}
				moveFlags=moveFlags<<4;
				moveFlags=moveFlags|(char)2;
				cp.packetID=moveFlags;
				cp.clientID=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID;
				cp.gameID= gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID;
				char c[256];
				memcpy(c,&cp,sizeof(ClientPacket_MoveEvent));
				gd3dApp->GetEngine()->GetMultiPlayerCore()->SendPacket(c);
			}
			if(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_RightShoulderPress&&m_canFire)
			{
				if (gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetPlayerDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID).health>0)
				{
					ClientPacket_FireEvent cp;
					m_canFire=false;
					cp.packetID=(char)3;
					cp.targetID=0;
					cp.gameID= gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID;
					cp.clientID=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID;
					char c[256];
					memcpy(c,&cp,sizeof(ClientPacket_FireEvent));
					gd3dApp->GetEngine()->GetMultiPlayerCore()->SendPacket(c);
					PlayerFire();
					gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(1000.0,[&](){
						m_canFire=true;
					}));
				}
			}
			if(m_GamePaused)
			{
				if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_B_ButtonPress)
				{
					m_GamePaused=false;
					gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(MultiplayerGameplayMenuStateID);
				}
			}
			gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->UpdateGame(dt);
			gd3dApp->GetEngine()->GetPhysicsCore()->UpdateListOfEntitiesAndGenerateContacts(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->m_GameEntities,dt);
			//gd3dApp->GetEngine()->GetMultiPlayerCore()->GetOnlineEntityById(0)->GetGamePlayObject()->m_mpOlineEntityAI->UpdateOnlineEntity(dt);
			gd3dApp->GetEngine()->GetPhysicsCore()->ClearContacts();
		}
	}
	gd3dApp->GetEngine()->GetMenuSystem()->Update(dt);
}
void MainMultiPlayerGameState::Render()
{
	gd3dApp->GetEngine()->GetGraphicsCore()->mLightDirection=m_Camera->GetLook();
	D3DXMATRIX view;
	m_Camera->CalculateViewMatrix(&view);
	if (gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID>0)
	{
		if(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->mGameInSession)
		{
			gd3dApp->GetEngine()->GetGraphicsCore()->m_RenderShadows=true;
			gd3dApp->GetEngine()->GetGraphicsCore()->Render(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->m_GameEntities,view,m_Camera->GetPosition());
		}
	}
	gd3dApp->GetEngine()->GetMenuSystem()->Render();
}
void MainMultiPlayerGameState::PlayerFire()
{
	BaseGameEntity* player=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetMapOfEntities().find(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID)->second;
	BaseGameEntity* b = new BaseGameEntity();
	BaseGameplayObject* projctle = new BaseGameplayObject();
	projctle->m_HitEffectApplicaple=false;
	b->SetBaseGamePlayObject(projctle);
	PhysicsSphere* p = new PhysicsSphere(5.0f);
	//DragonMeshCollisionObject* p = new DragonMeshCollisionObject();
	//gd3dApp->GetEngine()->GetPhysicsCore()->SetUpMesh(p,gd3dApp->GetEngine()->GetGraphicsCore()->CreateCollisionMesh("BulletProjectile.X"));
	b->SetBasePhysicsEntity(p);
	gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObjectWithRegisteredSchematic(b,"BulletSchematic");
	p->m_QuatRot=player->GetPhysicsPointer()->m_QuatRot;
	p->m_Position=player->GetPhysicsPointer()->getPointInWorldSpace(D3DXVECTOR3(0.0f,1.7f,7.0f));
	p->m_LinearVelocity=player->GetPhysicsPointer()->m_QuatRot.Forward()*80.0f;
	gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->m_GameEntities.push_back(b);
}