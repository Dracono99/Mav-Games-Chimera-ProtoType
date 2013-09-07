#pragma once
#include "DragonXMath.h"

class BaseGameEntity;
struct HitEffectSchematic
{
	std::string mDiffuseDecalTexFileName;
	std::string mNormalDecalTexFileName;
	std::string mSpecDecalTexFileName;
	float mTexWidth;
	float mTexHeight;
};

class HitEffectGamePlayObject
{
private:

public:
	HitEffectGamePlayObject();
	HitEffectGamePlayObject(HitEffectSchematic schem);
	~HitEffectGamePlayObject();
	IDirect3DTexture9* m_ProjDiffuseTex;
	IDirect3DTexture9* m_ProjNormalTex;
	IDirect3DTexture9* m_ProjSpecTex;
	float m_HitFXtexWidth;
	float m_HitFXtexHeight;
	DragonXVector3 m_DecalPosition;
	DragonXVector3 m_HelperUpPosition;
	DragonXVector3 m_ProjectorPosition;
	D3DXMATRIX m_ProjectionMtrx;
	D3DXMATRIX GetViewProjectionMatrix(BaseGameEntity* entity);
	void SetUpHitEffectParameters(D3DXVECTOR3 decalPosInLocal,D3DXVECTOR3 projPosInLocal,D3DXVECTOR3 helperPosInLocal)
	{
		m_DecalPosition=decalPosInLocal;
		m_HelperUpPosition=helperPosInLocal;
		m_ProjectorPosition=projPosInLocal;
	}
};