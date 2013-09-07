#pragma once
// No MFC
#define WIN32_LEAN_AND_MEAN

// We need the Windows Header and the XInput Header
#include <windows.h>
#include <XInput.h>
#undef XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  3000
#undef XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 3000
#undef XINPUT_GAMEPAD_TRIGGER_THRESHOLD
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    10
// Now, the XInput Library
// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING
// A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES
#pragma comment(lib, "XInput.lib")

// XBOX Controller Class Definition
class CXBOXController
{
private:
	XINPUT_STATE _controllerState;
	int _controllerNum;
public:
	CXBOXController(int playerNumber);
	XINPUT_STATE GetState();
	bool IsConnected();
	void Vibrate(int leftVal = 0, int rightVal = 0);
};
