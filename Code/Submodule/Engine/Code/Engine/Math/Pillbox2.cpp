#include "Engine/Math/Pillbox2.hpp"
#include "Engine/Physics/Transform2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------
/**
* ~Pillbox2
*/
Pillbox2::~Pillbox2()
{

}

//--------------------------------------------------------------------------
/**
* Pillbox2D
*/
Pillbox2::Pillbox2( Vec2 theCenter, float theRadius /*= 0.0f*/, Vec2 theExstents /*= Vec2::ZERO*/, float theRotationDegrees /*= 0.0f */ )
	: m_radius( theRadius )
{
	m_obb = OBB2( theCenter, theExstents, theRotationDegrees );
}

//--------------------------------------------------------------------------
/**
* Pillbox2
*/
Pillbox2::Pillbox2()
{

}

//--------------------------------------------------------------------------
/**
* GetTransformed
*/
Pillbox2 Pillbox2::GetTransformed( const Transform2D& transform ) const
{
	return Pillbox2( m_obb.m_center + transform.m_position, m_radius * GetMax( transform.m_scale.x, transform.m_scale.y ), m_obb.m_extents * transform.m_scale, m_obb.m_right.GetAngleDegrees() + transform.m_rotation );
}

//--------------------------------------------------------------------------
/**
* GetArea
*/
float Pillbox2::GetArea() const
{
	// http://mathworld.wolfram.com/RoundedRectangle.html
	float width = m_obb.m_extents.x * 2.0f;
	float height = m_obb.m_extents.y * 2.0f;
	float radius = m_radius;
	float radSqured = radius * radius;

	return width * height + 2.0f * radius * ( width + height ) + g_PI * radSqured;
}

//--------------------------------------------------------------------------
/**
* GetWidth
*/
float Pillbox2::GetWidth() const
{
	return  ( m_obb.m_extents.x + m_radius ) * 2.0f;
}

//--------------------------------------------------------------------------
/**
* GetHeight
*/
float Pillbox2::GetHeight() const
{
	return  ( m_obb.m_extents.y + m_radius ) * 2.0f;
}
