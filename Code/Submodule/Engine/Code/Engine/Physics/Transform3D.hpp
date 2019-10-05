#pragma once
#include "Engine/Math/Vec3.hpp"

struct Matrix44;

struct Transform3D
{
public:
	Vec3 m_rotation = Vec3::ZERO;
	Vec3 m_position = Vec3::ZERO;
	Vec3 m_scale = Vec3::ONE;

public:
	Transform3D(){};
	explicit Transform3D( const Vec3& position, const Vec3& rotationDegrees = Vec3::ZERO, const Vec3& scale = Vec3::ONE );

public:
	Matrix44 GetTransformMatrix() const;
	Vec3 GetForward() const;
	Vec3 GetUp() const;
	Vec3 GetRight() const;

public:
	void operator=( const Transform3D& copyFrom );	

};

