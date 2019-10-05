#include "Engine/Math/FloatRange.hpp"
#include "Engine/core/EngineCommon.hpp"
#include <vector>


//--------------------------------------------------------------------------
/**
* FloatRange
*/
FloatRange::FloatRange( float min, float max )
{
	this->min = min; 
	this->max = max; 
}

//--------------------------------------------------------------------------
/**
* FloatRange
*/
FloatRange::FloatRange( const char* text )
{
	SetFromText( text );
}

//--------------------------------------------------------------------------
/**
* FloatRange
*/
FloatRange::FloatRange( float minMax )
{
	this->min = minMax; 
	this->max = minMax; 
}

//--------------------------------------------------------------------------
/**
* SetFromText
*/
void FloatRange::SetFromText( const char* text )
{
	std::vector< std::string > splits = SplitStringOnDelitmiter( text, "~" );


	if( (int) splits.size() == 2 )
	{
		min = StringToFloat( splits[0] );
		max = StringToFloat( splits[1] );
	}
	else if( (int) splits.size() == 1 )
	{
		min = StringToFloat( splits[0] );
		max = StringToFloat( splits[0] );	
	}
	else
	{
		ERROR_AND_DIE( Stringf( "FloatRange SetFromText had a split of size %d", (int) splits.size() ) );
	}

}
