#include "MultiPlayerGamePlayMenu.h"
#include "MultiPlayerCore.h"
#include "MultiPlayerMenuElements.h"
#include "MenuEnums.h"
#include "MenuStateIds.h"
#include "ChimeraGameEngine.h"
#include "AudioCoreEnums.h"
#include "AudioCore.h"
#include "InputCore.h"
#include "MessageSystem.h"
#include "MainGameStateIDs.h"
#include "MenuFSM.h"
#include "GraphicsCore.h"
#include "PacketStructs.h"

MultiplayerGamePlayMenu::MultiplayerGamePlayMenu()
{
	mStateID=MultiplayerGameplayMenuStateID;
	m_XCurserButton=0;
	m_ButtonClickEffect=SoundEffectOne;
	m_buttonClicked=false;
	D3DXFONT_DESC fontDesc;
	fontDesc.Height          = 28;
	fontDesc.Width           = 12;
	fontDesc.Weight          = FW_LIGHT;
	fontDesc.MipLevels       = 0;
	fontDesc.Italic          = false;
	fontDesc.CharSet         = DEFAULT_CHARSET;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality         = DEFAULT_QUALITY;
	fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
	strcpy_s(fontDesc.FaceName,_T("MATRIX"));
	HR(D3DXCreateFontIndirect(gd3dDevice, &fontDesc, &mFont));

	HR(D3DXCreateSprite(gd3dDevice, &mSprite));
	m_buttonTex=gd3dApp->GetEngine()->GetGraphicsCore()->RegisterTexture("TechSliderBackGround.bmp");
	m_bkGrndTex=gd3dApp->GetEngine()->GetGraphicsCore()->RegisterTexture("MechWarsMainBackGround.bmp");
	m_SliderTex=gd3dApp->GetEngine()->GetGraphicsCore()->RegisterTexture("TechSliderSpriteTexture.bmp");
	m_LabelTex=gd3dApp->GetEngine()->GetGraphicsCore()->RegisterTexture("TechButtonsSpriteSheet.bmp");

	//HR(D3DXCreateTextureFromFile(gd3dDevice, "MainMenuBackGround.jpg", &m_bkGrndTex));
	//HR(D3DXCreateTextureFromFile(gd3dDevice, "Slider_BackGround.png", &m_buttonTex));
	//HR(D3DXCreateTextureFromFile(gd3dDevice, "Slider_Slider.png", &m_SliderTex));
	//HR(D3DXCreateTextureFromFile(gd3dDevice, "ExampleButtons512x512.png", &m_LabelTex));
	gd3dApp->GetEngine()->GetAudioCore()->LoadSound("BulletRicochetButtonClick.wav",m_ButtonClickEffect);
	RECT rct;
	GetClientRect(gd3dApp->getMainWnd(),&rct);
	ResetBkgrndScale();
	m_GuiSet = new GuiSet(m_bkGrndTexCentr,(int)rct.bottom/(int)16.0f);
	m_GuiSet->SetGuiElements(InitialiseGuiElements());
	m_GuiSet->MoveGuiSetTo(D3DXVECTOR3((float)rct.right-170.0f,(float)rct.bottom*0.2f,0.0f));
}
MultiplayerGamePlayMenu::~MultiplayerGamePlayMenu()
{
	if(m_GuiSet!=NULL)
	{
		delete m_GuiSet;
		m_GuiSet=NULL;
	}
}
void MultiplayerGamePlayMenu::MoveGuiElementsOffScreen()
{
	RECT rct;
	GetClientRect(gd3dApp->getMainWnd(),&rct);
	m_GuiSet->MoveGuiSetTo(D3DXVECTOR3((float)rct.right-170.0f,(float)rct.bottom*0.04f,0.0f));
}

std::vector<BaseGuiElement*> MultiplayerGamePlayMenu::InitialiseGuiElements()
{
	std::vector<BaseGuiElement*> V;
	RECT rct;
	GetClientRect(gd3dApp->getMainWnd(),&rct);
	V.push_back(new Button(mSprite,mFont,m_LabelTex,MPGPL_PlayerOneHealth,false,"Player One Health",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
	}));
	V.push_back(new Slider(mSprite,m_SliderTex,m_buttonTex,MPGPS_PlayerOneHealthSlider,true,0.0f,D3DXVECTOR3(16.0f, 32.0f, 0.0f),D3DXVECTOR3(128.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		float left = m_GuiSet->GetGuiElementById(MPGPS_PlayerOneHealthSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MPGPS_PlayerOneHealthSlider)->m_center.x;
		float right = m_GuiSet->GetGuiElementById(MPGPS_PlayerOneHealthSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MPGPS_PlayerOneHealthSlider)->m_center.x;
		float fDivisor = ((right-left)/100.0f);
		float vol = ((float)m_GuiSet->GetGuiElementById(MPGPS_PlayerOneHealthSlider)->GetSliderPos().x-left)/fDivisor;
		vol = (int)gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetPlayerDataByNumber(1).health/100.0f;
		if(vol<0.05f)
		{
			vol=0.000f;
		}
		if (vol > 1.0f)
		{
			vol = 1.0f;
		}
		m_GuiSet->GetGuiElementById(MPGPS_PlayerOneHealthSlider)->SetVol(vol);
		gd3dApp->GetEngine()->GetAudioCore()->SetStreamVolume(vol);
	}));
	V.push_back(new Button(mSprite,mFont,m_LabelTex,MPGPL_PlayerTwoHealth,false,"Player Two Health",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
	}));
	V.push_back(new Slider(mSprite,m_SliderTex,m_buttonTex,MPGPS_PlayerTwoHealthSlider,true,0.0f,D3DXVECTOR3(16.0f, 32.0f, 0.0f),D3DXVECTOR3(128.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		float left = m_GuiSet->GetGuiElementById(MPGPS_PlayerTwoHealthSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MPGPS_PlayerTwoHealthSlider)->m_center.x;
		float right = m_GuiSet->GetGuiElementById(MPGPS_PlayerTwoHealthSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MPGPS_PlayerTwoHealthSlider)->m_center.x;
		float fDivisor = ((right-left)/100.0f);
		float vol = ((float)m_GuiSet->GetGuiElementById(MPGPS_PlayerTwoHealthSlider)->GetSliderPos().x-left)/fDivisor;
		vol = (int)gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetPlayerDataByNumber(2).health/100.0f;
		if(vol<0.05f)
		{
			vol=0.000f;
		}
		if (vol > 1.0f)
		{
			vol = 1.0f;
		}
		m_GuiSet->GetGuiElementById(MPGPS_PlayerTwoHealthSlider)->SetVol(vol);
		gd3dApp->GetEngine()->GetAudioCore()->SetStreamVolume(vol);
	}));
	V.push_back(new Button(mSprite,mFont,m_LabelTex,MPGPL_PlayerThreeHealth,false,"Player Three Health",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
	}));
	V.push_back(new Slider(mSprite,m_SliderTex,m_buttonTex,MPGPS_PlayerThreeHealthSlider,true,0.0f,D3DXVECTOR3(16.0f, 32.0f, 0.0f),D3DXVECTOR3(128.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		float left = m_GuiSet->GetGuiElementById(MPGPS_PlayerThreeHealthSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MPGPS_PlayerThreeHealthSlider)->m_center.x;
		float right = m_GuiSet->GetGuiElementById(MPGPS_PlayerThreeHealthSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MPGPS_PlayerThreeHealthSlider)->m_center.x;
		float fDivisor = ((right-left)/100.0f);
		float vol = ((float)m_GuiSet->GetGuiElementById(MPGPS_PlayerThreeHealthSlider)->GetSliderPos().x-left)/fDivisor;
		vol = (int)gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetPlayerDataByNumber(3).health/100.0f;
		if(vol<0.05f)
		{
			vol=0.000f;
		}
		if (vol > 1.0f)
		{
			vol = 1.0f;
		}
		m_GuiSet->GetGuiElementById(MPGPS_PlayerThreeHealthSlider)->SetVol(vol);
		gd3dApp->GetEngine()->GetAudioCore()->SetStreamVolume(vol);
	}));
	V.push_back(new Button(mSprite,mFont,m_LabelTex,MPGPL_PlayerFourHealth,false,"Player Four Health",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
	}));
	V.push_back(new Slider(mSprite,m_SliderTex,m_buttonTex,MPGPS_PlayerFourHealthSlider,true,0.0f,D3DXVECTOR3(16.0f, 32.0f, 0.0f),D3DXVECTOR3(128.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		float left = m_GuiSet->GetGuiElementById(MPGPS_PlayerFourHealthSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MPGPS_PlayerFourHealthSlider)->m_center.x;
		float right = m_GuiSet->GetGuiElementById(MPGPS_PlayerFourHealthSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MPGPS_PlayerFourHealthSlider)->m_center.x;
		float fDivisor = ((right-left)/100.0f);
		float vol = ((float)m_GuiSet->GetGuiElementById(MPGPS_PlayerFourHealthSlider)->GetSliderPos().x-left)/fDivisor;
		vol = (int)gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetPlayerDataByNumber(4).health/100.0f;
		if(vol<0.05f)
		{
			vol=0.000f;
		}
		if (vol > 1.0f)
		{
			vol = 1.0f;
		}
		m_GuiSet->GetGuiElementById(MPGPS_PlayerFourHealthSlider)->SetVol(vol);
		gd3dApp->GetEngine()->GetAudioCore()->SetStreamVolume(vol);
	}));
	V.push_back(new Button(mSprite,mFont,m_LabelTex,MPGPL_PlayerFiveHealth,false,"Player Five Health",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
	}));
	V.push_back(new Slider(mSprite,m_SliderTex,m_buttonTex,MPGPS_PlayerFiveHealthSlider,true,0.0f,D3DXVECTOR3(16.0f, 32.0f, 0.0f),D3DXVECTOR3(128.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		float left = m_GuiSet->GetGuiElementById(MPGPS_PlayerFiveHealthSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MPGPS_PlayerFiveHealthSlider)->m_center.x;
		float right = m_GuiSet->GetGuiElementById(MPGPS_PlayerFiveHealthSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MPGPS_PlayerFiveHealthSlider)->m_center.x;
		float fDivisor = ((right-left)/100.0f);
		float vol = ((float)m_GuiSet->GetGuiElementById(MPGPS_PlayerFiveHealthSlider)->GetSliderPos().x-left)/fDivisor;
		vol = (int)gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetPlayerDataByNumber(5).health/100.0f;
		if(vol<0.05f)
		{
			vol=0.000f;
		}
		if (vol > 1.0f)
		{
			vol = 1.0f;
		}
		m_GuiSet->GetGuiElementById(MPGPS_PlayerFiveHealthSlider)->SetVol(vol);
		gd3dApp->GetEngine()->GetAudioCore()->SetStreamVolume(vol);
	}));
	V.push_back(new Button(mSprite,mFont,m_LabelTex,MPGPL_PlayerSixHealth,false,"Player Six Health",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
	}));
	V.push_back(new Slider(mSprite,m_SliderTex,m_buttonTex,MPGPS_PlayerSixHealthSlider,true,0.0f,D3DXVECTOR3(16.0f, 32.0f, 0.0f),D3DXVECTOR3(128.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		float left = m_GuiSet->GetGuiElementById(MPGPS_PlayerSixHealthSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MPGPS_PlayerSixHealthSlider)->m_center.x;
		float right = m_GuiSet->GetGuiElementById(MPGPS_PlayerSixHealthSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MPGPS_PlayerSixHealthSlider)->m_center.x;
		float fDivisor = ((right-left)/100.0f);
		float vol = ((float)m_GuiSet->GetGuiElementById(MPGPS_PlayerSixHealthSlider)->GetSliderPos().x-left)/fDivisor;
		vol = (int)gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetPlayerDataByNumber(6).health/100.0f;
		if(vol<0.05f)
		{
			vol=0.000f;
		}
		if (vol > 1.0f)
		{
			vol = 1.0f;
		}
		m_GuiSet->GetGuiElementById(MPGPS_PlayerSixHealthSlider)->SetVol(vol);
		gd3dApp->GetEngine()->GetAudioCore()->SetStreamVolume(vol);
	}));
	V.push_back(new Button(mSprite,mFont,m_LabelTex,MPGPL_PlayerSevenHealth,false,"Player Seven Health",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
	}));
	V.push_back(new Slider(mSprite,m_SliderTex,m_buttonTex,MPGPS_PlayerSevenHealthSlider,true,0.0f,D3DXVECTOR3(16.0f, 32.0f, 0.0f),D3DXVECTOR3(128.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		float left = m_GuiSet->GetGuiElementById(MPGPS_PlayerSevenHealthSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MPGPS_PlayerSevenHealthSlider)->m_center.x;
		float right = m_GuiSet->GetGuiElementById(MPGPS_PlayerSevenHealthSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MPGPS_PlayerSevenHealthSlider)->m_center.x;
		float fDivisor = ((right-left)/100.0f);
		float vol = ((float)m_GuiSet->GetGuiElementById(MPGPS_PlayerSevenHealthSlider)->GetSliderPos().x-left)/fDivisor;
		vol = (int)gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetPlayerDataByNumber(7).health/100.0f;
		if(vol<0.05f)
		{
			vol=0.000f;
		}
		if (vol > 1.0f)
		{
			vol = 1.0f;
		}
		m_GuiSet->GetGuiElementById(MPGPS_PlayerSevenHealthSlider)->SetVol(vol);
		gd3dApp->GetEngine()->GetAudioCore()->SetStreamVolume(vol);
	}));
	V.push_back(new Button(mSprite,mFont,m_LabelTex,MPGPL_Player8Health,false,"Player Eight Health",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
	}));
	V.push_back(new Slider(mSprite,m_SliderTex,m_buttonTex,MPGPS_Player8HealthSlider,true,0.0f,D3DXVECTOR3(16.0f, 32.0f, 0.0f),D3DXVECTOR3(128.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		float left = m_GuiSet->GetGuiElementById(MPGPS_Player8HealthSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MPGPS_Player8HealthSlider)->m_center.x;
		float right = m_GuiSet->GetGuiElementById(MPGPS_Player8HealthSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MPGPS_Player8HealthSlider)->m_center.x;
		float fDivisor = ((right-left)/100.0f);
		float vol = ((float)m_GuiSet->GetGuiElementById(MPGPS_Player8HealthSlider)->GetSliderPos().x-left)/fDivisor;
		vol = (int)gd3dApp->GetEngine()->GetMultiPlayerCore()->m_MultiPlayerLobby->GetMultiPlayerGameDataByID(gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID)->GetPlayerDataByNumber(8).health/100.0f;
		if(vol<0.05f)
		{
			vol=0.000f;
		}
		if (vol > 1.0f)
		{
			vol = 1.0f;
		}
		m_GuiSet->GetGuiElementById(MPGPS_Player8HealthSlider)->SetVol(vol);
		gd3dApp->GetEngine()->GetAudioCore()->SetStreamVolume(vol);
	}));
	return V;
}

void MultiplayerGamePlayMenu::ResetBkgrndScale()
{
	RECT rct;
	GetClientRect(gd3dApp->getMainWnd(),&rct);
	float sX=0.0f,sY=0.0f;
	sX=(((float)rct.right-(float)rct.left)/(float)2048);
	sY=(((float)rct.bottom-(float)rct.top)/(float)1024);
	m_bkGrndTexScale.x = sX;
	m_bkGrndTexScale.y = sY;
	m_bkGrndTexScale.z = 0.0f;
	sX = 2048.0f * sX;
	sY = 1024.0f * sY;
	sX=sX/2;
	sY=sY/2;
	m_bkGrndTexCentr.x=sX;
	m_bkGrndTexCentr.y=sY;
}
void MultiplayerGamePlayMenu::Enter()
{
	ResetBkgrndScale();
	MoveGuiElementsOffScreen();
	m_buttonClicked=false;
}
void MultiplayerGamePlayMenu::Exit()
{
}
void MultiplayerGamePlayMenu::OnLostDevice()
{
	HR(mSprite->OnLostDevice());
	HR(mFont->OnLostDevice());
}
void MultiplayerGamePlayMenu::OnResetDevice()
{
	HR(mSprite->OnResetDevice());
	HR(mFont->OnResetDevice());
	ResetBkgrndScale();
	MoveGuiElementsOffScreen();
}
void MultiplayerGamePlayMenu::Update(float dt)
{
	if (gd3dApp->GetEngine()->GetMultiPlayerCore()->GetOnlineStatus())
	{
		for(auto el : m_GuiSet->m_vGuiElements)
		{
			el->m_GuiEffect();
		}
		m_GuiSet->UpdateButtons(dt);
	}
	else
	{
		gd3dApp->SwitchGameState(MainGameMenuStateID);
	}
}
void MultiplayerGamePlayMenu::Render()
{
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHAREF, 10));	// alpha stuff needed 4 rendering text correctly
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER));	// and this stuff sets up the alpha
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true));
	// Clear the backbuffer and depth buffer.
	HR(gd3dDevice->BeginScene());
	HR(mSprite->Begin(D3DXSPRITE_ALPHABLEND));// begin drawing sprites
	//D3DXMATRIX S;
	//D3DXMatrixScaling(&S,m_bkGrndTexScale.x,m_bkGrndTexScale.y,1);
	//HR(mSprite->SetTransform(&S));
	//HR(mSprite->Draw(m_bkGrndTex,NULL, &m_bkGrndTexCentr,&m_bkGrndTexCentr, D3DCOLOR_XRGB(255, 255, 255)));
	//HR(mSprite->Flush());
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true));
	m_GuiSet->RenderGuiElements();
	HR(mSprite->Flush());
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false));
	HR(mSprite->End());// stop drawing sprites
	HR(gd3dDevice->EndScene());
	gd3dDevice->Present(0,0,0,0);
}