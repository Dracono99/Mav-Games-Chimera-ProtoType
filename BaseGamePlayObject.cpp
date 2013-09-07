#include "BaseGameplayObject.h"
#include "HitEffectGamePlayObject.h"
#include "BaseMpOnlineAIObject.h"

BaseGameplayObject::BaseGameplayObject()
{
	m_HitFX=NULL;
	m_mpOlineEntityAI=NULL;
	m_HitEffectApplied=false;
}
BaseGameplayObject::~BaseGameplayObject()
{
	if(m_HitFX!=NULL)
	{
		delete m_HitFX;
	}
	/*for (auto hitFX : m_HitFX)
	{
	delete hitFX;
	}*/

	if (m_mpOlineEntityAI!=NULL)
	{
		delete m_mpOlineEntityAI;
		m_mpOlineEntityAI=NULL;
	}
}
void BaseGameplayObject::ApplyHitEffect(HitEffectGamePlayObject* fx)
{
	if (m_HitFX!=NULL)
	{
		delete m_HitFX;
		m_HitFX=NULL;
		m_HitEffectApplied=false;
	}
	m_HitFX=fx;
	/*if (m_HitFX.size()>10)
	{
	for(auto fx : m_HitFX)
	{
	delete fx;
	fx=NULL;
	}
	m_HitFX.clear();
	}*/
	//m_HitFX.push_back(fx);
	m_HitEffectApplied=true;
}
void BaseGameplayObject::HitEffectApplied()
{
	if (m_HitFX!=NULL)
	{
		delete m_HitFX;
		m_HitFX=NULL;
		m_HitEffectApplied=false;
	}
}