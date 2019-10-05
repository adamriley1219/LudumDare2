#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"

#pragma warning(disable:4201)

struct Plane3
{
public:
	union 
	{ 
		Vec4 m_data; 

		struct 
		{ 
			Vec3 m_normal; 
			float m_distance; // distance along normal to get to the origin; 
		} capsilated_data;
	};
public:
	Plane3();
	Plane3( const Plane3& copy );
	Plane3& operator= (Plane3 const &other ) { m_data = other.m_data; return *this; }
	~Plane3();
	explicit Plane3( const Vec4& data );
	explicit Plane3( const Vec3& normal, float distanceToOrigin );
		
	// positive -> in front of the plane
	// negative -> behind the plane; 
	float GetDistance( const Vec3& point ) const; 
	inline bool IsInFront( const Vec3& point ) const { return (GetDistance(point) > 0.0f); }
	inline bool IsInFrontWithBuffer( const Vec3& point, float buffer ) const { return (GetDistance(point) > -buffer); }
	inline bool IsBehind( const Vec3& point ) const { return (GetDistance(point) < 0.0f); }
	inline bool IsBehindWithBuffer( const Vec3& point, float buffer ) const { return (GetDistance(point) < buffer); }

	// Not needed for this class, but useful
	// vec2 GetProjectedPoint( vec2 point ) const; 

public:
	// named constructors;
	static Plane3 AtPosition( const Vec3& pointOnPlane, const Vec3& normal ); 
	static Plane3 FromPointsLH( const Vec3& p0, const Vec3& p1, const Vec3& p2 ); 
};
