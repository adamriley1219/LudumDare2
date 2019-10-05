#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Input/KeyButtonState.hpp"


class AnalogJoystick
{
	friend class XboxController;

public:
	explicit AnalogJoystick( float innerDeadZoneFraction, float outerDeadZoneFraction );

	Vec2	GetPosition() const				{ return m_correctedPosition; }
	float	GetMagnitude() const				{ return m_correctedMagnitued; }
	float	GetAngleDegrees() const			{ return m_correctedDegrees; }
	Vec2	GetRawPosition() const				{ return m_rawPosition; }
	float	GetInnerDeadZoneFraction() const	{ return m_innerDeadZoneFraction; }
	float	GetOuterDeadZoneFraction() const	{ return m_outerDeadZoneFraction; }
	bool	WasJustTiledDown() const;
	bool	WasJustTiledRight() const;
	bool	WasJustTiledLeft() const;
	bool	WasJustTiledUp() const;
	bool	IsTiledDown() const;
	bool	IsTiledRight() const;
	bool	IsTiledLeft() const;
	bool	IsTiledUp() const;
	void	Reset();

private:
	void	UpdatePosition( float rawNormalizedX, float rawNormalizedY );

private:
	const float	m_innerDeadZoneFraction;
	const float m_outerDeadZoneFraction;
	Vec2 m_rawPosition				= Vec2::ZERO;
	Vec2 m_correctedPosition		= Vec2::ZERO;
	Vec2 m_prevCorrectedPosition	= Vec2::ZERO;
	float m_correctedDegrees		= 0.0f;
	float m_correctedMagnitued		= 0.0f;

	KeyButtonState m_downTiltButton;
	KeyButtonState m_rightTiltButton;
	KeyButtonState m_leftTiltButton;
	KeyButtonState m_upTiltButton;

	const float TILTTHRESHHOLD_ON	= 0.95f;
	const float TILTTHRESHHOLD_OFF	= 0.9f;
};