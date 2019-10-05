#include "Engine/Physics/Transform3D.hpp"
#include "Engine/Math/Matrix44.hpp"

//--------------------------------------------------------------------------
/**
* Transform3D
*/
Transform3D::Transform3D( const Vec3& position, const Vec3& rotationDegrees /*= Vec3::ZERO*/, const Vec3& scale /*= Vec3::ONE */ )
{
	m_position = position;
	m_scale = scale;
	m_rotation = rotationDegrees;
}

//--------------------------------------------------------------------------
/**
* GetTransformMatrix
*/
Matrix44 Transform3D::GetTransformMatrix() const
{
	return Matrix44::MakeTranslation3D( m_position ) * Matrix44::MakeScale3D( m_scale ) * Matrix44::FromEuler( m_rotation, ROTATION_ORDER_ZXY );
}

//--------------------------------------------------------------------------
/**
* GetForward
*/
Vec3 Transform3D::GetForward() const
{
	return Vec3( GetTransformMatrix().GetK() );
}

//--------------------------------------------------------------------------
/**
* GetUp
*/
Vec3 Transform3D::GetUp() const
{
	return Vec3( GetTransformMatrix().GetJ() );
}

//--------------------------------------------------------------------------
/**
* GetRight
*/
Vec3 Transform3D::GetRight() const
{
	return Vec3( GetTransformMatrix().GetI() );
}

//--------------------------------------------------------------------------
/**
* operator=
*/
void Transform3D::operator=( const Transform3D& copyFrom )
{
	m_position	= copyFrom.m_position;
	m_rotation	= copyFrom.m_rotation;
	m_scale		= copyFrom.m_scale;
}
