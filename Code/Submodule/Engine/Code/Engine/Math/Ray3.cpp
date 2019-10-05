#include "Engine/Math/Ray3.hpp"

//--------------------------------------------------------------------------
/**
* Ray3
*/
Ray3::Ray3()
{

}

//--------------------------------------------------------------------------
/**
* Ray3
*/
Ray3::Ray3( const Vec3& start, const Vec3& direction )
	: m_start(start), m_dir(direction.GetNormalized())
{

}

//--------------------------------------------------------------------------
/**
* GetPointFromTime
*/
Vec3 Ray3::GetPointFromTime( float time ) const
{
	return m_start + m_dir * time;
}

//--------------------------------------------------------------------------
/**
* FromPoints
*/
Ray3 Ray3::FromPoints( Vec3 start, Vec3 end )
{
	return Ray3( start, end - start );
}

