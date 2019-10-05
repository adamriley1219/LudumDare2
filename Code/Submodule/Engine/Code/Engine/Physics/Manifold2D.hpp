#pragma once
#include "Engine/Math/Vec2.hpp"



struct Manifold2D
{
public:
	Vec2	normal			= Vec2::ZERO;
	Vec2	contactPoint	= Vec2::ZERO;
	float	penetration		= 0.0f; 
};