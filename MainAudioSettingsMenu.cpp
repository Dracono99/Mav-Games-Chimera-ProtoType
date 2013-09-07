#include "MainAudioSettingsMenu.h"
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

MainAudioSettingsMenu::MainAudioSettingsMenu()
{
	mStateID=MainAudioSettingsMenuState;
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

	ResetBkgrndScale();
	m_GuiSet = new GuiSet(m_bkGrndTexCentr,(int)m_bkGrndTexCentr.y/(int)4.0f);
	m_GuiSet->SetGuiElements(InitialiseGuiElements());
	m_GuiSet->MoveGuiSetTo(m_bkGrndTexCentr);
}
MainAudioSettingsMenu::~MainAudioSettingsMenu()
{
	if(m_GuiSet!=NULL)
	{
		delete m_GuiSet;
		m_GuiSet=NULL;
	}
}
void MainAudioSettingsMenu::MoveGuiElementsOffScreen()
{
	RECT rct;
	GetClientRect(gd3dApp->getMainWnd(),&rct);
	m_GuiSet->MoveGuiSetTo(D3DXVECTOR3((float)rct.left-64.0f,(float)rct.bottom*0.5f,0.0f));
}

std::vector<BaseGuiElement*> MainAudioSettingsMenu::InitialiseGuiElements()
{
	std::vector<BaseGuiElement*> V;
	RECT rct;
	GetClientRect(gd3dApp->getMainWnd(),&rct);
	V.push_back(new Button(mSprite,mFont,m_LabelTex,MASML_MusicVolume,false,"MusicVolume",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
	}));
	V.push_back(new Slider(mSprite,m_SliderTex,m_buttonTex,MASMS_MusicVolumeSlider,true,1.0f,D3DXVECTOR3(16.0f, 32.0f, 0.0f),D3DXVECTOR3(128.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadRightButtonPress)
		{
			float left = m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_center.x;
			float right = m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_center.x;
			float fDivisor = ((right-left)/100.0f);
			float vol = ((float)m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->GetSliderPos().x-left)/fDivisor;
			vol = vol/100.0f;
			vol+=0.1f;
			if(vol<0.05f)
			{
				vol=0.000f;
			}
			if (vol > 1.0f)
			{
				vol = 1.0f;
			}
			m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->SetVol(vol);
			gd3dApp->GetEngine()->GetAudioCore()->SetStreamVolume(vol);
		}
		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadLeftButtonPress)
		{
			float left = m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_center.x;
			float right = m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_center.x;
			float fDivisor = ((right-left)/100.0f);
			float vol = ((float)m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->GetSliderPos().x-left)/fDivisor;
			vol = vol/100.0f;
			vol-=0.1f;
			if(vol<0.05f)
			{
				vol=0.000f;
			}
			if (vol > 1.0f)
			{
				vol = 1.0f;
			}
			m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->SetVol(vol);
			gd3dApp->GetEngine()->GetAudioCore()->SetStreamVolume(vol);
		}
	}));
	V.push_back(new Button(mSprite,mFont,m_LabelTex,MASML_SoundEffects,false,"EffectsVolume",D3DXVECTOR3(170.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
	}));
	V.push_back(new Slider(mSprite,m_SliderTex,m_buttonTex,MASMS_SoundEffectsSlider,true,1.0f,D3DXVECTOR3(16.0f, 32.0f, 0.0f),D3DXVECTOR3(128.0f, 32.0f, 0.0f),D3DXVECTOR3((float)rct.right+64.0f,(float)rct.bottom*0.5f,0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),[&](){
		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadRightButtonPress)
		{
			float left = m_GuiSet->GetGuiElementById(MASMS_SoundEffectsSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_center.x*0.90f;
			float right = m_GuiSet->GetGuiElementById(MASMS_SoundEffectsSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_center.x*0.90f;
			float fDivisor = ((right-left)/100.0f);
			float vol = ((float)m_GuiSet->GetGuiElementById(MASMS_SoundEffectsSlider)->GetSliderPos().x-left)/fDivisor;
			vol = vol/100.0f;
			vol+=0.1f;
			if(vol<0.05f)
			{
				vol=0.000f;
			}
			if (vol > 1.0f)
			{
				vol = 1.0f;
			}
			m_GuiSet->GetGuiElementById(MASMS_SoundEffectsSlider)->SetVol(vol);
			gd3dApp->GetEngine()->GetAudioCore()->SetSoundVolume(vol);
		}
		if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadLeftButtonPress)
		{
			float left = m_GuiSet->GetGuiElementById(MASMS_SoundEffectsSlider)->m_pos.x-m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_center.x*0.90f;
			float right = m_GuiSet->GetGuiElementById(MASMS_SoundEffectsSlider)->m_pos.x+m_GuiSet->GetGuiElementById(MASMS_MusicVolumeSlider)->m_center.x*0.90f;
			float fDivisor = ((right-left)/100.0f);
			float vol = ((float)m_GuiSet->GetGuiElementById(MASMS_SoundEffectsSlider)->GetSliderPos().x-left)/fDivisor;
			vol = vol/100.0f;
			vol-=0.1f;
			if(vol<0.05f)
			{
				vol=0.000f;
			}
			if (vol > 1.0f)
			{
				vol = 1.0f;
			}
			m_GuiSet->GetGuiElementById(MASMS_SoundEffectsSlider)->SetVol(vol);
			gd3dApp->GetEngine()->GetAudioCore()->SetSoundVolume(vol);
		}
	}));
	return V;
}

void MainAudioSettingsMenu::ResetBkgrndScale()
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
void MainAudioSettingsMenu::Enter()
{
	ResetBkgrndScale();
	m_GuiSet->MoveGuiSetTo(m_bkGrndTexCentr);
	m_buttonClicked=false;
}
void MainAudioSettingsMenu::Exit()
{
}
void MainAudioSettingsMenu::OnLostDevice()
{
	HR(mSprite->OnLostDevice());
	HR(mFont->OnLostDevice());
}
void MainAudioSettingsMenu::OnResetDevice()
{
	HR(mSprite->OnResetDevice());
	HR(mFont->OnResetDevice());
	ResetBkgrndScale();
	m_GuiSet->MoveGuiSetTo(m_bkGrndTexCentr);
}
void MainAudioSettingsMenu::Update(float dt)
{
	m_GuiSet->UpdateButtons(dt);
	if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadDownButtonPress)
	{
		m_XCurserButton++;
		if (m_XCurserButton<0)
		{
			m_XCurserButton=(m_GuiSet->m_vGuiElements.size()-1);
		}
		else if ((unsigned)m_XCurserButton>=m_GuiSet->m_vGuiElements.size())
		{
			m_XCurserButton=0;
		}
		if (!m_GuiSet->GetButtonAtIndex(m_XCurserButton)->m_clickable)
		{
			m_XCurserButton++;
		}
	}

	if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadUpButtonPress)
	{
		m_XCurserButton--;
		if (m_XCurserButton<0)
		{
			m_XCurserButton=(m_GuiSet->m_vGuiElements.size()-1);
		}
		else if ((unsigned)m_XCurserButton>=m_GuiSet->m_vGuiElements.size())
		{
			m_XCurserButton=0;
		}
		if (!m_GuiSet->GetButtonAtIndex(m_XCurserButton)->m_clickable)
		{
			m_XCurserButton--;
		}
	}

	if (m_XCurserButton<0)
	{
		m_XCurserButton=(m_GuiSet->m_vGuiElements.size()-1);
	}
	else if ((unsigned)m_XCurserButton>=m_GuiSet->m_vGuiElements.size())
	{
		m_XCurserButton=0;
	}

	if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadRightButtonPress||gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_DpadLeftButtonPress)
	{
		m_GuiSet->m_vGuiElements[m_XCurserButton]->m_GuiEffect();
	}

	if (gd3dApp->GetEngine()->GetInputCore()->GetXinput()->m_B_ButtonPress&&m_buttonClicked==false)
	{
		gd3dApp->GetEngine()->GetAudioCore()->PlaySoundFile(m_ButtonClickEffect);
		MoveGuiElementsOffScreen();
		gd3dApp->GetEngine()->GetMessageSystem()->AddMessage(EngineMessage(1000,[&](){
			gd3dApp->GetEngine()->GetMenuSystem()->SwitchMenuState(MainOptionsMenuState);
		}));
		m_buttonClicked=true;
	}
}
void MainAudioSettingsMenu::Render()
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