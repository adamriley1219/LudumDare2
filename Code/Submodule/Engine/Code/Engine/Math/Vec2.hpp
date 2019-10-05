#pragma once

struct Vec3;
struct Vec4;
struct IntVec2;


//-----------------------------------------------------------------------------------------------
struct Vec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;

public:
	// Construction/Destruction
	~Vec2() {}												// destructor: do nothing (for speed)
	Vec2();												// default constructor: do nothing (for speed)
	Vec2( const Vec2& copyFrom );							// copy constructor (from another vec2)
	explicit Vec2( float initialX, float initialY );		// explicit constructor (from x, y)
	explicit Vec2( const char* text );
	explicit Vec2( const Vec3& toCopy );
	explicit Vec2( const Vec4& toCopy );
	explicit Vec2( const IntVec2& toCopy );

	// Static constructors
	static const Vec2 MakeFromPolarDegrees( float directionDegrees, float length  = 1.f );
	static const Vec2 MakeFromPolarRadians( float RadiansDegrees, float length  = 1.f );

	// const static members
	const static Vec2 ZERO;
	const static Vec2 ONE;
	const static Vec2 RIGHT;
	const static Vec2 LEFT;
	const static Vec2 UP;
	const static Vec2 DOWN;

	const static Vec2 ALIGN_TOP;
	const static Vec2 ALIGN_CENTERED;
	const static Vec2 ALIGN_BOTTOM;

	const static Vec2 ALIGN_TOP_LEFT;
	const static Vec2 ALIGN_CENTER_LEFT;
	const static Vec2 ALIGN_BOTTOM_LEFT;

	const static Vec2 ALIGN_TOP_RIGHT;
	const static Vec2 ALIGN_CENTER_RIGHT;
	const static Vec2 ALIGN_BOTTOM_RIGHT;


	// Math applications of Vec2
	float GetLength() const;
	float GetLengthSquared() const;
	float GetAngleDegrees() const;
	float GetAngleRadians() const;

	const Vec2 GetRotated90Degrees() const;
	const Vec2 GetRotatedMinus90Degrees() const;
	const Vec2 GetRotatedDegrees( float rotateDegrees ) const;
	const Vec2 GetRotatedRadians( float rotateRadians ) const;

	void SetAngleDegrees( float angleDegrees );
	void SetAngleRadians( float angleRadians );
	void SetPolarDegrees( float newAngleDegrees, float newLength );
	void SetPolarRadians( float newAngleRadians, float newLength );
	void SetFromText( const char* text );

	void RotateDegrees( float rotateDegrees );
	void RotateRadians( float rotateRadians );
	void Rotate90Degrees();
	void RotateMinus90Degrees();

	const Vec2 GetClamped( float maxLength ) const;
	const Vec2 GetNormalized() const;
	void SetLength( float newLength );
	void ClampLength( float maxLength );
	void Normalize();
	float NormalizeAndGetOldLength();

	static const Vec2 ClampMin( const Vec2& toClamp, const Vec2& minValues ); // Clamps toClamp to have values that are minimized by the corresponding values in minValues.
	static const Vec2 ClampMax( const Vec2& toClamp, const Vec2& maxValues ); // Clamps toClamp to have values that are maximized by the corresponding values in maxValues.
	static const Vec2 ClampBetween( const Vec2& toClamp, const Vec2& minValues, const Vec2& maxValues ); // Clamps toClamp between minValues and maxValues.

	friend const Vec2 operator*( float uniformScale, const Vec2& vecToScale );	// float * vec2


public:
	// Operator definitions
	const Vec2 operator+( const Vec2& vecToAdd ) const;			// vec2 + vec2
	const Vec2 operator+( float add ) const;			// vec2 + vec2
	const Vec2 operator-( const Vec2& vecToSubtract ) const;	// vec2 - vec2
	const Vec2 operator-( float subtract ) const;	// vec2 - vec2
	const Vec2 operator*( float uniformScale ) const;			// vec2 * float
	const Vec2 operator*( Vec2 scale ) const;			// vec2 * float
	const Vec2 operator/( float inverseScale ) const;			// vec2 / float
	void operator+=( const Vec2& vecToAdd );					// vec2 += vec2
	void operator+=( float add );					// vec2 += vec2
	void operator-=( const Vec2& vecToSubtract );				// vec2 -= vec2
	void operator-=( float subtract );				// vec2 -= vec2
	void operator*=( const float uniformScale );				// vec2 *= float
	void operator/=( const float uniformDivisor );				// vec2 /= float
	void operator=( const Vec2& copyFrom );						// vec2 = vec2
	bool operator==( const Vec2& compare ) const;				// vec2 == vec2
	bool operator!=( const Vec2& compare ) const;	
};


Vec2 ConstructBottomLeft( const Vec2& vecA, const Vec2& vecB );
Vec2 ConstructTopRight( const Vec2& vecA, const Vec2& vecB );
Vec2 ConstructTopLeft( const Vec2& vecA, const Vec2& vecB );
Vec2 ConstructBottomRight( const Vec2& vecA, const Vec2& vecB );