#pragma once
#include "GuiSet.h"

class MultiplayerGameOverMenu : public state
{
private:
	ID3DXSprite* mSprite;
	ID3DXFont* mFont;
	IDirect3DTexture9* m_bkGrndTex;
	IDirect3DTexture9* m_buttonTex;
	D3DXVECTOR3 m_bkGrndTexCentr;
	D3DXVECTOR3 m_bkGrndTexScale;
	void MoveGuiElementsOffScreen();
	GuiSet* m_GuiSet;
	int m_XCurserButton;
	int m_ButtonClickEffect;
	bool m_buttonClicked;
	std::vector<BaseGuiElement*> InitialiseGuiElements();
	void ResetBkgrndScale();
public:
	MultiplayerGameOverMenu();
	~MultiplayerGameOverMenu();
	void Enter();
	void Exit();
	void OnLostDevice();
	void OnResetDevice();
	void Update(float dt);
	void Render();
};