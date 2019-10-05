#include "Engine/Input/KeyButtonState.hpp"

//--------------------------------------------------------------------------
/**
* UpdateStatus
*/
void KeyButtonState::UpdateStatus( bool isNowPressed )
{
	m_wasPressedLastFrame = m_isPressed;
	m_isPressed = isNowPressed;
}

//--------------------------------------------------------------------------
/**
* IsPressed
*/
bool KeyButtonState::IsPressed() const
{
	return m_isPressed;
}

//--------------------------------------------------------------------------
/**
* WasJustPressed
*/
bool KeyButtonState::WasJustPressed() const
{
	return m_isPressed && !m_wasPressedLastFrame;
}

//--------------------------------------------------------------------------
/**
* WasJustReleased
*/
bool KeyButtonState::WasJustReleased() const
{
	return !m_isPressed && m_wasPressedLastFrame;
}

//--------------------------------------------------------------------------
/**
* Reset
*/
void KeyButtonState::Reset()
{
	m_isPressed = false;
	m_wasPressedLastFrame = false;
}

