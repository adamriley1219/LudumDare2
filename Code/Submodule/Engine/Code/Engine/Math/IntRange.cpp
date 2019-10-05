#include "Engine/Math/IntRange.hpp"
#include "Engine/core/EngineCommon.hpp"
#include <vector>


//--------------------------------------------------------------------------
/**
* IntRange
*/
IntRange::IntRange( int min, int max )
{
	this->min = min; 
	this->max = max; 
}

//--------------------------------------------------------------------------
/**
* IntRange
*/
IntRange::IntRange( const char* text )
{
	SetFromText( text );
}

//--------------------------------------------------------------------------
/**
* IntRange
*/
IntRange::IntRange( int minAndMax )
{
	this->min = minAndMax; 
	this->max = minAndMax; 
}

//--------------------------------------------------------------------------
/**
* SetFromText
*/
void IntRange::SetFromText( const char* text )
{
	std::vector< std::string > splits = SplitStringOnDelitmiter( text, "~" );
	

	if( (int) splits.size() == 2 )
	{
		min = StringToInt( splits[0] );
		max = StringToInt( splits[1] );
	}
	else if( (int) splits.size() == 1 )
	{
		min = StringToInt( splits[0] );
		max = StringToInt( splits[0] );
	}
	else
	{
		ERROR_AND_DIE( Stringf( "IntRange SetFromText had a split of size %d", (int) splits.size() ) );
	}
}
