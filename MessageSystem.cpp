#include "MessageSystem.h"
#include "MessageTimer.h"

EngineMessageSystem::EngineMessageSystem()
{
	InitializeCriticalSection(&m_msgCS);
}
EngineMessageSystem::~EngineMessageSystem()
{
	DeleteCriticalSection(&m_msgCS);
}
void EngineMessageSystem::AddMessage(EngineMessage msg)
{
	if(msg.mDelay<=0.0000)
	{
		msg.mMessage();
	}
	else
	{
		msg.mDelay+=MsgTimer->getCurrentTime();
		EnterCriticalSection(&m_msgCS);
		m_MessageQueue.push(msg);
		LeaveCriticalSection(&m_msgCS);
	}
}
void EngineMessageSystem::DispatchMessages()
{
	double curTime = MsgTimer->getCurrentTime();
	EnterCriticalSection(&m_msgCS);
	while (!m_MessageQueue.empty()&&(m_MessageQueue.top().mDelay<=curTime)&&(m_MessageQueue.top().mDelay>0))
	{
		m_MessageQueue.top().mMessage();
		if (!m_MessageQueue.empty())
		{
			m_MessageQueue.pop();
		}
	}
	LeaveCriticalSection(&m_msgCS);
}