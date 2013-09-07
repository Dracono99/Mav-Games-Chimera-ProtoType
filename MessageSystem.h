#pragma once
#include <functional>
#include <windows.h>
#include <queue>

struct EngineMessage
{
	double mDelay;
	std::function<void ()> mMessage;
	EngineMessage(double del,std::function<void ()> msg)
	{
		mDelay=del;
		mMessage=msg;
	}
};

struct Comparator
{
	bool operator()(const EngineMessage& lhs,const EngineMessage& rhs)
	{
		return lhs.mDelay>rhs.mDelay;
	}
};

class EngineMessageSystem
{
private:
	CRITICAL_SECTION m_msgCS;
	std::priority_queue<EngineMessage,std::vector<EngineMessage>,Comparator> m_MessageQueue;
public:
	EngineMessageSystem();
	~EngineMessageSystem();
	void AddMessage(EngineMessage msg);
	void DispatchMessages();
};