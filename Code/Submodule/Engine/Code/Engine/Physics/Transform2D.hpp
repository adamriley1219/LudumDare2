#pragma once
#include "Engine/Math/Vec2.hpp"




struct Transform2D
{
public:
	Transform2D(){};
	explicit Transform2D( const Vec2& position, float rotationDegrees = 0.0f, const Vec2& scale = Vec2::ONE );

public:
	float m_rotation = 0.0f;
	Vec2 m_position = Vec2::ZERO;
	Vec2 m_scale = Vec2::ONE;

public:
	void operator=( const Transform2D& copyFrom );	

};

