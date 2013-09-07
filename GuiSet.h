#pragma once
#include "GuiElements.h"
#include <vector>

class GuiSet
{
private:
	D3DXVECTOR3 m_Pos;
	int m_offSet;
public:
	std::vector<BaseGuiElement*> m_vGuiElements;
public:
	GuiSet(D3DVECTOR Pos, int offSet=64)
	{
		m_Pos = Pos;
		m_offSet=offSet;
	}
	~GuiSet()
	{
		for(auto guielements : m_vGuiElements)
		{
			delete guielements;
		}
		m_vGuiElements.clear();
	}
	void SetGuiElements(std::vector<BaseGuiElement*> v){m_vGuiElements=v;}
	void SetOffSet(int offSet){m_offSet=offSet;}
	BaseGuiElement* GetButtonAtIndex(int i){ return m_vGuiElements[i];}
	void SetPos(D3DXVECTOR3 p){m_Pos=p;}
	D3DXVECTOR3 GetPos(){return m_Pos;}
	void UpdateButtons(double dt)
	{
		for(int i = 0; (unsigned)i < m_vGuiElements.size();i++)
		{
			m_vGuiElements[i]->UpDate(dt);
		}
	}
	void RenderGuiElements()
	{
		for(auto element : m_vGuiElements)
		{
			element->RenderGuiElement();
		}
	}
	void OnLeftClick()
	{
		for(int i = 0; (unsigned)i < m_vGuiElements.size();i++)
		{
			m_vGuiElements[i]->onClick();
		}
	}
	void UpdateDesPos()
	{
		for(int i = 0;(unsigned) i < m_vGuiElements.size();i++)
		{
			D3DXVECTOR3 tempV = m_Pos;
			tempV.y=tempV.y+i*m_offSet;
			m_vGuiElements[i]->m_DesPos = tempV;
		}
	}
	BaseGuiElement* GetGuiElementById(int id)
	{
		for(int i = 0; (unsigned)i < m_vGuiElements.size();i++)
		{
			if(m_vGuiElements[i]->ID==id)
			{
				return m_vGuiElements[i];
			}
		}
		return NULL;
	}
	void MoveGuiSetTo(D3DXVECTOR3 p)
	{
		SetPos(p);
		UpdateDesPos();
	}
	bool ReadyToSwitch()
	{
		for(int i = 0; (unsigned)i < m_vGuiElements.size();i++)
		{
			if(m_vGuiElements[i]->m_pos.x<0.0f)
			{
				return true;
			}
		}
		return false;
	}
	void ResetStates()
	{
		for(int i = 0; (unsigned)i < m_vGuiElements.size();i++)
		{
			m_vGuiElements[i]->ResetStates();
		}
	}
};