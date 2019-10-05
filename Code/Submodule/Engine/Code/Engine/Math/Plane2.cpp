#include "Engine/Math/Plane2.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------
/**
* Plane2
*/
Plane2::Plane2()
	: m_data( 0.0f, 1.0f, 0.0f )
{

}

//--------------------------------------------------------------------------
/**
* Plane2
*/
Plane2::Plane2( Vec3 data )
	: m_data(data)
{

}

//--------------------------------------------------------------------------
/**
* Plane2
*/
Plane2::Plane2( Vec2 normal, float distanceToOrigin )
	: m_data( normal, distanceToOrigin )
{

}

//--------------------------------------------------------------------------
/**
* Plane2
*/
Plane2::Plane2( const Plane2& copy )
	: m_data( copy.m_data )
{

}

//--------------------------------------------------------------------------
/**
* GetDistance
*/
float Plane2::GetDistance( Vec2 point ) const
{
	return DotProduct( point, capsilated_data.m_normal ) + capsilated_data.m_distance;
}

//--------------------------------------------------------------------------
/**
* ~Plane2
*/
Plane2::~Plane2()
{

}

//--------------------------------------------------------------------------
/**
* AtPosition
*/
Plane2 Plane2::AtPosition( Vec2 pointOnPlane, Vec2 normal )
{
	Plane2 p; 
	p.capsilated_data.m_normal = normal; 
	p.capsilated_data.m_distance = -DotProduct( pointOnPlane, normal );  
	return p;
}

//--------------------------------------------------------------------------
/**
* FromPointsLH
*/
Plane2 Plane2::FromPointsLH( Vec2 p0, Vec2 p1 )
{
	Vec2 disp = p1 - p0;
	Vec2 norm = disp.GetRotated90Degrees();
	norm.Normalize();

	Plane2 p; 
	p.capsilated_data.m_normal = norm; 
	p.capsilated_data.m_distance = -DotProduct( p0, norm );  
	return p;
}
