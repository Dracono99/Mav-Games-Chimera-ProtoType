#pragma once

#pragma comment(lib, "winmm.lib")

#include <windows.h>

#define MsgTimer MessageTimer::Instance()

class MessageTimer
{
private:
	double m_dStartTime;

	MessageTimer(){m_dStartTime = timeGetTime();}

	MessageTimer(const MessageTimer&);
	MessageTimer& operator=(const MessageTimer&);
public:
	static MessageTimer* Instance();

	double getCurrentTime(){return timeGetTime() - m_dStartTime;}
};