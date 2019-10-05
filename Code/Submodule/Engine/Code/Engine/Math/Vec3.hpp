#pragma once

struct Vec4;
struct Vec2;

//-----------------------------------------------------------------------------------------------
struct Vec3
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

public:
	// Construction/Destruction
	~Vec3() {}												// destructor: do nothing (for speed)
	Vec3() {}												// default constructor: do nothing (for speed)
	Vec3( const Vec3& copyFrom );							// copy constructor (from another Vec3)
	explicit Vec3( float initialX, float initialY, float initialZ );		// explicit constructor (from x, y)
	explicit Vec3( Vec2 vec2, float inZ );
	explicit Vec3( Vec4 vec4 );
	explicit Vec3( const char* text );
	
	const static Vec3 ZERO;
	const static Vec3 ONE;
	const static Vec3 FORWARD;
	const static Vec3 UP;
	const static Vec3 RIGHT;
	
	// Operators
	const Vec3 operator+( const Vec3& vecToAdd ) const;			// Vec3 + Vec3
	const Vec3 operator-( const Vec3& vecToSubtract ) const;	// Vec3 - Vec3
	const Vec3 operator*( float uniformScale ) const;			// Vec3 * float
	const Vec3 operator/( float inverseScale ) const;			// Vec3 / float
	void operator+=( const Vec3& vecToAdd );					// Vec3 += Vec3
	void operator-=( const Vec3& vecToSubtract );				// Vec3 -= Vec3
	void operator*=( const float uniformScale );				// Vec3 *= float
	void operator/=( const float uniformDivisor );				// Vec3 /= float
	void operator=( const Vec3& copyFrom );						// Vec3 = Vec3
	void operator=( const Vec4& copyFrom );						// Vec3 = Vec3
	bool operator==( const Vec3& compare ) const;				// Vec3 == Vec3
	bool operator!=( const Vec3& compare ) const;				// Vec3 != Vec3


	friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * Vec3
	
	float GetLength() const;
	float GetLengthSquared() const;
	float GetLengthXY() const;
	float GetLengthXYSquared() const;
	float GetAngleAboutZDegrees() const;
	float GetAngleAboutZRadians() const;
	const Vec3 GetRotatedAboutZDegrees( float rotateDegrees ) const;
	const Vec3 GetRotatedAboutXDegrees( float rotateDegrees ) const;
	const Vec3 GetRotatedAboutYDegrees( float rotateDegrees ) const;
	const Vec3 GetRotatedAboutZRadians( float rotateRadians ) const;

	void Normalize();
	Vec3 GetNormalized() const;

	void SetFromText( const char* text );
	void SetLength( float newLength );


	Vec3 Cross( Vec3 other ) const;
private:
};

