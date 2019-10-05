#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"

#ifdef _WIN32
#define PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

const unsigned char KEY_ESC			= VK_ESCAPE;
const unsigned char KEY_ENTER		= VK_RETURN;
const unsigned char KEY_SPACEBAR	= VK_SPACE;
const unsigned char KEY_BACKSPACE	= VK_BACK;
const unsigned char KEY_DELETE		= VK_DELETE;

const unsigned char KEY_UPARROW		= VK_UP;
const unsigned char KEY_LEFTARROW	= VK_LEFT;
const unsigned char KEY_DOWNARROW	= VK_DOWN;
const unsigned char KEY_RIGHTARROW	= VK_RIGHT;

const unsigned char KEY_F1	= VK_F1;
const unsigned char KEY_F2	= VK_F2;
const unsigned char KEY_F3	= VK_F3;
const unsigned char KEY_F4	= VK_F4;
const unsigned char KEY_F5	= VK_F5;
const unsigned char KEY_F6	= VK_F6;
const unsigned char KEY_F7	= VK_F7;
const unsigned char KEY_F8	= VK_F8;
const unsigned char KEY_F9	= VK_F9;
const unsigned char KEY_F10	= VK_F10;
const unsigned char KEY_F11 = VK_F11;
const unsigned char KEY_F12	= VK_F12;

const unsigned char TILDE_KEY = 192;
const unsigned char SHIFT_KEY = VK_SHIFT;
const unsigned char SHIFT_KEY_L = VK_LSHIFT;
const unsigned char SHIFT_KEY_R = VK_RSHIFT;
const unsigned char CTRL_KEY = VK_CONTROL;
const unsigned char CTRL_KEY_L = VK_LCONTROL;
const unsigned char CTRL_KEY_R = VK_RCONTROL;

const unsigned char KEY_NUM_0 = 48;
const unsigned char KEY_NUM_1 = 49;
const unsigned char KEY_NUM_2 = 50;
const unsigned char KEY_NUM_3 = 51;
const unsigned char KEY_NUM_4 = 52;
const unsigned char KEY_NUM_5 = 53;
const unsigned char KEY_NUM_6 = 54;
const unsigned char KEY_NUM_7 = 55;
const unsigned char KEY_NUM_8 = 56;
const unsigned char KEY_NUM_9 = 57;

const unsigned char KEY_A = 65;
const unsigned char KEY_B = 66;
const unsigned char KEY_C = 67;
const unsigned char KEY_D = 68;
const unsigned char KEY_E = 69;
const unsigned char KEY_F = 70;
const unsigned char KEY_G = 71;
const unsigned char KEY_H = 72;
const unsigned char KEY_I = 73;
const unsigned char KEY_J = 74;
const unsigned char KEY_K = 75;
const unsigned char KEY_L = 76;
const unsigned char KEY_M = 77;
const unsigned char KEY_N = 78;
const unsigned char KEY_O = 79;
const unsigned char KEY_P = 80;
const unsigned char KEY_Q = 81;
const unsigned char KEY_R = 82;
const unsigned char KEY_S = 83;
const unsigned char KEY_T = 84;
const unsigned char KEY_U = 85;
const unsigned char KEY_V = 86;
const unsigned char KEY_W = 87;
const unsigned char KEY_X = 88;
const unsigned char KEY_Y = 89;
const unsigned char KEY_Z = 90;

const unsigned char MOUSE_L = VK_LBUTTON;
const unsigned char MOUSE_R = VK_RBUTTON;
const unsigned char MOUSE_M = VK_MBUTTON;

//--------------------------------------------------------------------------
/**
* InputSystem
*/
InputSystem::InputSystem()
{

}

//--------------------------------------------------------------------------
/**
* ~InputSystem
*/
InputSystem::~InputSystem()
{

}

//--------------------------------------------------------------------------
/**
* Startup
*/
void InputSystem::Startup()
{

}

//--------------------------------------------------------------------------
/**
* BeginFrame
*/
void InputSystem::BeginFrame()
{
#ifdef PLATFORM_WINDOWS
	for( int i = 0; i < MAX_XBOX_CONTROLLERS ; i++ )
	{
		m_controllers[i].Update();
	}

	char key[256];

	//fill the keys from windowsState

	GetKeyboardState((PBYTE)&key);

	for( int idx = 0; idx < 256; ++idx )
	{
		bool res = key[idx] & 0XF0;
		m_keyStates[idx].UpdateStatus( res );
	}

	for( auto button : m_mouseStates )
	{
		button.second->UpdateStatus( GetKeyState( button.first ) & 0XF0 );
	}
#else
#endif
}

//--------------------------------------------------------------------------
/**
* EndFrame
*/
void InputSystem::EndFrame()
{
	
}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void InputSystem::Shutdown()
{
	for( auto button : m_mouseStates )
	{
		SAFE_DELETE( button.second );
	}
}

//--------------------------------------------------------------------------
/**
* GetControllerByID
*/
const XboxController& InputSystem::GetControllerByID( int controllerID )
{
	return m_controllers[controllerID];
}

//--------------------------------------------------------------------------
/**
* IsShiftPressed
*/
bool InputSystem::IsShiftPressed()
{
#ifdef PLATFORM_WINDOWS
	return GetKeyState( VK_SHIFT ) & 0XF0;
#else
	return false;
#endif
}

//--------------------------------------------------------------------------
/**
* IsCtrlPressed
*/
bool InputSystem::IsCtrlPressed()
{
#ifdef PLATFORM_WINDOWS
	return GetKeyState( VK_CONTROL ) & 0XF0;
#else
	return false;
#endif
}

//--------------------------------------------------------------------------
/**
* IsAltPressed
*/
bool InputSystem::IsAltPressed()
{
#ifdef PLATFORM_WINDOWS
	return GetKeyState( VK_MENU ) & 0XF0;
#else
	return false;
#endif
}

//--------------------------------------------------------------------------
/**
* KeyIsDown
*/
bool InputSystem::KeyIsDown( unsigned char key ) const
{
	return m_keyStates[key].IsPressed();
}

//--------------------------------------------------------------------------
/**
* KeyIsUp
*/
bool InputSystem::KeyIsUp( unsigned char key ) const
{
	return !m_keyStates[key].IsPressed();
}

//--------------------------------------------------------------------------
/**
* KeyWasPressed
*/
bool InputSystem::KeyWasPressed( unsigned char key ) const
{
	return m_keyStates[key].WasJustPressed();
}

//--------------------------------------------------------------------------
/**
* KeyWasReleased
*/
bool InputSystem::KeyWasReleased( unsigned char key ) const
{
	return m_keyStates[key].WasJustReleased();
}

//--------------------------------------------------------------------------
/**
* GetMouseScroll
*/
float InputSystem::GetMouseScroll() const
{
	return m_wheelOffset;
}

//--------------------------------------------------------------------------
/**
* WheelMovement
*/
void InputSystem::WheelMovement( float offset )
{
	m_wheelOffset = offset;
}

