#pragma once
#include "Engine/Math/Vec3.hpp"


struct Capsule3
{
public:
	Vec3 m_start;
	Vec3 m_end;
	float m_radius;

public:
	explicit Capsule3( const Vec3& start, const Vec3& end, float radius );

};