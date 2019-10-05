#pragma once
#include "Engine/Math/Vec3.hpp"


struct Ray3
{
public:
	Vec3 m_start = Vec3::ZERO;
	Vec3 m_dir = Vec3::FORWARD;

public:
	Ray3();
	Ray3( const Vec3& start, const Vec3& direction );

	Vec3 GetPointFromTime( float time ) const;

	static Ray3 FromPoints( Vec3 start, Vec3 end ); 

};