#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Plane2.hpp"


struct Transform2D;

struct OBB2
{
public:
	Vec2 m_right         = Vec2( 1.0f, 0.0f ); 
	Vec2 m_up            = Vec2( 0.0f, 1.0f );   
	Vec2 m_center        = Vec2( 0.0f, 0.0f ); 
	Vec2 m_extents       = Vec2( 0.0f, 0.0f ); 

public:
	OBB2();
	~OBB2();
	explicit OBB2( Vec2 center, Vec2 exstents, float rotation );
	explicit OBB2( AABB2 box );
	
	void Translate( Vec2 offset );
	void MoveTo( Vec2 position ); 

	void RotateBy( float degrees );           // A04
	void SetRotation( float degrees ); 

	Vec2 GetCenter() const; 
	void GetCorners( Vec2 *out ) const; // bl, br, tr, tl
	void GetPlanes( Plane2 *out ) const; 

	// Collision Utility
	Vec2 ToLocalPoint( Vec2 worldPoint ) const; 
	Vec2 ToWorldPoint( Vec2 localPoint ) const; 

	bool Contains( Vec2 worldPoint ) const;
	Vec2 GetClosestPoint( Vec2 worldPoint ) const;

	bool Intersects( OBB2 const &other ) const; 

public:
	Vec2 GetBotLeft() const;
	Vec2 GetBotRight() const;
	Vec2 GetTopLeft() const;
	Vec2 GetTopRight() const;

	inline Vec2 GetRight() const        { return m_right; }
	inline Vec2 GetUp() const           { return m_right.GetRotated90Degrees(); } 

	float GetArea();

};
