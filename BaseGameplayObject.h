#pragma once
#include <vector>

class BaseGameEntity;
class HitEffectGamePlayObject;
class BaseMpOnlineAIObject;

class BaseGameplayObject
{
private:

public:
	BaseGameplayObject();
	~BaseGameplayObject();
	int m_ObjectID;
	int m_ObjectTypeID;
	bool m_HitEffectApplied;
	bool m_HitEffectApplicaple;
	BaseGameEntity* m_Owner;
	//std::vector<HitEffectGamePlayObject*> m_HitFX;
	HitEffectGamePlayObject* m_HitFX;
	BaseMpOnlineAIObject* m_mpOlineEntityAI;
	void ApplyHitEffect(HitEffectGamePlayObject* fx);
	void HitEffectApplied();
};