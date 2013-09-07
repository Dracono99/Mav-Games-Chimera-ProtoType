#include "MultiplayerGameOverMenu.h"
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
#include "MultiPlayerCore.h"
#include "MultiPlayerMenuElements.h"
#include "PacketStructs.h"

MultiplayerGameOverMenu::MultiplayerGameOverMenu()
{
	mStateID=MultiplayerGameOverStateID;
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

	//HR(D3DXCreateTextureFromFile(gd3dDevice, "MainMenuBackGround.jpg", &m_bkGrndTex));
	m_buttonTex=gd3dApp->GetEngine()->GetGraphicsCore()->RegisterTexture("TechButtonsSpriteSheet.bmp");
	m_bkGrndTex=gd3dApp->GetEngine()->GetGraphicsCore()->RegisterTexture("MechWarsMainBackGround.bmp");
	//HR(D3DXCreateTextureFromFile(gd3dDevice, "ExampleButtons512x512.png", &m_buttonTex));
	gd3dApp->GetEngine()->GetAudioCore()->LoadSound("BulletRicochetButtonClick.wav",m_ButtonClickEffect);

	ResetBkgrndScale();
	m_GuiSet = new GuiSet(m_bkGrndTexCentr,(int)m_bkGrndTexCentr.y/(int)3.0f);
	m_GuiSet->SetGuiElements(InitialiseGuiElements());
	m_GuiSet->MoveGuiSetTo(m_bkGrndTexCentr);
}
MultiplayerGameOverMenu::~MultiplayerGameOverMenu()
{
	if(m_GuiSet!=NULL)
	{
		delete m_GuiSet;
		m_GuiSet=NULL;
	}
}
void MultiplayerGameOverMenu::MoveGuiElementsOffScreen()
{
	RECT rct;
	GetClientRect(gd3dApp->getMainWnd(),&rct);
	m_GuiSet->MoveGuiSetTo(D3DXVECTOR3((float)rct.left-64.0f,(float)rct.bottom*0.5f,0.0f));
}

std::vector<BaseGuiElement*> MultiplayerGameOverMenu::InitialiseGuiElements()
{
	std::vector<BaseGuiElement*> V;
	RECT rct;
	GetClientRect(gd3dApp->getMainWnd(),&rct);
	V.push_back(new Button(mSprite,mFont,m_buttonTex,GPPMB_MainMenu,false,"Winner",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){

	}));
	V.push_back(new Button(mSprite,mFont,m_buttonTex,GPPMB_Options,true,"Rematch",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		gd3dApp->GetEngine()->GetAudioCore()->PlaySoundFile(m_ButtonClickEffect);
		m_GuiSet->GetGuiElementById(GPPMB_MainMenu)->mState=clicked;
		MoveGuiElementsOffScreen();
		gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(10,[&](){
			if(gd3dApp->GetEngine()->GetMultiPlayerCore()->GetOnlineStatus())
			{
				ClientPacket_JoinGame cqgp;
				cqgp.clientID=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_ClientID;
				cqgp.gameID=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_LastJoinedGameID;
				cqgp.packetID=(char)CP_JG;
				char buf[256];
				memcpy(buf,&cqgp,sizeof(ClientPacket_JoinGame));
				gd3dApp->GetEngine()->GetMultiPlayerCore()->SendPacket(buf);
				gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID=gd3dApp->GetEngine()->GetMultiPlayerCore()->m_LastJoinedGameID;
				gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(MultiplayerGameWaitingStateID);
			}
			else
			{
				gd3dApp->SwitchGameState(MainGameMenuStateID);
			}
		}));
	}));
	V.push_back(new Button(mSprite,mFont,m_buttonTex,GPPMB_Options,true,"Quit To Main Menu",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		gd3dApp->GetEngine()->GetAudioCore()->PlaySoundFile(m_ButtonClickEffect);
		m_GuiSet->GetGuiElementById(GPPMB_MainMenu)->mState=clicked;
		MoveGuiElementsOffScreen();
		gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(10,[&](){
			gd3dApp->SwitchGameState(MainGameMenuStateID);
		}));
	}));
	return V;
}

void MultiplayerGameOverMenu::ResetBkgrndScale()
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
void MultiplayerGameOverMenu::Enter()
{
	ResetBkgrndScale();
	m_GuiSet->MoveGuiSetTo(m_bkGrndTexCentr);
	m_buttonClicked=false;
	gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(5000,[&](){
		if (gd3dApp->GetEngine()->GetMultiPlayerCore()->GetOnlineStatus())
		{
			if (gd3dApp->GetEngine()->GetMultiPlayerCore()->m_JoinedGameID!=-1)
			{
				gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(MainMultiPlayerMenuStateID);
			}
		}
	}));
}
void MultiplayerGameOverMenu::Exit()
{
}
void MultiplayerGameOverMenu::OnLostDevice()
{
	HR(mSprite->OnLostDevice());
	HR(mFont->OnLostDevice());
}
void MultiplayerGameOverMenu::OnResetDevice()
{
	HR(mSprite->OnResetDevice());
	HR(mFont->OnResetDevice());
	ResetBkgrndScale();
	m_GuiSet->MoveGuiSetTo(m_bkGrndTexCentr);
}
void MultiplayerGameOverMenu::Update(float dt)
{
	if (gd3dApp->GetEngine()->GetMultiPlayerCore()->GetOnlineStatus())
	{
		m_GuiSet->UpdateButtons(dt);
		m_GuiSet->GetGuiElementById(GPPMB_MainMenu)->SetString(gd3dApp->GetEngine()->GetMultiPlayerCore()->mNameOfWinner);
		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadDownButtonPress)
		{
			m_XCurserButton++;
		}

		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadUpButtonPress)
		{
			m_XCurserButton--;
		}

		if (m_XCurserButton<0)
		{
			m_XCurserButton=(m_GuiSet->m_vGuiElements.size()-1);
		}
		else if ((unsigned)m_XCurserButton>=m_GuiSet->m_vGuiElements.size())
		{
			m_XCurserButton=0;
		}

		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_A_ButtonPress&&m_buttonClicked==false)
		{
			m_GuiSet->m_vGuiElements[m_XCurserButton]->m_GuiEffect();
			m_buttonClicked=true;
		}
		else
		{
			m_GuiSet->m_vGuiElements[m_XCurserButton]->mState=hovered;
		}
	}
	else
	{
		gd3dApp->SwitchGameState(MainGameMenuStateID);
	}
}
void MultiplayerGameOverMenu::Render()
{
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHAREF, 10));	// alpha stuff needed 4 rendering text correctly
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER));	// and this stuff sets up the alpha
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true));
	// Clear the backbuffer and depth buffer.
	HR(gd3dDevice->BeginScene());
	HR(mSprite->Begin(D3DXSPRITE_ALPHABLEND));// begin drawing sprites
	D3DXMATRIX S;
	D3DXMatrixScaling(&S,m_bkGrndTexScale.x,m_bkGrndTexScale.y,1);
	HR(mSprite->SetTransform(&S));
	HR(mSprite->Draw(m_bkGrndTex,NULL, &m_bkGrndTexCentr,&m_bkGrndTexCentr, D3DCOLOR_XRGB(255, 255, 255)));
	HR(mSprite->Flush());
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true));
	m_GuiSet->RenderGuiElements();
	HR(mSprite->Flush());
	//HR(gd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false));
	HR(mSprite->End());// stop drawing sprites
	HR(gd3dDevice->EndScene());
	gd3dDevice->Present(0,0,0,0);
}