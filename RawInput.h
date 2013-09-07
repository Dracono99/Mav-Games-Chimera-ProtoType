#pragma once
#include <Windows.h>
#define RII RawInput::instance()
class RawInput
{
	RawInput();
	LPBYTE lpb;
public:
	~RawInput();
	static RawInput* instance();
	void registerDevices(PCRAWINPUTDEVICE pRawInputDevices,UINT uiNumDevices);
	RAWINPUT* buffer(LPARAM lParam);
	void GetRawInput(LPARAM lParam);
};