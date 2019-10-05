#include "Engine/Physics/Transform2D.hpp"



//--------------------------------------------------------------------------
/**
* Transform2D
*/
Transform2D::Transform2D( const Vec2& position, float rotationDegrees /*= 0.0f*/, const Vec2& scale /*= Vec2::ONE */ )
{
	m_position = position;
	m_scale = scale;
	m_rotation = rotationDegrees;
}

//--------------------------------------------------------------------------
/**
* operator=
*/
void Transform2D::operator=( const Transform2D& copyFrom )
{
	m_position	= copyFrom.m_position;
	m_rotation	= copyFrom.m_rotation;
	m_scale		= copyFrom.m_scale;
}
