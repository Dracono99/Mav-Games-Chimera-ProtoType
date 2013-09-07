#pragma once
#include "RawInput.h"
#include "XInput.h"

struct xInputValues
{
	bool mLeftStickActive;
	float mLeftStick_X_Val;
	float mLeftStick_Y_Val;

	bool mRightStickActive;
	float mRightStick_X_Val;
	float mRightStick_Y_Val;

	bool mRightTriggerActive;
	float mRightTriggerValue;

	bool mLeftTriggerActive;
	float mLeftTRiggerValue;

	bool m_BackButtonDown;
	bool m_BackButtonUp;
	bool m_BackButtonPress;

	bool m_StartButtonDown;
	bool m_StartButtonUp;
	bool m_StartButtonPress;

	bool m_LeftShoulderDown;
	bool m_LeftShoulderUp;
	bool m_LeftShoulderPress;

	bool m_RightShoulderDown;
	bool m_RightShoulderUp;
	bool m_RightShoulderPress;

	bool m_X_ButtonDown;
	bool m_X_ButtonUp;
	bool m_X_ButtonPress;

	bool m_A_ButtonDown;
	bool m_A_ButtonUp;
	bool m_A_ButtonPress;

	bool m_B_ButtonDown;
	bool m_B_ButtonUp;
	bool m_B_ButtonPress;

	bool m_Y_ButtonDown;
	bool m_Y_ButtonUp;
	bool m_Y_ButtonPress;

	bool m_DpadUpButtonDown;
	bool m_DpadUpButtonUp;
	bool m_DpadUpButtonPress;

	bool m_DpadLeftButtonDown;
	bool m_DpadLeftButtonUp;
	bool m_DpadLeftButtonPress;

	bool m_DpadRightButtonDown;
	bool m_DpadRightButtonUp;
	bool m_DpadRightButtonPress;

	bool m_DpadDownButtonDown;
	bool m_DpadDownButtonUp;
	bool m_DpadDownButtonPress;
};

struct RawInputValues
{
	bool m_LeftMouseButtonUp;
	bool m_LeftMouseButtonDown;
	bool m_LeftMouseButtonPress;

	bool m_RightMouseButtonUp;
	bool m_RightMouseButtonDown;
	bool m_RightMouseButtonPress;
};

class InputCore
{
private:
	CXBOXController* m_Controller;
	xInputValues mXinputVals;
	RawInputValues mRawInputValues;
	void CatchControllerInput();
public:
	InputCore();
	~InputCore();
	void Update();
	void CatchRawInput(RAWINPUT* raw);
	xInputValues* GetXinput();
	RawInputValues* GetRawInput();
};