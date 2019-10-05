#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------
/**
* ~Plane3
*/
Plane3::~Plane3()
{

}

//--------------------------------------------------------------------------
/**
* Plane3
*/
Plane3::Plane3()
{

}

//--------------------------------------------------------------------------
/**
* Plane3
*/
Plane3::Plane3( const Plane3& copy )
{
	m_data = copy.m_data;
}

//--------------------------------------------------------------------------
/**
* Plane3
*/
Plane3::Plane3( const Vec4& data )
{
	m_data = data;
}

//--------------------------------------------------------------------------
/**
* Plane3
*/
Plane3::Plane3( const Vec3& normal, float distanceToOrigin )
{
	capsilated_data.m_normal = normal.GetNormalized();
	capsilated_data.m_distance = distanceToOrigin;
}

//--------------------------------------------------------------------------
/**
* GetDistance
*/
float Plane3::GetDistance( const Vec3& point ) const
{
	return capsilated_data.m_distance + DotProduct( point, capsilated_data.m_normal );
}

//--------------------------------------------------------------------------
/**
* AtPosition
*/
Plane3 Plane3::AtPosition( const Vec3& pointOnPlane, const Vec3& normal )
{
	Plane3 p; 
	p.capsilated_data.m_normal = normal.GetNormalized(); 
	p.capsilated_data.m_distance = -DotProduct( pointOnPlane, normal );  
	return p;
}

//--------------------------------------------------------------------------
/**
* FromPointsLH
*/
Plane3 Plane3::FromPointsLH( const Vec3& p0, const Vec3& p1, const Vec3& p2 )
{
	Vec3 p0p1 = p1 - p0;
	Vec3 p0p2 = p2 - p0;
	Vec3 norm = Cross( p0p2, p0p1 );
	norm.Normalize();

	Plane3 p; 
	p.capsilated_data.m_normal = norm; 
	p.capsilated_data.m_distance = -DotProduct( p0, norm );  
	return p;
}

