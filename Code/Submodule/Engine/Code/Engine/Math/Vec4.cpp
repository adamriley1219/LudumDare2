#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <cmath>
#include <vector>

//--------------------------------------------------------------------------
const Vec4 Vec4::ZERO	( 0.0f, 0.0f, 0.0f, 0.0f );
const Vec4 Vec4::ONE	( 1.0f, 1.0f, 1.0f, 1.0f );


//--------------------------------------------------------------------------

Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
	, w( initialW )
{

}

//--------------------------------------------------------------------------
/**
* Vec4
*/
Vec4::Vec4( Vec3 init3, float initialW )
	: x( init3.x )
	, y( init3.y )
	, z( init3.z )
	, w( initialW )
{
	
}

//--------------------------------------------------------------------------
/**
* operator-
*/
const Vec4 Vec4::operator-( const Vec4& vecToSubtract ) const
{
	return Vec4( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w );
}

//--------------------------------------------------------------------------
/**
* operator*
*/
const Vec4 Vec4::operator*( const float uniformScale ) const
{
	return Vec4( x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale );
}

//--------------------------------------------------------------------------
/**
* operator*=
*/
void Vec4::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}

//--------------------------------------------------------------------------
/**
* operator/
*/
const Vec4 Vec4::operator/( const float uniformScale ) const
{
	float mul = 1.0f / uniformScale;
	return Vec4( x * mul, y * mul, z * mul, w * mul );
}

