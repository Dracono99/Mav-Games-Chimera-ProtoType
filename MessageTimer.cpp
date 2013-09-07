#include "MessageTimer.h"

MessageTimer* MessageTimer::Instance()
{
	static MessageTimer instance;
	return &instance;
}