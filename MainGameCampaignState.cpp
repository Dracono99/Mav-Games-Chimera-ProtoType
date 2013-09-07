#include "MainGameCampaignState.h"
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
#include "MessageSystem.h"

MainGameCampaignState::MainGameCampaignState()
{
	m_Camera=NULL;
	m_Player=NULL;
	m_FreeCameraToggle=false;
	mStateID=MainCampaignDemoStateID;
	m_Camera = new FreeForm3DCamera(D3DXVECTOR3(0.0f,0.0f,-50.0f));
	// begin creation of player object
	m_Player = new BaseGameEntity();
	BaseGameplayObject* bgpo= new BaseGameplayObject();
	bgpo->m_HitEffectApplicaple=true;
	m_Player->SetBaseGamePlayObject(bgpo);
	bgpo->m_Owner=m_Player;
	DragonMeshCollisionObject* p = new DragonMeshCollisionObject();
	GraphicsSchematic playerShip;
	playerShip.mName="PlayerShipCollision";
	playerShip.mMeshFileName="DarkFighter3_LP.X";
	playerShip.mMeshDiffuseTexName="NULL";
	playerShip.mMeshNormalTexName="NULL";
	playerShip.mMeshSpecTexName="NULL";
	gd3dApp->GetEngine()->GetPhysicsCore()->SetUpMesh(p,gd3dApp->GetEngine()->GetGraphicsCore()->CreateCollisionMesh(playerShip.mMeshFileName));
	m_Player->SetBasePhysicsEntity(p);
	p->SetMassAndInverseTensor(1000.0f);
	//p->m_Position=D3DXVECTOR3(0.0f,500.0f,0.0f);
	GraphicsSchematic playerShipGfx;
	playerShipGfx.mName="PlayerShipGfx";
	playerShipGfx.mMeshFileName="darkfghtr3.X";
	playerShipGfx.mMeshDiffuseTexName="dark_fighter_3_DIF.jpg";
	playerShipGfx.mMeshNormalTexName="dark_fighter_3_BMP.jpg";
	playerShipGfx.mMeshSpecTexName="dark_fighter_3_SPC.jpg";
	gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(m_Player,playerShipGfx);
	p->m_Position=D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_Entities.push_back(m_Player);
	// end creation of player object

	// begin creation of galaxy skybox object
	//m_Galaxy=new BaseGameEntity();
	//BaseGameplayObject* worldObj = new BaseGameplayObject();
	//worldObj->m_HitEffectApplicaple=false;
	//m_Galaxy->SetBaseGamePlayObject(worldObj);
	//PhysicsSphere* c = new PhysicsSphere();
	//c->m_isCollisionDetectionOn=false;
	//GraphicsSchematic galaxy;
	//galaxy.mName="GalaxySchem";
	//galaxy.mMeshFileName="BigGalaxy.X";
	////galaxy.mMeshFileName="LowishPolyHeavyArms.X";
	//galaxy.mMeshDiffuseTexName="BigSpaceTex.jpg";
	//galaxy.mMeshNormalTexName="MaPZone[Fire_Norm].png";
	//galaxy.mMeshSpecTexName="BigBlackTex.png";
	//gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObject(m_Galaxy,galaxy);
	//m_Galaxy->SetBasePhysicsEntity(c);
	//m_Entities.push_back(m_Galaxy);
	// end creation of galaxy skybox

	// begin deffinition of bullets
	GraphicsSchematic Bullets;
	Bullets.mName="BulletSchematic";
	Bullets.mMeshFileName="TankBullet.X";
	Bullets.mMeshDiffuseTexName="MaPZone[TankBullet_diffuse].png";
	Bullets.mMeshNormalTexName="MaPZone[TankBullet_bump].png";
	Bullets.mMeshSpecTexName="TankBulletSpec.bmp";
	gd3dApp->GetEngine()->GetGraphicsCore()->RegisterGraphicsSchematic(Bullets);
	// end definition of bullets

	// begin definition of target ship object
	BaseGameEntity* en = new BaseGameEntity();
	AdvancedGraphicsSchematic EnemyShip;
	EnemyShip.mName="EnemyShipMeshSchem";
	EnemyShip.mMeshFileName="6kPolyCubeNoSpacing.X";//TankMP 6kPolyCubeNoSpacing M1AbramsTankMPFull MPTankReCentered.X . MPTankFullRecentered
	EnemyShip.mUndamagedDiffuseTexName="6KpolyDestructableCubeUnDamagedDiff.bmp ";// 6KpolyDestructableCubeUnDamagedDiff.bmp tankDiffuseMapWithAO.bmp
	EnemyShip.mUndamagedNormalTexName="MaPZone[6KpolyDestroyMeshUnDamaged_bump].png";//MaPZone[6KpolyDestroyMeshUnDamaged_bump].png   tankNormalsMap.bmp
	EnemyShip.mUndamagedSpecTexName="6kcibespecUndamaged.png ";// 6kcibespecUndamaged.png tankSpecularMap.bmp
	EnemyShip.mDamagedDiffuseTexName="DamagedCubeDiff.bmp";// DamagedCubeDiff.bmp   DamagedTankDiffusewithAO.bmp
	EnemyShip.mDamagedNormalTexName="MaPZone[Damaged6KCube_bump].bmp";//MaPZone[Damaged6KCube_bump].bmp   MaPZone[DamagedTankSet_bump].bmp
	EnemyShip.mDamagedSpecTexName="Damaged6kSpec.bmp";//Damaged6kSpec.bmp   DamagedTankSpec.bmp
	gd3dApp->GetEngine()->GetGraphicsCore()->CreateAdvancedGraphicsObject(en,EnemyShip);
	BaseGameplayObject* engpo = new BaseGameplayObject();
	engpo->m_HitEffectApplicaple=true;
	//DragonMeshCollisionObject* encmp = new DragonMeshCollisionObject();
	PhysicsOBB* encmp = new PhysicsOBB(DragonXVector3(0.0f,0.0f,0.0f),DragonXVector3(50.0f,50.0f,50.0f),1000000.0f);
	en->SetBaseGamePlayObject(engpo);
	//gd3dApp->GetEngine()->GetPhysicsCore()->SetUpMesh(encmp,gd3dApp->GetEngine()->GetGraphicsCore()->CreateCollisionMesh("MPTankFullCollisionMesh.X"));
	encmp->SetMassAndInverseTensor(1000000.0f);
	en->SetBasePhysicsEntity(encmp);
	//gd3dApp->GetEngine()->GetGraphicsCore()->CreateGraphicsObjectWithRegisteredSchematic(en,"EnemyShipMeshSchem");
	encmp->m_Position=DragonXVector3(0.0f,0.0f,500.0f);
	m_Entities.push_back(en);
	// end definition of target ship object
}
MainGameCampaignState::~MainGameCampaignState()
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
void MainGameCampaignState::Enter()
{
	m_GamePaused=false;
	mUpdateImposters=true;
	mCanFire=true;
	m_FreeCameraToggle=true;
	gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(GamePlayMenu);
}
void MainGameCampaignState::FireShip()
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
	D3DXQUATERNION Pitch(1.0f*sin(0.5f*m_Camera->GetPitch()),0.0f,0.0f,cos(0.5f*m_Camera->GetPitch()));
	D3DXQUATERNION Yaw(0.0f,1.0f*sin(0.5f*m_Camera->GetYaw()),0.0f,cos(0.5f*m_Camera->GetYaw()));
	//p->GetPhysicsPointer()->m_QuatRot*=Pitch;
	p->m_QuatRot*=Pitch;
	p->m_QuatRot*=Yaw;
	p->m_Position=m_Player->GetPhysicsPointer()->getPointInWorldSpace(D3DXVECTOR3(0.0f,0.0f,50.0f));
	p->m_LinearVelocity=m_Player->GetPhysicsPointer()->m_QuatRot.Forward()*200.0f;
	if(m_FreeCameraToggle)
	{
		p->m_Position=m_Camera->GetPosition();
		p->m_LinearVelocity=m_Camera->GetLook()*200.0f;
	}
	m_Entities.push_back(b);
}
void MainGameCampaignState::Exit()
{
	m_GamePaused=true;
}
void MainGameCampaignState::OnLostDevice()
{
}
void MainGameCampaignState::OnResetDevice()
{
}
void MainGameCampaignState::Update(float dt)
{
	if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_StartButtonPress)
	{
		if(m_GamePaused)
		{
			m_GamePaused=false;
			gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(GamePlayMenu);
		}
		else
		{
			m_GamePaused=true;
			gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(GamePauseMenuState);
		}
	}

	if(!m_GamePaused)
	{
		DragonXVector3 straife = m_Player->GetPhysicsPointer()->m_QuatRot.Right();
		DragonXVector3 pitch = m_Player->GetPhysicsPointer()->m_QuatRot.Forward();
		DragonXVector3 roll = m_Player->GetPhysicsPointer()->m_QuatRot.Up();

		if(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftStickActive)// player straife
		{
			m_Player->GetPhysicsPointer()->addForce(straife*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftStick_X_Val*dt*1000);
			m_Camera->MoveRight(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftStick_X_Val*dt/1000);
			m_Camera->MoveForward(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftStick_Y_Val*dt/1000);
		}

		if(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightStickActive)// player pitch n roll
		{
			m_Player->GetPhysicsPointer()->addTorque(m_Player->GetPhysicsPointer()->getDirectionInLocalSpace(straife*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightStick_Y_Val*dt*1000));
			m_Camera->Yaw(1.0f*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightStick_X_Val*dt/25000);

			m_Player->GetPhysicsPointer()->addTorque(m_Player->GetPhysicsPointer()->getDirectionInLocalSpace(pitch*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightStick_X_Val*-1.0f*dt*1000));
			m_Camera->Pitch(-1.0f*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightStick_Y_Val*dt/25000);
		}
		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftTriggerActive)
		{
			m_Player->GetPhysicsPointer()->addForce(pitch*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftTRiggerValue*-1000.0f);
			m_Camera->MoveUp(-1.0f*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mLeftTRiggerValue*dt/10);
		}
		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightTriggerActive)
		{
			m_Player->GetPhysicsPointer()->addForce(pitch*gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightTriggerValue*1000.0f);
			m_Camera->MoveUp(gd3dApp->GetEngine()->GetInputCore()->GetXinput()->mRightTriggerValue*dt/10);
		}

		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_Y_ButtonPress)
		{
			m_Player->GetPhysicsPointer()->m_RotationalVelocity.clear();
			m_Player->GetPhysicsPointer()->m_LinearVelocity.clear();
			//m_Player->GetPhysicsPointer()->addTorque(DragonXVector3(0.0f,1000000.0f,0.0f));
		}

		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_RightShoulderDown)
		{
			if(mCanFire)
			{
				mCanFire=false;
				FireShip();
				gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(100.0,[&](){
					mCanFire=true;
				}));
			}
		}

		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_B_ButtonPress)
		{
			if(m_FreeCameraToggle)
			{
				m_FreeCameraToggle=false;
			}
			else
			{
				m_FreeCameraToggle=true;
			}
		}
		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_X_ButtonPress)
		{
			mUpdateImposters=true;
		}
		gd3dApp->GetEngine()->GetPhysicsCore()->UpdateListOfEntitiesAndGenerateContacts(m_Entities,dt);
		gd3dApp->GetEngine()->GetGamePlayCore()->ApplyGamePlayEffects(m_Entities,gd3dApp->GetEngine()->GetPhysicsCore()->GetContactPairs());
		gd3dApp->GetEngine()->GetPhysicsCore()->ResolveContacts(dt);
	}
	else
	{
		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_B_ButtonPress)
		{
			m_GamePaused=false;
			gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(GamePlayMenu);
		}
	}
	gd3dApp->GetEngine()->GetMenuSystem()->Update(dt);
}
void MainGameCampaignState::Render()
{
	gd3dApp->GetEngine()->GetGraphicsCore()->m_UpdateImposters=mUpdateImposters;
	gd3dApp->GetEngine()->GetGraphicsCore()->mLightDirection=m_Camera->GetLook();
	D3DXMATRIX view;
	if (m_FreeCameraToggle)
	{
		m_Camera->CalculateViewMatrix(&view);
	}
	else
	{
		D3DXMatrixLookAtLH(&view,&m_Player->GetPhysicsPointer()->getPointInWorldSpace(D3DXVECTOR3(0.0f,10.0f,-30.0f)),&m_Player->GetPhysicsPointer()->getPointInWorldSpace(D3DXVECTOR3(0.0f,10.0f,0.0f)),&m_Player->GetPhysicsPointer()->m_QuatRot.Up());
	}
	gd3dApp->GetEngine()->GetGraphicsCore()->m_RenderShadows=false;
	gd3dApp->GetEngine()->GetGraphicsCore()->Render(m_Entities,view,m_Camera->GetPosition());
	gd3dApp->GetEngine()->GetMenuSystem()->Render();
	mUpdateImposters=false;
}