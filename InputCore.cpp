#include "InputCore.h"
#include <math.h>

InputCore::InputCore()
{
	m_Controller = new CXBOXController(1);
	mXinputVals.m_A_ButtonDown=false;
	mXinputVals.m_A_ButtonUp=true;
	mXinputVals.m_A_ButtonPress=false;
	mXinputVals.m_B_ButtonDown=false;
	mXinputVals.m_B_ButtonUp=true;
	mXinputVals.m_B_ButtonPress=false;
	mXinputVals.m_X_ButtonDown=false;
	mXinputVals.m_X_ButtonUp=true;
	mXinputVals.m_X_ButtonPress=false;
	mXinputVals.m_Y_ButtonDown=false;
	mXinputVals.m_Y_ButtonUp=true;
	mXinputVals.m_Y_ButtonPress=false;
	mXinputVals.m_BackButtonDown=false;
	mXinputVals.m_BackButtonPress=false;
	mXinputVals.m_BackButtonUp=true;
	mXinputVals.m_StartButtonDown=false;
	mXinputVals.m_StartButtonPress=false;
	mXinputVals.m_StartButtonUp=true;
	mXinputVals.m_DpadDownButtonDown=false;
	mXinputVals.m_DpadDownButtonUp=true;
	mXinputVals.m_DpadDownButtonPress=false;
	mXinputVals.m_DpadUpButtonDown=false;
	mXinputVals.m_DpadUpButtonUp=true;
	mXinputVals.m_DpadUpButtonPress=false;
	mXinputVals.m_DpadLeftButtonDown=false;
	mXinputVals.m_DpadLeftButtonUp=true;
	mXinputVals.m_DpadLeftButtonPress=false;
	mXinputVals.m_DpadRightButtonDown=false;
	mXinputVals.m_DpadRightButtonUp=true;
	mXinputVals.m_DpadRightButtonPress=false;
	mXinputVals.m_RightShoulderDown=false;
	mXinputVals.m_RightShoulderPress=false;
	mXinputVals.m_RightShoulderUp=true;
	mXinputVals.m_LeftShoulderDown=false;
	mXinputVals.m_LeftShoulderPress=false;
	mXinputVals.m_LeftShoulderUp=true;
	mXinputVals.mLeftStickActive=false;
	mXinputVals.mLeftStick_X_Val=0.0f;
	mXinputVals.mLeftStick_Y_Val=0.0f;
	mXinputVals.mRightStickActive=false;
	mXinputVals.mRightStick_X_Val=0.0f;
	mXinputVals.mRightStick_Y_Val=0.0f;
	mXinputVals.mLeftTriggerActive=false;
	mXinputVals.mLeftTRiggerValue=0.0f;
	mXinputVals.mRightTriggerActive=false;
	mXinputVals.mRightTriggerValue=0.0f;

	mRawInputValues.m_LeftMouseButtonDown=false;
	mRawInputValues.m_LeftMouseButtonUp=true;
	mRawInputValues.m_LeftMouseButtonPress=false;
	mRawInputValues.m_RightMouseButtonDown=false;
	mRawInputValues.m_RightMouseButtonPress=false;
	mRawInputValues.m_RightMouseButtonUp=true;
}

InputCore::~InputCore()
{
	delete m_Controller;
}

void InputCore::CatchControllerInput()
{
	// get controller state
	XINPUT_STATE state =  m_Controller->GetState();
	float LX = state.Gamepad.sThumbLX;
	float LY = state.Gamepad.sThumbLY;

	//determine how far the controller is pushed
	float magnitude = sqrt(LX*LX + LY*LY);
	if(magnitude==0)
	{
		magnitude=1;
	}

	//determine the direction the controller is pushed
	float normalizedLX = LX / magnitude;
	float normalizedLY = LY / magnitude;

	float normalizedMagnitude = 0;

	//check if the controller is outside a circular dead zone
	if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		mXinputVals.mLeftStickActive=true;
		//clip the magnitude at its expected maximum value
		if (magnitude > 32767) magnitude = 32767;

		//adjust magnitude relative to the end of the dead zone
		magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	}
	else //if the controller is in the dead zone zero out the magnitude
	{
		mXinputVals.mLeftStickActive=false;
		magnitude = 0.0;
		normalizedMagnitude = 0.0;
	}
	mXinputVals.mLeftStick_X_Val=(normalizedLX*magnitude);
	mXinputVals.mLeftStick_Y_Val=(normalizedLY*magnitude);

	//repeat for right thumb stick
	float RX = state.Gamepad.sThumbRX;
	float RY = state.Gamepad.sThumbRY;

	//determine how far the controller is pushed
	magnitude = sqrt(RX*RX + RY*RY);
	if(magnitude==0)
	{
		magnitude=1;
	}

	//determine the direction the controller is pushed
	float normalizedRX = RX / magnitude;
	float normalizedRY = RY / magnitude;

	normalizedMagnitude = 0;

	//check if the controller is outside a circular dead zone
	if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		mXinputVals.mRightStickActive=true;
		//clip the magnitude at its expected maximum value
		if (magnitude > 32767) magnitude = 32767;

		//adjust magnitude relative to the end of the dead zone
		magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	}
	else //if the controller is in the dead zone zero out the magnitude
	{
		mXinputVals.mRightStickActive=false;
		magnitude = 0.0;
		normalizedMagnitude = 0.0;
	}
	mXinputVals.mRightStick_X_Val=(normalizedRX*magnitude);
	mXinputVals.mRightStick_Y_Val=(normalizedRY*magnitude);

	// get right trigger data
	float RT = state.Gamepad.bRightTrigger;
	if(RT>0)
	{
		mXinputVals.mRightTriggerActive=true;
		mXinputVals.mRightTriggerValue=RT;
	}
	else
	{
		mXinputVals.mRightTriggerActive=false;
	}
	//get left trigger data
	float LT = state.Gamepad.bLeftTrigger;
	if(LT>0)
	{
		mXinputVals.mLeftTriggerActive=true;
		mXinputVals.mLeftTRiggerValue=LT;
	}
	else
	{
		mXinputVals.mLeftTriggerActive=false;
	}

	// game pad right shoulder
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		if(mXinputVals.m_RightShoulderUp)
		{
			mXinputVals.m_RightShoulderDown=true;
			mXinputVals.m_RightShoulderUp=false;
			mXinputVals.m_RightShoulderPress=true;
		}
		else
		{
			mXinputVals.m_RightShoulderPress=false;
		}
	}
	else
	{
		mXinputVals.m_RightShoulderDown=false;
		mXinputVals.m_RightShoulderUp=true;
		mXinputVals.m_RightShoulderPress=false;
	}

	// game pad left shoulder
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
	{
		if(mXinputVals.m_LeftShoulderUp)
		{
			mXinputVals.m_LeftShoulderDown=true;
			mXinputVals.m_LeftShoulderUp=false;
			mXinputVals.m_LeftShoulderPress=true;
		}
		else
		{
			mXinputVals.m_LeftShoulderPress=false;
		}
	}
	else
	{
		mXinputVals.m_LeftShoulderDown=false;
		mXinputVals.m_LeftShoulderUp=true;
		mXinputVals.m_LeftShoulderPress=false;
	}

	// game pad back button
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
	{
		if(mXinputVals.m_BackButtonUp)
		{
			mXinputVals.m_BackButtonDown=true;
			mXinputVals.m_BackButtonUp=false;
			mXinputVals.m_BackButtonPress=true;
		}
		else
		{
			mXinputVals.m_BackButtonPress=false;
		}
	}
	else
	{
		mXinputVals.m_BackButtonDown=false;
		mXinputVals.m_BackButtonUp=true;
		mXinputVals.m_BackButtonPress=false;
	}

	// game pad start button
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
	{
		if(mXinputVals.m_StartButtonUp)
		{
			mXinputVals.m_StartButtonDown=true;
			mXinputVals.m_StartButtonUp=false;
			mXinputVals.m_StartButtonPress=true;
		}
		else
		{
			mXinputVals.m_StartButtonPress=false;
		}
	}
	else
	{
		mXinputVals.m_StartButtonDown=false;
		mXinputVals.m_StartButtonUp=true;
		mXinputVals.m_StartButtonPress=false;
	}

	// game pad dpad down arrow
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		if(mXinputVals.m_DpadDownButtonUp)
		{
			mXinputVals.m_DpadDownButtonDown=true;
			mXinputVals.m_DpadDownButtonUp=false;
			mXinputVals.m_DpadDownButtonPress=true;
		}
		else
		{
			mXinputVals.m_DpadDownButtonPress=false;
		}
	}
	else
	{
		mXinputVals.m_DpadDownButtonDown=false;
		mXinputVals.m_DpadDownButtonUp=true;
		mXinputVals.m_DpadDownButtonPress=false;
	}

	// game pad dpad up arrow
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		if(mXinputVals.m_DpadUpButtonUp)
		{
			mXinputVals.m_DpadUpButtonDown=true;
			mXinputVals.m_DpadUpButtonUp=false;
			mXinputVals.m_DpadUpButtonPress=true;
		}
		else
		{
			mXinputVals.m_DpadUpButtonPress=false;
		}
	}
	else
	{
		mXinputVals.m_DpadUpButtonDown=false;
		mXinputVals.m_DpadUpButtonUp=true;
		mXinputVals.m_DpadUpButtonPress=false;
	}

	// game pad dpad left arrow
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
	{
		if(mXinputVals.m_DpadLeftButtonUp)
		{
			mXinputVals.m_DpadLeftButtonDown=true;
			mXinputVals.m_DpadLeftButtonUp=false;
			mXinputVals.m_DpadLeftButtonPress=true;
		}
		else
		{
			mXinputVals.m_DpadLeftButtonPress=false;
		}
	}
	else
	{
		mXinputVals.m_DpadLeftButtonDown=false;
		mXinputVals.m_DpadLeftButtonUp=true;
		mXinputVals.m_DpadLeftButtonPress=false;
	}

	// game pad dpad right arrow
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
	{
		if(mXinputVals.m_DpadRightButtonUp)
		{
			mXinputVals.m_DpadRightButtonDown=true;
			mXinputVals.m_DpadRightButtonUp=false;
			mXinputVals.m_DpadRightButtonPress=true;
		}
		else
		{
			mXinputVals.m_DpadRightButtonPress=false;
		}
	}
	else
	{
		mXinputVals.m_DpadRightButtonDown=false;
		mXinputVals.m_DpadRightButtonUp=true;
		mXinputVals.m_DpadRightButtonPress=false;
	}

	// game pad y button
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
	{
		if(mXinputVals.m_Y_ButtonUp)
		{
			mXinputVals.m_Y_ButtonDown=true;
			mXinputVals.m_Y_ButtonUp=false;
			mXinputVals.m_Y_ButtonPress=true;
		}
		else
		{
			mXinputVals.m_Y_ButtonPress=false;
		}
	}
	else
	{
		mXinputVals.m_Y_ButtonDown=false;
		mXinputVals.m_Y_ButtonUp=true;
		mXinputVals.m_Y_ButtonPress=false;
	}

	// game pad x button
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
	{
		if(mXinputVals.m_X_ButtonUp)
		{
			mXinputVals.m_X_ButtonDown=true;
			mXinputVals.m_X_ButtonUp=false;
			mXinputVals.m_X_ButtonPress=true;
		}
		else
		{
			mXinputVals.m_X_ButtonPress=false;
		}
	}
	else
	{
		mXinputVals.m_X_ButtonDown=false;
		mXinputVals.m_X_ButtonUp=true;
		mXinputVals.m_X_ButtonPress=false;
	}

	// game pad a button
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		if(mXinputVals.m_A_ButtonUp)
		{
			mXinputVals.m_A_ButtonDown=true;
			mXinputVals.m_A_ButtonUp=false;
			mXinputVals.m_A_ButtonPress=true;
		}
		else
		{
			mXinputVals.m_A_ButtonPress=false;
		}
	}
	else
	{
		mXinputVals.m_A_ButtonDown=false;
		mXinputVals.m_A_ButtonUp=true;
		mXinputVals.m_A_ButtonPress=false;
	}

	// game pad b button
	if(state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
	{
		if(mXinputVals.m_B_ButtonUp)
		{
			mXinputVals.m_B_ButtonDown=true;
			mXinputVals.m_B_ButtonUp=false;
			mXinputVals.m_B_ButtonPress=true;
		}
		else
		{
			mXinputVals.m_B_ButtonPress=false;
		}
	}
	else
	{
		mXinputVals.m_B_ButtonDown=false;
		mXinputVals.m_B_ButtonUp=true;
		mXinputVals.m_B_ButtonPress=false;
	}
}

void InputCore::Update()
{
	CatchControllerInput();
}

void InputCore::CatchRawInput(RAWINPUT* raw)
{
	if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		// put keyboard keys here
	}
	if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		// left mouse button data
		if(raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN)
		{
			if(mRawInputValues.m_LeftMouseButtonUp)
			{
				mRawInputValues.m_LeftMouseButtonDown=true;
				mRawInputValues.m_LeftMouseButtonPress=true;
				mRawInputValues.m_LeftMouseButtonUp=false;
			}
			else
			{
				mRawInputValues.m_LeftMouseButtonPress=false;
			}
		}
		if(raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP)
		{
			mRawInputValues.m_LeftMouseButtonDown=false;
			mRawInputValues.m_LeftMouseButtonUp=true;
			mRawInputValues.m_LeftMouseButtonPress=false;
		}

		// right mouse button data
		if(raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN)
		{
			if(mRawInputValues.m_RightMouseButtonUp)
			{
				mRawInputValues.m_RightMouseButtonDown=true;
				mRawInputValues.m_RightMouseButtonPress=true;
				mRawInputValues.m_RightMouseButtonUp=false;
			}
			else
			{
				mRawInputValues.m_RightMouseButtonPress=false;
			}
		}
		if(raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_UP)
		{
			mRawInputValues.m_RightMouseButtonDown=false;
			mRawInputValues.m_RightMouseButtonUp=true;
			mRawInputValues.m_RightMouseButtonPress=false;
		}
	}
}

xInputValues* InputCore::GetXinput()
{
	return &mXinputVals;
}

RawInputValues* InputCore::GetRawInput()
{
	return &mRawInputValues;
}