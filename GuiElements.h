#include "BaseGUIelement.h"

class Button : public BaseGuiElement
{
private:
	ID3DXSprite* m_Sprite;
	ID3DXFont* m_Font;
	IDirect3DTexture9* m_buttonTex;
public:
	Button(ID3DXSprite* sprite,ID3DXFont* font,IDirect3DTexture9* tex,int id,bool clickable,std::string buttonCaption,D3DXVECTOR3 center, D3DXVECTOR3 StartPos, D3DXVECTOR3 DesPos, std::function<void ()> guiFX)
		:BaseGuiElement(id,clickable,center,StartPos,DesPos,guiFX)
	{
		m_Sprite=sprite;
		m_Font=font;
		m_buttonTex=tex;
		mbuttonCaption = buttonCaption;
	}
	~Button()
	{
		mbuttonCaption.clear();
	}
	std::string mbuttonCaption;
	RECT getRec()
	{
		RECT r;
		r.top=(LONG)m_pos.y-(LONG)m_center.y;
		r.bottom=(LONG)m_pos.y+(LONG)m_center.y;
		r.left=(LONG)m_pos.x-(LONG)m_center.x;
		r.right=(LONG)m_pos.x+(LONG)m_center.x;
		return r;
	}
	void RenderGuiElement()
	{
		D3DXMATRIX M,T;
		D3DXMatrixIdentity(&M);
		D3DXMatrixTranslation(&T, m_pos.x, m_pos.y, 0);
		HR(m_Sprite->SetTransform(&M));
		int s = mState;
		RECT Rct = {(LONG)(0+s*2.0f*m_center.x),0,(LONG)(2.0f*m_center.x+s*2.0f*m_center.x),(LONG)(2.0f*m_center.y)};
		//HR(gd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true));
		//gd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		//gd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		//RECT Rct = {0,0,256,64};
		HR(m_Sprite->Draw(m_buttonTex,&Rct,&m_center,&m_pos,0xFFFFFFFF));
		HR(m_Sprite->SetTransform(&T));
		RECT r = {0,0,0,0};
		HR(m_Font->DrawTextA(m_Sprite,mbuttonCaption.c_str(), -1, &r, DT_CENTER | DT_VCENTER | DT_CALCRECT,D3DCOLOR_RGBA(255,255,255,0)));
		HR(m_Font->DrawTextA(m_Sprite,mbuttonCaption.c_str(), -1, &r, DT_CENTER | DT_VCENTER,D3DCOLOR_RGBA(0,255,100,255)));
		//HR(gd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false));
	}
	void SetString(std::string str)
	{
		mbuttonCaption = str;
	}
};

class Slider : public BaseGuiElement
{
private:
	ID3DXSprite* m_Sprite;
	IDirect3DTexture9* m_sldrBkgTex;
	IDirect3DTexture9* m_SliderTex;
public:
	float m_val;
	D3DXVECTOR3 m_Spos;
	D3DXVECTOR3 m_Scenter;

	Slider(ID3DXSprite* sprite,IDirect3DTexture9* slideTex,IDirect3DTexture9* SldrBkTex,int id,bool clickable,float val,D3DXVECTOR3 Scenter,D3DXVECTOR3 center, D3DXVECTOR3 StartPos, D3DXVECTOR3 DesPos,std::function<void ()> guiFX)
		:BaseGuiElement(id,clickable,center,StartPos,DesPos,guiFX)
	{
		m_Sprite=sprite;
		m_sldrBkgTex=SldrBkTex;
		m_SliderTex=slideTex;
		m_Scenter=Scenter;
		m_Spos=D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_val = val;
	}

	void SetUpVal()
	{
		float left = m_pos.x-m_center.x*0.90f;
		float right = m_pos.x+m_center.x*0.90f;
		float offSet = right - left;
		m_Spos.x=left+offSet*m_val;
	}

	RECT getRec()
	{
		RECT r;
		r.top=(LONG)m_pos.y-(LONG)m_center.y;
		r.bottom=(LONG)m_pos.y+(LONG)m_center.y;
		r.left=(LONG)m_pos.x-(LONG)m_center.x;
		r.right=(LONG)m_pos.x+(LONG)m_center.x;
		return r;
	}

	void UpDate(double dt)
	{
		m_pos.z=0.0f;
		m_DesPos.z=0.0f;
		if(m_clickable)
		{
			POINT mouse;
			GetCursorPos(&mouse);
			ScreenToClient(gd3dApp->getMainWnd(),&mouse);
			if(OnButton((float)mouse.x,(float)mouse.y))
			{
				mState = hovered;
			}
			else
			{
				mState = neutral;
			}
		}
		D3DXVECTOR3 ToTarget = m_DesPos - m_pos;

		//calculate the distance to the target
		double dist = sqrt(pow(ToTarget.x,2)+pow(ToTarget.y,2));

		if (dist > 0)
		{
			//because Deceleration is enumerated as an int, this value is required
			//to provide fine tweaking of the deceleration..
			const double DecelerationTweaker = 0.3;

			//calculate the speed required to reach the target given the desired
			//deceleration
			double speed =  dist / ((double)normal * DecelerationTweaker);

			//make sure the velocity does not exceed the max
			speed = min(speed, 500.0);

			//from here proceed just like Seek except we don't need to normalize
			//the ToTarget vector because we have already gone to the trouble
			//of calculating its length: dist.
			D3DXVECTOR3 DesiredVelocity =  ToTarget * ((float)speed / (float)dist);

			m_pos+=(m_velocity*(float)dt);
			m_velocity+=(DesiredVelocity - m_velocity);
			m_pos.z=0.0f;
			m_DesPos.z=0.0f;
			m_Spos.z=0.0f;//expariment
			m_Spos.y=m_pos.y;
			SetUpVal();
		}
	}

	void RenderGuiElement()
	{
		D3DXMATRIX M;
		D3DXMatrixIdentity(&M);
		HR(m_Sprite->SetTransform(&M));
		HR(m_Sprite->Draw(m_sldrBkgTex,NULL,&m_center,&m_pos,D3DCOLOR_XRGB(255, 255, 255)));
		HR(m_Sprite->Flush());
		D3DXMatrixIdentity(&M);
		HR(m_Sprite->SetTransform(&M));
		HR(m_Sprite->Draw(m_SliderTex,NULL,&m_Scenter,&m_Spos,D3DCOLOR_XRGB(255, 255, 255)));
	}
	D3DXVECTOR3 GetSliderCenter(){return m_Scenter;}
	D3DXVECTOR3 GetSliderPos(){return m_Spos;}
	void SetSliderPos(float pos){m_Spos.x=pos;}
	float GetVol(){return m_val;}
	void SetVol(float val){m_val=val;}
};