#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <cmath>
#include <vector>

//--------------------------------------------------------------------------
const Vec3 Vec3::ZERO	( 0.0f, 0.0f, 0.0f );
const Vec3 Vec3::ONE	( 1.0f, 1.0f, 1.0f );
const Vec3 Vec3::FORWARD( 0.0f, 0.0f, 1.0f );
const Vec3 Vec3::UP		( 0.0f, 1.0f, 0.0f );
const Vec3 Vec3::RIGHT	( 1.0f, 0.0f, 0.0f );

Vec3::Vec3( const Vec3& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
	, z( copyFrom.z )
{

}

Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{

}

//--------------------------------------------------------------------------
/**
* Vec3
*/
Vec3::Vec3( const char* text )
{
	SetFromText( text );
}

//--------------------------------------------------------------------------
/**
* Vec3
*/
Vec3::Vec3( Vec4 vec4 )
{
	x = vec4.x;
	y = vec4.y;
	z = vec4.z;
}

//--------------------------------------------------------------------------
/**
* Vec3
*/
Vec3::Vec3( Vec2 vec2, float inZ )
{
	x = vec2.x;
	y = vec2.y;
	z = inZ;
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator+( const Vec3& vecToAdd ) const
{
	return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	return Vec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}

//--------------------------------------------------------------------------
/**
* operator=
*/
void Vec3::operator=( const Vec4& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
	return Vec3( x * uniformScale, y * uniformScale, z * uniformScale );
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
	return Vec3( x / inverseScale, y / inverseScale, z / inverseScale );
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor; 
	y /= uniformDivisor; 
	z /= uniformDivisor;
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
	return x == compare.x && y == compare.y && z == compare.z; 
}

//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
	return ! ( x == compare.x && y == compare.y && z == compare.z );
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{	
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

//-----------------------------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	return Vec3( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale ); // #MP1Fixme
}

//--------------------------------------------------------------------------
/**
* RotateDegreesGetLength
*/
float Vec3::GetLength() const
{
	return sqrt( GetLengthSquared() );
}

//--------------------------------------------------------------------------
/**
* GetRotatedAboutZRadiansGetLengthSquared
*/
float Vec3::GetLengthSquared() const
{
	return  x * x + y * y + z * z;
}

//--------------------------------------------------------------------------
/**
* NormalizeGetLengthXY
*/
float Vec3::GetLengthXY() const
{
	return  sqrt( GetLengthXYSquared() );
}

//--------------------------------------------------------------------------
/**
* GetLengthXYSquared
*/
float Vec3::GetLengthXYSquared() const
{
	return x * x + y * y;
}

//--------------------------------------------------------------------------
/**
* GetLengthXYGetAngleAboutZDegrees
*/
float Vec3::GetAngleAboutZDegrees() const
{
	return ATan2Degrees( y, x );
}

//--------------------------------------------------------------------------
/**
* GetAngleAboutZDegreesGetAngleAboutZRadians
*/
float Vec3::GetAngleAboutZRadians() const
{
	return ConvertDegreesToRadians( GetAngleAboutZDegrees() );
}

//--------------------------------------------------------------------------
/**
* GetRotatedAboutZDegrees
*/
const Vec3 Vec3::GetRotatedAboutZDegrees( float rotateDegrees ) const
{
	float theta = GetAngleAboutZDegrees() + rotateDegrees;
	float length = GetLengthXY();
	Vec3 newVec;
	newVec.x = length * CosDegrees( theta );
	newVec.y = length * SinDegrees( theta );
	newVec.z = z;
	return newVec;
}

//--------------------------------------------------------------------------
/**
* GetRotatedAboutXDegrees
*/
const Vec3 Vec3::GetRotatedAboutXDegrees( float rotateDegrees ) const
{
	Matrix44 mat = Matrix44::MakeXRotationDegrees( rotateDegrees );
	return mat.TransformPosition3D( *this );
}

//--------------------------------------------------------------------------
/**
* GetRotatedAboutYDegrees
*/
const Vec3 Vec3::GetRotatedAboutYDegrees( float rotateDegrees ) const
{
	Matrix44 mat = Matrix44::MakeYRotationDegrees( rotateDegrees );
	return mat.TransformPosition3D( *this );
}

//--------------------------------------------------------------------------
/**
* GetRotatedAboutZRadians
*/
const Vec3 Vec3::GetRotatedAboutZRadians( float rotateRadians ) const
{
	return GetRotatedAboutZDegrees( ConvertRadiansToDegrees( rotateRadians ) );
}

//--------------------------------------------------------------------------
/**
* Normalize
*/
void Vec3::Normalize()
{
	if( x == 0.0f && y == 0.0f && z == 0.0f )
	{
		return;
	}
	float length = GetLength();
	x /= length;
	y /= length;
	z /= length;
}

//--------------------------------------------------------------------------
/**
* GetNormalized
*/
Vec3 Vec3::GetNormalized() const
{
	Vec3 vec = *this;
	vec.Normalize();
	return vec;
}

//--------------------------------------------------------------------------
/**
* SetFromText
*/
void Vec3::SetFromText( const char* text )
{
	std::vector<std::string> splits = SplitStringOnDelitmiter( text, "," );
	GUARANTEE_OR_DIE( (int) splits.size() == 3, Stringf( "Vec3 SetFromText being called with %d splits", (int) splits.size() ) );

	x = StringToFloat( splits[0] );
	y = StringToFloat( splits[1] );
	y = StringToFloat( splits[2] );
}

//--------------------------------------------------------------------------
/**
* SetLength
*/
void Vec3::SetLength( float newLength )
{
	if( newLength == 0 )
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
		return;
	}

	float length = GetLength();
	ASSERT_OR_DIE( length != 0.0f, "Deviding by 0 while attempting to call Vec3::SetLength" );
	float toScale = newLength / length;
	x *= toScale;
	y *= toScale;
	z *= toScale;
}

//--------------------------------------------------------------------------
/**
* Cross
* https://docs.microsoft.com/en-us/previous-versions/windows/desktop/bb324332(v%3Dvs.85)/
*/
Vec3 Vec3::Cross( Vec3 other ) const
{
	return Vec3(
		y * other.z - z * other.y,
		z * other.x - x * other.z,
		x * other.y - y * other.x
	);
}
