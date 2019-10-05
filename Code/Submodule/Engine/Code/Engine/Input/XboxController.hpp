#pragma once
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/AnalogJoystick.hpp"

enum XboxButtonID
{
	XBOX_BUTTON_ID_INVALID = -1,

	XBOX_BUTTON_ID_A,
	XBOX_BUTTON_ID_B,
	XBOX_BUTTON_ID_X,
	XBOX_BUTTON_ID_Y,
	XBOX_BUTTON_ID_BACK,
	XBOX_BUTTON_ID_START,
	XBOX_BUTTON_ID_LSHOULDER,
	XBOX_BUTTON_ID_RSHOULDER,
	XBOX_BUTTON_ID_LTHUMB,
	XBOX_BUTTON_ID_RTHUMB,
	XBOX_BUTTON_ID_LTRIGGER,
	XBOX_BUTTON_ID_RTRIGGER,
	XBOX_BUTTON_ID_DPAD_RIGHT,
	XBOX_BUTTON_ID_DPAD_UP,
	XBOX_BUTTON_ID_DPAD_LEFT,
	XBOX_BUTTON_ID_DPAD_DOWN,


	NUM_XBOX_BUTTONS	
};

class XboxController
{
	friend class InputSystem;

public:
	XboxController( const XboxController& toCopy );
	explicit XboxController( int controllerID );
	~XboxController();

	bool					IsConnected() const			{ return m_isConnected; }
	int						GetControllerID() const		{ return m_controllerID; }
	const AnalogJoystick&	GetLeftJoystick() const		{ return m_leftJoystick; }
	const AnalogJoystick&	GetRightJoystick() const	{ return m_rightJoystick; }
	float					GetLeftTrigger() const		{ return m_leftTriggerValueNormalized; }
	float					GetRightTrigger() const		{ return m_rightTriggerValueNormalized; }
	const KeyButtonState&	GetButtonState( XboxButtonID buttonID ) const;

private:
	void Update();
	void Reset();
	void UpdateTriggersAsButtons();
	void UpdateTrigger( float& triggerValue, unsigned char rawValue );
	void UpdateJoystick( AnalogJoystick& joystick, short rawX, short rawY );
	void UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonTag );

public:
	const int		m_controllerID = -1;
	bool			m_isConnected = false;
	KeyButtonState	m_buttonStates[ NUM_XBOX_BUTTONS ];
	AnalogJoystick	m_leftJoystick = AnalogJoystick( 0.3f, 0.9f );
	AnalogJoystick	m_rightJoystick = AnalogJoystick( 0.3f, 0.9f );
	float			m_leftTriggerValueNormalized = 0.0f;
	float			m_rightTriggerValueNormalized = 0.0f;
};
