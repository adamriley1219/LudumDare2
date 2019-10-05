#pragma once



struct IntRange
{
public:
	int min = 0;
	int max = 0;

public:
	IntRange() {}
	explicit IntRange( int min, int max );
	explicit IntRange( int minAndMax );
	explicit IntRange( const char* text );
	~IntRange(){}

	void SetFromText( const char* text );
};
