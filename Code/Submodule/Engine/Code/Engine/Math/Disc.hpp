#pragma once
#include "Engine/Math/Vec2.hpp"


struct Disc
{
public:
	Vec2 m_center = Vec2::ZERO;
	float m_radius = 0.0f;
public:
	Disc() {};
	explicit Disc( Vec2 center, float radius );

};