#pragma once
#include "PhysicsOBB.h"
#include "DragonEnums.h"

class DragonMeshCollisionObject : public PhysicsOBB
{
private:

public:
	std::vector<DragonSector*> m_DragonSectors;
	std::vector<DragonTriangle*> m_DragonTris;
	DragonMeshCollisionObject():PhysicsOBB()
	{
		mI_PrimitiveType=PT_Mesh;
	}
	~DragonMeshCollisionObject()
	{
		for (auto DragonSector : m_DragonSectors)
		{
			/*if(DragonSector!=nullptr)
			{*/
			delete DragonSector;
			//DragonSector=nullptr;
			//}
		}
		m_DragonSectors.clear();

		for (auto DragonTriangle : m_DragonTris)
		{
			delete DragonTriangle;
		}
		m_DragonTris.clear();
	}
	std::vector<DragonSector*> GetSectors()
	{
		return m_DragonSectors;
	}
	inline void CalcInverseInertialTensor()
	{
		DragonXMatrix m = DragonXMatrix();
		D3DXMatrixIdentity(&m);
		m._11=(real)0.2f*m_Mass*((GetHalfSizes().y*GetHalfSizes().y)+(GetHalfSizes().z*GetHalfSizes().z));
		m._22=(real)0.2f*m_Mass*((GetHalfSizes().x*GetHalfSizes().x)+(GetHalfSizes().z*GetHalfSizes().z));
		m._33=(real)0.2f*m_Mass*((GetHalfSizes().x*GetHalfSizes().x)+(GetHalfSizes().y*GetHalfSizes().y));
		D3DXMatrixInverse(&m,NULL,&m);
		inverseInertiaTensor= m;
	}
	virtual void SetMassAndInverseTensor(real newMass)
	{
		m_Mass=newMass;
		m_InverseMass=1.0f/newMass;
		CalcInverseInertialTensor();
	}
};
