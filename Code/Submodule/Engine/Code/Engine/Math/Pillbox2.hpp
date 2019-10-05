#pragma once
#include "Engine/Math/OBB2.hpp"

struct Transform2D;

//-----------------------------------------------------------------------------------------------
struct Pillbox2
{
public: 
	OBB2 m_obb;
	float m_radius = 0.0f;


public:
	~Pillbox2();		
	Pillbox2();
	explicit Pillbox2( Vec2 center,
		float radius = 0.0f,
		Vec2 exstents = Vec2::ZERO, 
		float rotationDegrees = 0.0f );												

public:
	Pillbox2	GetTransformed( const Transform2D& transform ) const;
	float		GetArea() const;
	float		GetWidth() const;
	float		GetHeight() const;

private:

};