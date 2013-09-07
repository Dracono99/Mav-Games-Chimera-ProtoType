#include "GamePlayCore.h"
#include "DragonPhysicsEngine.h"
#include "HitEffectGamePlayObject.h"
#include "ChimeraGameEngine.h"
#include "MainGameManager.h"

GamePlayCore::GamePlayCore()
{
}
GamePlayCore::~GamePlayCore()
{
}
void GamePlayCore::ApplyGamePlayEffects(std::vector<BaseGameEntity*>& entities,std::vector<DragonicPair*>& contactPairs)
{
	for(const auto DragonicPair : contactPairs)
	{
		if(DragonicPair->obj1->GetGamePlayObject()->m_HitEffectApplicaple)
		{
			HitEffectSchematic schem;
			schem.mDiffuseDecalTexFileName="BulletHoleDecalAlpa.bmp";
			schem.mNormalDecalTexFileName="BulletHoleDecalNorm.bmp";
			schem.mSpecDecalTexFileName="BulletHoleDecalSpec.bmp";
			schem.mTexHeight=1024.0f;
			schem.mTexWidth=1024.0f;
			for(auto contacts : gd3dApp->GetEngine()->GetPhysicsCore()->GetContactsByPairOfEntities(DragonicPair->obj1,DragonicPair->obj2))
			{
				HitEffectGamePlayObject* fx = new HitEffectGamePlayObject(schem);
				fx->m_DecalPosition= DragonicPair->obj1->GetPhysicsPointer()->getPointInLocalSpace(contacts->m_vContactPoint);
				fx->m_ProjectorPosition=fx->m_DecalPosition+(DragonicPair->obj1->GetPhysicsPointer()->getDirectionInLocalSpace(contacts->m_vContactNormal)*-50.0f);
				D3DXVECTOR3 up;
				D3DXVec3Cross(&up,&fx->m_DecalPosition,&fx->m_ProjectorPosition);
				D3DXVec3Normalize(&up,&up);
				fx->m_HelperUpPosition=fx->m_ProjectorPosition+(up*3.0f);
				DragonicPair->obj1->GetGamePlayObject()->ApplyHitEffect(fx);
			}
		}
		if(DragonicPair->obj2->GetGamePlayObject()->m_HitEffectApplicaple)
		{
			HitEffectSchematic schem;
			schem.mDiffuseDecalTexFileName="BulletHoleDecalAlpa.bmp";
			schem.mNormalDecalTexFileName="BulletHoleDecalNorm.bmp";
			schem.mSpecDecalTexFileName="BulletHoleDecalSpec.bmp";
			schem.mTexHeight=1024.0f;
			schem.mTexWidth=1024.0f;
			for(auto contacts : gd3dApp->GetEngine()->GetPhysicsCore()->GetContactsByPairOfEntities(DragonicPair->obj2,DragonicPair->obj1))
			{
				HitEffectGamePlayObject* fx = new HitEffectGamePlayObject(schem);
				fx->m_DecalPosition= DragonicPair->obj2->GetPhysicsPointer()->getPointInLocalSpace(contacts->m_vContactPoint);
				fx->m_ProjectorPosition=fx->m_DecalPosition+(DragonicPair->obj2->GetPhysicsPointer()->getDirectionInLocalSpace(contacts->m_vContactNormal)*-50.0f);
				D3DXVECTOR3 up;
				D3DXVec3Cross(&up,&fx->m_DecalPosition,&fx->m_ProjectorPosition);
				D3DXVec3Normalize(&up,&up);
				fx->m_HelperUpPosition=fx->m_ProjectorPosition+(up*3.0f);
				DragonicPair->obj1->GetGamePlayObject()->ApplyHitEffect(fx);
			}
			/*	fx->m_DecalPosition= DragonicPair->obj2->GetPhysicsPointer()->getPointInLocalSpace(gd3dApp->GetEngine()->GetPhysicsCore()->GetContactsByPairOfEntities(DragonicPair->obj1,DragonicPair->obj2)[0]->m_vContactPoint);
			fx->m_ProjectorPosition=fx->m_DecalPosition+(DragonicPair->obj2->GetPhysicsPointer()->getDirectionInLocalSpace(gd3dApp->GetEngine()->GetPhysicsCore()->GetContactsByPairOfEntities(DragonicPair->obj1,DragonicPair->obj2)[0]->m_vContactNormal)*-10.0f);
			D3DXVECTOR3 up;
			D3DXVec3Cross(&up,&fx->m_DecalPosition,&fx->m_ProjectorPosition);
			D3DXVec3Normalize(&up,&up);
			fx->m_HelperUpPosition=fx->m_ProjectorPosition+(up*3.0f);
			DragonicPair->obj2->GetGamePlayObject()->ApplyHitEffect(fx);*/
		}
	}
}