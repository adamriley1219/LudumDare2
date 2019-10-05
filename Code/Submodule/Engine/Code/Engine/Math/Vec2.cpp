#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>


//--------------------------------------------------------------------------
const Vec2 Vec2::ZERO	( 0.0f, 0.0f );
const Vec2 Vec2::ONE	( 1.0f, 1.0f );

const Vec2 Vec2::RIGHT	( 1.0f , 0.0f  );
const Vec2 Vec2::LEFT	( -1.0f, 0.0f  );
const Vec2 Vec2::UP		( 0.0f , 1.0f  );
const Vec2 Vec2::DOWN	( 0.0f , -1.0f );

const Vec2 Vec2::ALIGN_TOP			( 0.5f, 1.0 );
const Vec2 Vec2::ALIGN_CENTERED		( 0.5f, 0.5f );
const Vec2 Vec2::ALIGN_BOTTOM		( 0.5f, 0.0f );

const Vec2 Vec2::ALIGN_TOP_LEFT		( 0.0f, 1.0f );
const Vec2 Vec2::ALIGN_CENTER_LEFT	( 0.0f, 0.5f );
const Vec2 Vec2::ALIGN_BOTTOM_LEFT	( 0.0f, 0.0f );

const Vec2 Vec2::ALIGN_TOP_RIGHT	( 1.0f, 1.0f );
const Vec2 Vec2::ALIGN_CENTER_RIGHT	( 1.0f, 0.5f );
const Vec2 Vec2::ALIGN_BOTTOM_RIGHT	( 1.0f, 0.0f );



//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
		: x( copy.x )
	, y( copy.y )
{
}
 

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}


//--------------------------------------------------------------------------
/**
* Vec2
*/
Vec2::Vec2()
{
	x = 0.0f;
	y = 0.0f;
}

//--------------------------------------------------------------------------
/**
* Vec2
*/
Vec2::Vec2( const char* text )
{
	SetFromText( text );
}

//--------------------------------------------------------------------------
/**
* Vec2
*/
Vec2::Vec2( const Vec3& toCopy )
{
	x = toCopy.x;
	y = toCopy.y;
}

//--------------------------------------------------------------------------
/**
* Vec2
*/
Vec2::Vec2( const IntVec2& toCopy )
{
	x = (float) toCopy.x;
	y = (float) toCopy.y;
}

//--------------------------------------------------------------------------
/**
* Vec2
*/
Vec2::Vec2( const Vec4& toCopy )
{
	x = toCopy.x;
	y = toCopy.y;
}

//--------------------------------------------------------------------------
/**
* MakeFromPolarDegrees
*/
const Vec2 Vec2::MakeFromPolarDegrees( float directionDegrees, float length /*= 1.f */ )
{
	return Vec2( length * CosDegrees( directionDegrees ), length * SinDegrees( directionDegrees ) );
}

//--------------------------------------------------------------------------
/**
* MakeFromPolarRadians
*/
const Vec2 Vec2::MakeFromPolarRadians( float RadiansDegrees, float length /*= 1.f */ )
{
	return Vec2::MakeFromPolarDegrees( ConvertRadiansToDegrees( RadiansDegrees ), length );
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator+( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y ); // #MP1Fixme
}


//--------------------------------------------------------------------------
/**
* operator+
*/
const Vec2 Vec2::operator+( float add ) const
{
	return Vec2( x + add, y + add );
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y ); // #MP1Fixme
}

//--------------------------------------------------------------------------
/**
* operator-
*/
const Vec2 Vec2::operator-( float subtract ) const
{
	return Vec2( x - subtract, y - subtract );
}

//--------------------------------------------------------------------------
/**
* RotateDegreesRotateDegrees
*/
void Vec2::RotateDegrees( float degrees )
{
	float theta = ATan2Degrees( y, x );
	float radius = GetLength();
	theta = theta + degrees;
	x = radius * CosDegrees( theta );
	y = radius * SinDegrees( theta );
}

//--------------------------------------------------------------------------
/**
* RotateRadians
*/
void Vec2::RotateRadians( float radians )
{
	float theta = ATan2Degrees( y, x );
	float radius = GetLength();
	theta = theta + ConvertRadiansToDegrees( radians );
	x = radius * CosDegrees( theta );
	y = radius * SinDegrees( theta );
}

//--------------------------------------------------------------------------
/**
* Rotate90Degrees
*/
void Vec2::Rotate90Degrees()
{
	float oldX = x;
	x = -y;
	y = oldX;
}

//--------------------------------------------------------------------------
/**
* RotateMinus90Degrees
*/
void Vec2::RotateMinus90Degrees()
{
	float oldX = x;
	x = y;
	y = -oldX;
}

//--------------------------------------------------------------------------
/**
* GetClamped
*/
const Vec2 Vec2::GetClamped( float maxLength ) const
{
	if( GetLength() > maxLength )
	{
		float theta = ATan2Degrees( y, x );
		return MakeFromPolarDegrees( theta, maxLength );
	}
	return Vec2( x, y );
}

//--------------------------------------------------------------------------
/**
* GetNormalized
*/
const Vec2 Vec2::GetNormalized() const
{
	float length = GetLength();
	return Vec2( x / length, y / length );
}

//--------------------------------------------------------------------------
/**
* SetLength
*/
void Vec2::SetLength( float newLength )
{
	if( newLength == 0 )
	{
		x = 0.f;
		y = 0.f;
		return;
	}

	float length = GetLength();
	ASSERT_OR_DIE( length != 0.0f, "Deviding by 0 while attempting to call Vec2::SetLength" );
	float toScale = newLength / length;
	x *= toScale;
	y *= toScale;
}

//--------------------------------------------------------------------------
/**
* ClampLength
*/
void Vec2::ClampLength( float maxLength )
{
	if( GetLength() > maxLength )
	{
		float theta = ATan2Degrees( y, x );
		Vec2 newVec = MakeFromPolarDegrees( theta, maxLength );
		x = newVec.x;
		y = newVec.y;
	}
}

//--------------------------------------------------------------------------
/**
* RotateDegreesGetLength
*/
float Vec2::GetLength() const
{
	return sqrt( GetLengthSquared() );
}

//--------------------------------------------------------------------------
/**
* GetLengthSquared
*/
float Vec2::GetLengthSquared() const
{
	return  x * x + y * y ;
}

//--------------------------------------------------------------------------
/**
* GetAngleDegrees
*/
float Vec2::GetAngleDegrees() const
{
	return ATan2Degrees( y, x );
}

//--------------------------------------------------------------------------
/**
* GetAngleRadians
*/
float Vec2::GetAngleRadians() const
{
	return ConvertDegreesToRadians( GetAngleDegrees() );
}

//--------------------------------------------------------------------------
/**
* GetRotated90Degrees
*/
const Vec2 Vec2::GetRotated90Degrees() const
{
	return Vec2( -y, x );
}

//--------------------------------------------------------------------------
/**
* GetRotatedMinus90Degrees
*/
const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	return Vec2( y, -x );
}

//--------------------------------------------------------------------------
/**
* GetRotatedDegrees
*/
const Vec2 Vec2::GetRotatedDegrees( float rotateDegrees ) const
{
	float theta = ATan2Degrees( y, x );
	float radius = GetLength();
	theta = theta + rotateDegrees;
	return Vec2( radius * CosDegrees( theta ), radius * SinDegrees( theta ));
}

//--------------------------------------------------------------------------
/**
* GetRotatedRadians
*/
const Vec2 Vec2::GetRotatedRadians( float rotateRadians ) const
{
	float theta = ATan2Degrees( y, x );
	float radius = GetLength();
	theta = theta + ConvertRadiansToDegrees( rotateRadians );
	return Vec2( radius * CosDegrees( theta ), radius * SinDegrees( theta ));}

//--------------------------------------------------------------------------
/**
* SetAngleDegrees
*/
void Vec2::SetAngleDegrees( float angleDegrees )
{
	float length = GetLength();
	x = length * CosDegrees( angleDegrees );
	y = length * SinDegrees( angleDegrees );
}

//--------------------------------------------------------------------------
/**
* SetAngleRadians
*/
void Vec2::SetAngleRadians( float angleRadians )
{
	SetAngleDegrees( ConvertRadiansToDegrees( angleRadians ) );
}

//--------------------------------------------------------------------------
/**
* SetPolarDegrees
*/
void Vec2::SetPolarDegrees( float newAngleDegrees, float newLength )
{
	Vec2 newVec = MakeFromPolarDegrees( newAngleDegrees, newLength );
	x = newVec.x;
	y = newVec.y;
}

//--------------------------------------------------------------------------
/**
* SetPolarRadians
*/
void Vec2::SetPolarRadians( float newAngleRadians, float newLength )
{
	SetPolarDegrees( ConvertRadiansToDegrees( newAngleRadians ), newLength );
}

//--------------------------------------------------------------------------
/**
* SetFromText
*/
void Vec2::SetFromText( const char* text )
{
	std::vector<std::string> splits = SplitStringOnDelitmiter( text, "," );
	GUARANTEE_OR_DIE( (int) splits.size() == 2, Stringf( "Vec2 SetFromText being called with %d splits", (int) splits.size() ) );

	x = StringToFloat( splits[0] );
	y = StringToFloat( splits[1] );
}

//--------------------------------------------------------------------------
/**
* Normalize
*/
void Vec2::Normalize()
{
	if( x == 0.0f && y == 0.0f )
	{
		return;
	}
	float length = GetLength();
	x /= length;
	y /= length;
}

//--------------------------------------------------------------------------
/**
* NormalizeAndGetPreviousLength
*/
float Vec2::NormalizeAndGetOldLength()
{
	float length = GetLength();
	Normalize();
	return length;
}



//--------------------------------------------------------------------------
/**
* ClampMin
*/
const Vec2 Vec2::ClampMin( const Vec2& toClamp, const Vec2& minValues )
{
	Vec2 toRet = toClamp;
	if( toRet.x < minValues.x )
	{
		toRet.x = minValues.x;
	}
	if( toRet.y < minValues.y )
	{
		toRet.y = minValues.y;
	}
	return toRet;
}

//--------------------------------------------------------------------------
/**
* ClampMax
*/
const Vec2 Vec2::ClampMax( const Vec2& toClamp, const Vec2& maxValues )
{
	Vec2 toRet = toClamp;
	if( toRet.x > maxValues.x )
	{
		toRet.x = maxValues.x;
	}
	if( toRet.y > maxValues.y )
	{
		toRet.y = maxValues.y;
	}
	return toRet;
}

//--------------------------------------------------------------------------
/**
* ClampBetween
*/
const Vec2 Vec2::ClampBetween( const Vec2& toClamp, const Vec2& minValues, const Vec2& maxValues )
{
	return ClampMax( ClampMin( toClamp, minValues ), maxValues );
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( x * uniformScale, y * uniformScale ); // #MP1Fixme
}


//--------------------------------------------------------------------------
/**
* operator*
*/
const Vec2 Vec2::operator*( Vec2 scale ) const
{
	return Vec2( x * scale.x, y * scale.y );
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2( x / inverseScale, y / inverseScale ); // #MP1Fixme
}


//--------------------------------------------------------------------------
/**
* operator-=
*/
void Vec2::operator-=( float subtract )
{
	x -= subtract;
	y -= subtract;
}

//--------------------------------------------------------------------------
/**
* operator+=
*/
void Vec2::operator+=( float add )
{
	x += add;
	y += add;
}

//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x; // #MP1Fixme
	y += vecToAdd.y; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x; // #MP1Fixme
	y -= vecToSubtract.y; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale; // #MP1Fixme
	y *= uniformScale; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor; // #MP1Fixme
	y /= uniformDivisor; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x; // #MP1Fixme
	y = copyFrom.y; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale ); // #MP1Fixme
}

//--------------------------------------------------------------------------
/**
* ConstructBottomLeft
*/
Vec2 ConstructBottomLeft(const Vec2& vecA, const Vec2& vecB)
{
	return Vec2( GetMin( vecA.x, vecB.x ), GetMin( vecA.y, vecB.y ) );
}

//--------------------------------------------------------------------------
/**
* ConstructTopRight
*/
Vec2 ConstructTopRight(const Vec2& vecA, const Vec2& vecB)
{
	return Vec2( GetMax( vecA.x, vecB.x ), GetMax( vecA.y, vecB.y ) );
}

//--------------------------------------------------------------------------
/**
* ConstructTopLeft
*/
Vec2 ConstructTopLeft(const Vec2& vecA, const Vec2& vecB)
{
	return Vec2( GetMin( vecA.x, vecB.x ), GetMax( vecA.y, vecB.y ) );
}

//--------------------------------------------------------------------------
/**
* ConstructBottomRight
*/
Vec2 ConstructBottomRight(const Vec2& vecA, const Vec2& vecB)
{
	return Vec2( GetMax( vecA.x, vecB.x ), GetMin( vecA.y, vecB.y ) );
}

//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	return x == compare.x && y == compare.y; // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	return  !( x == compare.x && y == compare.y ); // #MP1Fixme
}

