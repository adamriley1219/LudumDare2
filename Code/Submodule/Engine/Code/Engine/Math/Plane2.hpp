#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

#pragma warning(disable:4201)

struct Plane2
{
public:
	union 
	{ 
		Vec3 m_data; 

		struct 
		{ 
			Vec2 m_normal; 
			float m_distance; // distance along normal to get to the origin; 
		} capsilated_data;
	};
public:
	Plane2();
	Plane2( const Plane2& copy );
	Plane2& operator= (Plane2 const &other ) { m_data = other.m_data; return *this; }
	~Plane2();
	explicit Plane2( Vec3 data );
	explicit Plane2( Vec2 normal, float distanceToOrigin );
		
	// positive -> in front of the plane
	// negative -> behind the plane; 
	float GetDistance( Vec2 point ) const; 
	inline bool IsInFront( Vec2 point ) const { return (GetDistance(point) > 0.0f); }
	inline bool IsBehind( Vec2 point ) const { return (GetDistance(point) < 0.0f); }

	// Not needed for this class, but useful
	// vec2 GetProjectedPoint( vec2 point ) const; 

public:
	// named constructors;
	static Plane2 AtPosition( Vec2 pointOnPlane, Vec2 normal ); 
	static Plane2 FromPointsLH( Vec2 p0, Vec2 p1 ); 
};
