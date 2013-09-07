#pragma once

#include "DragonXMath.h"

class DragonTriangle
{
private:
	real m_MaxX;
	real m_MinX;
	real m_MaxY;
	real m_MinY;
	real m_MaxZ;
	real m_MinZ;
	DragonXVector3 m_VertA;
	DragonXVector3 m_VertB;
	DragonXVector3 m_VertC;
	DragonXVector3 m_MidPoint;
	DragonXVector3 m_Normal;
private:
	inline void SetUp()
	{
		// set up max x
		m_MaxX=m_VertA.x;
		if(m_VertB.x>m_MaxX)
		{
			m_MaxX=m_VertB.x;
		}
		if (m_VertC.x>m_MaxX)
		{
			m_MaxX=m_VertC.x;
		}
		// set up min x
		m_MinX=m_VertA.x;
		if(m_VertB.x<m_MinX)
		{
			m_MinX=m_VertB.x;
		}
		if (m_VertC.x<m_MinX)
		{
			m_MinX=m_VertC.x;
		}
		// set up max Y
		m_MaxY=m_VertA.y;
		if(m_VertB.y>m_MaxY)
		{
			m_MaxY=m_VertB.y;
		}
		if (m_VertC.y>m_MaxY)
		{
			m_MaxY=m_VertC.y;
		}
		// set up min y
		m_MinY=m_VertA.y;
		if(m_VertB.y<m_MinY)
		{
			m_MinY=m_VertB.y;
		}
		if (m_VertC.y<m_MinY)
		{
			m_MinY=m_VertC.y;
		}
		// set up max Z
		m_MaxZ=m_VertA.z;
		if(m_VertB.z>m_MaxZ)
		{
			m_MaxZ=m_VertB.z;
		}
		if (m_VertC.z>m_MaxZ)
		{
			m_MaxZ=m_VertC.z;
		}
		// set up min z
		m_MinZ=m_VertA.z;
		if(m_VertB.z<m_MinZ)
		{
			m_MinZ=m_VertB.z;
		}
		if (m_VertC.z<m_MinZ)
		{
			m_MinZ=m_VertC.z;
		}
		real mx = m_VertA.x+m_VertB.x+m_VertC.x;
		mx/=3.0f;
		real my = m_VertA.y+m_VertB.y+m_VertC.y;
		my/=3.0f;
		real mz = m_VertA.z+m_VertB.z+m_VertC.z;
		mz/=3.0f;
		m_MidPoint=DragonXVector3(mx,my,mz);
		m_Normal=((m_VertB-m_VertA)%(m_VertC-m_VertA));
		m_Normal.invert();
		m_Normal.Normalize();
	}
public:
	DragonTriangle(DragonXVector3& a,DragonXVector3& b,DragonXVector3& c)
	{
		m_VertA=a;
		m_VertB=b;
		m_VertC=c;
		SetUp();
	}
	~DragonTriangle()
	{
	}
	inline real GetMaxX()
	{
		return m_MaxX;
	}
	inline real GetMinX()
	{
		return m_MinX;
	}
	inline real GetMaxY()
	{
		return m_MaxY;
	}
	inline real GetMinY()
	{
		return m_MinY;
	}
	inline real GetMaxZ()
	{
		return m_MaxZ;
	}
	inline real GetMinZ()
	{
		return m_MinZ;
	}
	inline DragonXVector3 GetNormal()
	{
		return m_Normal;
	}
	inline DragonXVector3 GetMidPoint()
	{
		return m_MidPoint;
	}
	inline DragonXVector3 GetVertA()
	{
		return m_VertA;
	}
	inline DragonXVector3 GetVertB()
	{
		return m_VertB;
	}
	inline DragonXVector3 GetVertC()
	{
		return m_VertC;
	}
	inline DragonXVector3 GetNormal(DragonXVector3& vA,DragonXVector3& vB,DragonXVector3& vC)
	{
		DragonXVector3 n = ((vB-vA)%(vC-vA));
		n.invert();
		n.Normalize();
		return n;
	}
};