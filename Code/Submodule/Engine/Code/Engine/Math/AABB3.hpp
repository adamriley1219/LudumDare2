#pragma once
#include "Engine/Math/Vec3.hpp"

//    6-----7
//   /|    /|
//  2-----3 |
//  | 4---|-5
//  |/    |/
//  0-----1

struct AABB3
{
public:
	Vec3 m_maxs;
	Vec3 m_mins;

	const static AABB3 ONE_BY_ONE_BY_ONE;

public:
	AABB3();
	explicit AABB3( float width, float height, float depth, Vec3 center = Vec3::ZERO );
	explicit AABB3( const Vec3& mins, const Vec3& maxs );

public:
	void GetCorners( Vec3* corners );

};
