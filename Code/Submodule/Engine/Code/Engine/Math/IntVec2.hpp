#pragma once

struct Vec2;

struct IntVec2
{
public:
	int x = 0;
	int y = 0;

public:
	IntVec2() {}
	explicit IntVec2( int x, int y );
	explicit IntVec2( Vec2 vec );
	explicit IntVec2( const char* text );
	~IntVec2(){}

	const static IntVec2 ZERO;
	const static IntVec2 ONE;
	const static IntVec2 NEG_ONE;

	void SetFromText( const char* text );

public:
	const IntVec2 operator-( const IntVec2& vecToSubtract ) const;
	void operator=( const IntVec2& copyFrom );						// vec2 = vec2
	bool operator==( const IntVec2& compare ) const;				// vec2 == vec2
	bool operator!=( const IntVec2& compare ) const;				// vec2 == vec2
};
