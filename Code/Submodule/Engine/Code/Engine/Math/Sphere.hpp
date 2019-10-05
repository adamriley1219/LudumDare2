#pragma once
#include "Engine/Math/Vec3.hpp"


struct Sphere
{
public:
	Vec3 m_center = Vec3::ZERO;
	float m_radius = 1.0f;

public:
	Sphere();
	Sphere( const Vec3& center, float radius );

};