#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Matrix44.hpp"

struct AABB3;
struct Transform2D;

//    6-----7
//   /|    /|
//  2-----3 |
//  | 4---|-5
//  |/    |/
//  0-----1

struct OBB3
{
public:
	Vec3 m_extents			= Vec3::ONE; 
	Matrix44 m_transform	= Matrix44::IDENTITY;

public:
	OBB3();
	~OBB3();
	explicit OBB3( const Vec3& center, const Vec3& exstents, const Vec3& rotation );
	explicit OBB3( const AABB3& box );
	
	void GetCorners( Vec3* corners ) const;

private:
	void GetCornersInLocalSpace( Vec3* corners ) const;

};
