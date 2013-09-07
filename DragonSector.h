#pragma once
#include "PhysicsTriangle.h"
#include <vector>

class DragonSector
{
private:
	DragonXVector3 m_Position;
	real m_Radius;
	std::vector<DragonTriangle*> m_DragonTris;
public:
	DragonSector(DragonXVector3& pos, real rad, std::vector<DragonTriangle*>& v)
	{
		m_Position=pos;
		m_Radius=rad;
		m_DragonTris=v;
	}
	~DragonSector()
	{
		/*for (auto DragonTriangle : m_DragonTris)
		{
		if(DragonTriangle!=nullptr)
		{
		delete DragonTriangle;
		DragonTriangle=nullptr;
		}
		}*/
		m_DragonTris.clear();
	}
	inline DragonXVector3 GetPosition()
	{
		return m_Position;
	}
	inline real GetRadius()
	{
		return m_Radius;
	}
	inline std::vector<DragonTriangle*> GetDragonTris()
	{
		return m_DragonTris;
	}
};
