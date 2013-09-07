#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include "MainGameManager.h"
//#include "ButtonEnums.h"
#include <functional>
#include <Windows.h>
//member function pointers and original templete class credit to the amazing Anthony Hernandez
enum Deceleration{slow = 3, normal = 2, fast = 1};
enum Button_State
{
	neutral,
	hovered,
	clicked,
};

class BaseGuiElement
{
public:
	std::function<void ()> m_GuiEffect;
	bool m_clickable;
	D3DXVECTOR3 m_center;
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_DesPos;
	D3DXVECTOR3 m_velocity;
	int ID, mState;

	BaseGuiElement(int id,bool clickable,D3DXVECTOR3 center, D3DXVECTOR3 StartPos, D3DXVECTOR3 DesPos, std::function<void ()> guiFX)
	{
		ID=id;
		m_clickable=clickable;
		this->m_GuiEffect = guiFX;
		m_center=center;
		m_pos=StartPos;
		m_DesPos=DesPos;
		mState = neutral;
		m_velocity=D3DXVECTOR3(0.0f,0.0f,0.0f);
	}
	virtual ~BaseGuiElement()
	{
	}

	bool OnButton(float mouseX,float mouseY)
	{
		if((m_pos.x-m_center.x)>mouseX)//test left
		{
			return false;
		}
		else if((m_pos.x+m_center.x)<mouseX)//test right
		{
			return false;
		}
		else if((m_pos.y+m_center.y)<mouseY)// test bottom
		{
			return false;
		}
		else if((m_pos.y-m_center.y)>mouseY)// test top
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	void onClick()
	{
		if(m_clickable)
		{
			POINT mouse;
			GetCursorPos(&mouse);
			ScreenToClient(gd3dApp->getMainWnd(),&mouse);
			if(OnButton((float)mouse.x,(float)mouse.y))
			{
				this->m_GuiEffect();
			}
		}
	}

	void ResetStates()
	{
		mState = neutral;
	}

	virtual void UpDate(double dt)
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
				if(mState!=clicked)
				{
					mState = hovered;
				}
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
		}
	}
	virtual void RenderGuiElement()=0;
	virtual D3DXVECTOR3 GetSliderCenter(){return m_velocity;}
	virtual D3DXVECTOR3 GetSliderPos(){return m_velocity;}
	virtual void SetSliderPos(float pos){}
	virtual float GetVol(){return m_pos.z;}
	virtual void SetVol(float val){}
	virtual void SetString(std::string str){}
};
