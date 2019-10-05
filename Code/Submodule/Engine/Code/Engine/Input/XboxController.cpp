#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"

#include "Engine/Core/EngineCommon.hpp"

#ifdef _WIN32
#define PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // must #include Windows.h before #including Xinput.h
#include <Xinput.h> // include the Xinput API header file (interface)
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use version 9_1_0 explicitly for broadest compatibility
#endif


//--------------------------------------------------------------------------
/**
* XboxController
*/
XboxController::XboxController( int controllerID )
	: m_controllerID(controllerID)
{

}

//--------------------------------------------------------------------------
/**
* UpdateXboxController
*/
XboxController::XboxController( const XboxController& toCopy )
	: m_controllerID( toCopy.GetControllerID() )
{
	
}

//--------------------------------------------------------------------------
/**
* ~XboxController
*/
XboxController::~XboxController()
{

}

//--------------------------------------------------------------------------
/**
* GetButtonState
*/
const KeyButtonState& XboxController::GetButtonState( XboxButtonID buttonID ) const
{
	return m_buttonStates[buttonID];
}

//--------------------------------------------------------------------------
/**
* Update
*/
void XboxController::Update()
{
#ifdef PLATFORM_WINDOWS
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	GUARANTEE_OR_DIE( m_controllerID >= 0 && m_controllerID < MAX_XBOX_CONTROLLERS, "Error: Invalid Contrtoller ID");
	DWORD errorStatus = XInputGetState( m_controllerID, &xboxControllerState );
	if( errorStatus == ERROR_SUCCESS )
	{
		m_isConnected = true;
		// Updating Buttons 
		UpdateButton( XBOX_BUTTON_ID_DPAD_UP, XINPUT_GAMEPAD_DPAD_UP, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_LEFT, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_DPAD_RIGHT, XINPUT_GAMEPAD_DPAD_RIGHT, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_DPAD_DOWN, XINPUT_GAMEPAD_DPAD_DOWN, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_START, XINPUT_GAMEPAD_START, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_BACK, XINPUT_GAMEPAD_BACK, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_LTHUMB, XINPUT_GAMEPAD_LEFT_THUMB, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_RTHUMB, XINPUT_GAMEPAD_RIGHT_THUMB, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_LSHOULDER, XINPUT_GAMEPAD_LEFT_SHOULDER, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_RSHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_A, XINPUT_GAMEPAD_A, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_B, XINPUT_GAMEPAD_B, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_X, XINPUT_GAMEPAD_X, xboxControllerState.Gamepad.wButtons );
		UpdateButton( XBOX_BUTTON_ID_Y, XINPUT_GAMEPAD_Y, xboxControllerState.Gamepad.wButtons );

		// Updating Trigger values
		UpdateTrigger( m_leftTriggerValueNormalized, xboxControllerState.Gamepad.bLeftTrigger );
		UpdateTrigger( m_rightTriggerValueNormalized, xboxControllerState.Gamepad.bRightTrigger );
		UpdateTriggersAsButtons();

		// Updating Joystick Values;
		UpdateJoystick( m_leftJoystick, xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY );
		UpdateJoystick( m_rightJoystick, xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY );
	}
	else if( errorStatus == ERROR_DEVICE_NOT_CONNECTED )
	{
		m_isConnected = false;

		Reset();
	}
	else
	{
//		ERROR_RECOVERABLE( Stringf( "XBOX CONTROLLER gave an ERROR that was not handled by the Engine. XboxController::Update, with error %d", errorStatus ).c_str() );
	}
#else
#endif
}

//--------------------------------------------------------------------------
/**
* Reset
*/
void XboxController::Reset()
{
	for( int i = 0; i < NUM_XBOX_BUTTONS; i++)
	{
		m_buttonStates[i].Reset();
	}
	m_leftJoystick.Reset();
	m_rightJoystick.Reset();
	m_leftTriggerValueNormalized = 0.0f;
	m_rightTriggerValueNormalized = 0.0f;
}

//--------------------------------------------------------------------------
/**
* UpdateTriggersAsButtons
*/
void XboxController::UpdateTriggersAsButtons()
{
	if( GetButtonState( XBOX_BUTTON_ID_RTRIGGER ).IsPressed() && m_rightTriggerValueNormalized > 0.3f
		|| !GetButtonState( XBOX_BUTTON_ID_RTRIGGER ).IsPressed() && m_rightTriggerValueNormalized > 0.7f )
	{
		m_buttonStates[XBOX_BUTTON_ID_RTRIGGER].UpdateStatus( true );
	}
	else if( !GetButtonState( XBOX_BUTTON_ID_RTRIGGER ).IsPressed() && m_rightTriggerValueNormalized <= 0.7f
		|| GetButtonState( XBOX_BUTTON_ID_RTRIGGER ).IsPressed() && m_rightTriggerValueNormalized <= 0.3f )
	{
		m_buttonStates[XBOX_BUTTON_ID_RTRIGGER].UpdateStatus( false );
	}

	if( GetButtonState( XBOX_BUTTON_ID_LTRIGGER ).IsPressed() && m_leftTriggerValueNormalized > 0.3f
		|| !GetButtonState( XBOX_BUTTON_ID_LTRIGGER ).IsPressed() && m_leftTriggerValueNormalized > 0.7f )
	{
		m_buttonStates[XBOX_BUTTON_ID_LTRIGGER].UpdateStatus( true );
	}
	else if( !GetButtonState( XBOX_BUTTON_ID_LTRIGGER ).IsPressed() && m_leftTriggerValueNormalized <= 0.7f
		|| GetButtonState( XBOX_BUTTON_ID_LTRIGGER ).IsPressed() && m_leftTriggerValueNormalized <= 0.3f )
	{
		m_buttonStates[XBOX_BUTTON_ID_LTRIGGER].UpdateStatus( false );
	}
}

//--------------------------------------------------------------------------
/**
* UpdateTrigger
*/
void XboxController::UpdateTrigger( float& triggerValue, unsigned char rawValue )
{
	triggerValue = (float) rawValue / 255.0f;
}

//--------------------------------------------------------------------------
/**
* UpdateJoystick
*/
void XboxController::UpdateJoystick( AnalogJoystick& joystick, short rawX, short rawY )
{
	float normX = (float) rawX / 32757.0f;
	float normY = (float) rawY / 32757.0f;
	joystick.UpdatePosition( normX, normY ); // Normalize before updating. Joystick shouldn't know where the values come from.
}

//--------------------------------------------------------------------------
/**
* UpdateButton
*/
void XboxController::UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonTag )
{
	bool isDown = IsBitFlagSet( buttonTag, buttonFlags );
	m_buttonStates[buttonID].UpdateStatus( isDown );
}
