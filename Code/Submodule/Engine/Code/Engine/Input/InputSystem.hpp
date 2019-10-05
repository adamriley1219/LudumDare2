#pragma once
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <map>


//--------------------------------------------------------------------------
constexpr int MAX_XBOX_CONTROLLERS = 4;

//--------------------------------------------------------------------------
// Key codes
// 
extern const unsigned char KEY_ESC;
extern const unsigned char KEY_ENTER;
extern const unsigned char KEY_SPACEBAR;
extern const unsigned char KEY_BACKSPACE;
extern const unsigned char KEY_DELETE;

extern const unsigned char KEY_UPARROW;
extern const unsigned char KEY_LEFTARROW;
extern const unsigned char KEY_DOWNARROW;
extern const unsigned char KEY_RIGHTARROW;

extern const unsigned char KEY_F1;
extern const unsigned char KEY_F2;
extern const unsigned char KEY_F3;
extern const unsigned char KEY_F4;
extern const unsigned char KEY_F5;
extern const unsigned char KEY_F6;
extern const unsigned char KEY_F7;
extern const unsigned char KEY_F8;
extern const unsigned char KEY_F9;
extern const unsigned char KEY_F10;
extern const unsigned char KEY_F11;
extern const unsigned char KEY_F12;

extern const unsigned char TILDE_KEY;
extern const unsigned char SHIFT_KEY;
extern const unsigned char SHIFT_KEY_L;
extern const unsigned char SHIFT_KEY_R;
extern const unsigned char CTRL_KEY;
extern const unsigned char CTRL_KEY_L;
extern const unsigned char CTRL_KEY_R;

extern const unsigned char KEY_NUM_0;
extern const unsigned char KEY_NUM_1;
extern const unsigned char KEY_NUM_2;
extern const unsigned char KEY_NUM_3;
extern const unsigned char KEY_NUM_4;
extern const unsigned char KEY_NUM_5;
extern const unsigned char KEY_NUM_6;
extern const unsigned char KEY_NUM_7;
extern const unsigned char KEY_NUM_8;
extern const unsigned char KEY_NUM_9;

extern const unsigned char KEY_A;
extern const unsigned char KEY_B;
extern const unsigned char KEY_C;
extern const unsigned char KEY_D;
extern const unsigned char KEY_E;
extern const unsigned char KEY_F;
extern const unsigned char KEY_G;
extern const unsigned char KEY_H;
extern const unsigned char KEY_I;
extern const unsigned char KEY_J;
extern const unsigned char KEY_K;
extern const unsigned char KEY_L;
extern const unsigned char KEY_M;
extern const unsigned char KEY_N;
extern const unsigned char KEY_O;
extern const unsigned char KEY_P;
extern const unsigned char KEY_Q;
extern const unsigned char KEY_R;
extern const unsigned char KEY_S;
extern const unsigned char KEY_T;
extern const unsigned char KEY_U;
extern const unsigned char KEY_V;
extern const unsigned char KEY_W;
extern const unsigned char KEY_X;
extern const unsigned char KEY_Y;
extern const unsigned char KEY_Z;

extern const unsigned char MOUSE_L;
extern const unsigned char MOUSE_R;
extern const unsigned char MOUSE_M;

//--------------------------------------------------------------------------
class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	const XboxController& GetControllerByID( int controllerID );

	static bool IsShiftPressed();
	static bool IsCtrlPressed();
	static bool IsAltPressed();

	bool KeyIsDown( unsigned char key ) const; 
	bool KeyIsUp( unsigned char key ) const;
	bool KeyWasPressed( unsigned char key ) const;   // was pressed this frame
	bool KeyWasReleased( unsigned char key ) const;  // was released this frame; 

	float GetMouseScroll() const; 

	void WheelMovement( float offset );

private:
	KeyButtonState m_keyStates[256];
	mutable std::map< unsigned int, KeyButtonState* > m_mouseStates;
	float m_wheelOffset				 = 0.0f;

private:
	XboxController m_controllers[ MAX_XBOX_CONTROLLERS ] =
	{
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3)
	};
};