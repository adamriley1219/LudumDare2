#pragma once
#include "Engine/Math/Plane3.hpp"

struct Vec3;

enum eFrustumFace {
	FRUSTUM_RIGHT	= 0,
	FRUSTUM_TOP		= 1,
	FRUSTUM_FRONT	= 2, 
	FRUSTUM_LEFT	= 3,
	FRUSTUM_BOTTOM	= 4, 
	FRUSTUM_BACK	= 5, 

	FRUSTUM_SIDE_COUNT,
};

//    6-----7
//   /|    /|
//  2-----3 |
//  | 4---|-5
//  |/    |/
//  0-----1

struct Frustum
{
public:
	Plane3 m_planes[6];
	Vec3 m_corners[8];

public:
	Frustum();
	explicit Frustum( const Vec3* corners );

public:
	void GetCorners( Vec3* corners ) const;

};