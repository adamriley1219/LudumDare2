#pragma once

struct Vec3;

//-----------------------------------------------------------------------------------------------
struct Vec4
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

public:
	// Construction/Destruction
	~Vec4() {}												// destructor: do nothing (for speed)
	Vec4() {}												// default constructor: do nothing (for speed)
	explicit Vec4( float initialX, float initialY, float initialZ, float initialW );		// explicit constructor (from x, y)
	explicit Vec4( Vec3 init3, float initialW );		
															// Operators

	const static Vec4 ZERO;
	const static Vec4 ONE;

public:
	const Vec4 operator-( const Vec4& vecToSubtract ) const;	
	const Vec4 operator*( const float uniformScale ) const;				
	const Vec4 operator/( const float uniformScale ) const;				

	void operator*=( const float uniformScale );				


private:
};