#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"


//--------------------------------------------------------------------------
/**
* AnalogJoystick
*/
AnalogJoystick::AnalogJoystick( float innerDeadZoneFraction, float outerDeadZoneFraction )
	: m_innerDeadZoneFraction( innerDeadZoneFraction )
	, m_outerDeadZoneFraction( outerDeadZoneFraction )
{

}

//--------------------------------------------------------------------------
/**
* WasJustTiledDown
*/
bool AnalogJoystick::WasJustTiledDown() const
{
	return m_downTiltButton.WasJustPressed();
}

//--------------------------------------------------------------------------
/**
* WasJustTiledRight
*/
bool AnalogJoystick::WasJustTiledRight() const
{
	return m_rightTiltButton.WasJustPressed();
}

//--------------------------------------------------------------------------
/**
* WasJustTiledLeft
*/
bool AnalogJoystick::WasJustTiledLeft() const
{
	return m_leftTiltButton.WasJustPressed();
}

//--------------------------------------------------------------------------
/**
* WasJustTiledUp
*/
bool AnalogJoystick::WasJustTiledUp() const
{
	return m_upTiltButton.WasJustPressed();
}

//--------------------------------------------------------------------------
/**
* IsTiledDown
*/
bool AnalogJoystick::IsTiledDown() const
{
	return m_downTiltButton.IsPressed();
}

//--------------------------------------------------------------------------
/**
* IsTiledRight
*/
bool AnalogJoystick::IsTiledRight() const
{
	return m_rightTiltButton.IsPressed();
}

//--------------------------------------------------------------------------
/**
* IsTiledLeft
*/
bool AnalogJoystick::IsTiledLeft() const
{
	return m_leftTiltButton.IsPressed();
}

//--------------------------------------------------------------------------
/**
* IsTiledUp
*/
bool AnalogJoystick::IsTiledUp() const
{
	return m_upTiltButton.IsPressed();
}

//--------------------------------------------------------------------------
/**
* Reset
*/
void AnalogJoystick::Reset()
{
	m_rawPosition			= Vec2( 0.0f, 0.0f );
	m_correctedPosition		= Vec2( 0.0f, 0.0f );
	m_correctedDegrees		= 0.0f;
	m_correctedMagnitued	= 0.0f;
}

//--------------------------------------------------------------------------
/**
* UpdatePosition
*/
void AnalogJoystick::UpdatePosition( float rawNormalizedX, float rawNormalizedY )
{
	m_prevCorrectedPosition = m_correctedPosition;

	m_rawPosition.x = rawNormalizedX;
	m_rawPosition.y = rawNormalizedY;
	
	if( m_rawPosition.GetLength() > m_outerDeadZoneFraction )
	{
		m_correctedPosition = Vec2::MakeFromPolarDegrees( m_rawPosition.GetAngleDegrees(), 1.0f );
	}
	else if ( m_rawPosition.GetLength() < m_innerDeadZoneFraction )
	{
		m_correctedPosition = Vec2::MakeFromPolarDegrees( m_rawPosition.GetAngleDegrees(), 0.0f );
	}
	else
	{
		float theta = m_rawPosition.GetAngleDegrees();
		float radius = m_rawPosition.GetLength();
		float convertedRadius = RangeMapFloat( radius, m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.0f, 1.0f  );
		m_correctedPosition = Vec2::MakeFromPolarDegrees( theta, convertedRadius );
	}
	
	if( m_rightTiltButton.IsPressed() )
		m_rightTiltButton.UpdateStatus( m_correctedPosition.x > TILTTHRESHHOLD_OFF );
	else
		m_rightTiltButton.UpdateStatus( m_correctedPosition.x > TILTTHRESHHOLD_ON );

	if( m_leftTiltButton.IsPressed() )
		m_leftTiltButton.UpdateStatus( m_correctedPosition.x < -TILTTHRESHHOLD_OFF );
	else
		m_leftTiltButton.UpdateStatus( m_correctedPosition.x < -TILTTHRESHHOLD_ON );

	if( m_downTiltButton.IsPressed() )
		m_downTiltButton.UpdateStatus( m_correctedPosition.y < -TILTTHRESHHOLD_OFF );
	else
		m_downTiltButton.UpdateStatus( m_correctedPosition.y < -TILTTHRESHHOLD_ON );

	if( m_upTiltButton.IsPressed() )
		m_upTiltButton.UpdateStatus( m_correctedPosition.y > TILTTHRESHHOLD_OFF );
	else
		m_upTiltButton.UpdateStatus( m_correctedPosition.y > TILTTHRESHHOLD_ON );
		

	m_correctedMagnitued = m_correctedPosition.GetLength();
	m_correctedDegrees = m_correctedPosition.GetAngleDegrees();
}

