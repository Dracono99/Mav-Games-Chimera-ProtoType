#include "RawInput.h"
//#include "d3dUtil.h"
#include "MainGameManager.h"
#include "ChimeraGameEngine.h"
#include "InputCore.h"

RawInput::RawInput()
{
	RAWINPUTDEVICE Rid[2];

	// Keyboard
	Rid[0].usUsagePage = 1;
	Rid[0].usUsage = 6;
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget=NULL;

	// Mouse
	Rid[1].usUsagePage = 1;
	Rid[1].usUsage = 2;
	Rid[1].dwFlags = 0;
	Rid[1].hwndTarget=NULL;

	registerDevices(Rid,2);
}
void RawInput::registerDevices(PCRAWINPUTDEVICE Rid,UINT uiNumDevices)
{
	if (RegisterRawInputDevices(Rid,uiNumDevices,sizeof(RAWINPUTDEVICE))==FALSE)
	{
		::MessageBox(0, "Register RawInput Devices - Failed", 0, 0);
	}
}

RawInput* RawInput::instance()
{
	static RawInput instance;
	return &instance;
}

RawInput::~RawInput()
{
}
RAWINPUT* RawInput::buffer(LPARAM lParam)
{
	UINT dwSize;//buffer size

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT,NULL, &dwSize, sizeof(RAWINPUTHEADER));
	lpb = new BYTE[dwSize];
	if (lpb == NULL)
	{
		return 0;
	}
	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize )
	{
		OutputDebugString (TEXT("GetRawInputData does not return correct size !\n"));
	}
	RAWINPUT* raw = (RAWINPUT*)lpb;
	return raw;
}
void RawInput::GetRawInput(LPARAM lParam)
{
	RAWINPUT* raw = buffer(lParam);
	gd3dApp->GetEngine()->GetInputCore()->CatchRawInput(raw);
	delete lpb;
}