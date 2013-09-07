#include "HitEffectGamePlayObject.h"
#include "GraphicsCore.h"
#include "MainGameManager.h"
#include "ChimeraGameEngine.h"

HitEffectGamePlayObject::HitEffectGamePlayObject()
{
}
HitEffectGamePlayObject::HitEffectGamePlayObject(HitEffectSchematic schem)
{
	m_ProjDiffuseTex=gd3dApp->GetEngine()->GetGraphicsCore()->RegisterTexture(schem.mDiffuseDecalTexFileName);
	m_ProjNormalTex=gd3dApp->GetEngine()->GetGraphicsCore()->RegisterTexture(schem.mNormalDecalTexFileName);
	m_ProjSpecTex=gd3dApp->GetEngine()->GetGraphicsCore()->RegisterTexture(schem.mSpecDecalTexFileName);
	D3DXMatrixPerspectiveLH(&m_ProjectionMtrx,schem.mTexWidth,schem.mTexHeight,0.0f,500.0f);
	D3DXMatrixPerspectiveFovLH(&m_ProjectionMtrx,
		D3DX_PI * 0.25f,    // the horizontal field of view
		(schem.mTexWidth/ schem.mTexHeight), // aspect ratio
		1.0f,    // the near view-plane
		55.0f);    // the far view-plane
}
HitEffectGamePlayObject::~HitEffectGamePlayObject()
{
}
D3DXMATRIX HitEffectGamePlayObject::GetViewProjectionMatrix(BaseGameEntity* entity)
{
	D3DXVECTOR3 worldUp = entity->GetPhysicsPointer()->getPointInWorldSpace(m_HelperUpPosition);
	worldUp=worldUp-entity->GetPhysicsPointer()->getPointInWorldSpace(m_ProjectorPosition);
	D3DXVec3Normalize(&worldUp,&worldUp);
	D3DXMATRIX dvp;
	D3DXMatrixLookAtLH(&dvp,&entity->GetPhysicsPointer()->getPointInWorldSpace(m_ProjectorPosition),&entity->GetPhysicsPointer()->getPointInWorldSpace(m_DecalPosition),&worldUp);
	return dvp;
}