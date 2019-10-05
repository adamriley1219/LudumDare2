#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/core/EngineCommon.hpp"
#include <vector>

const IntVec2 IntVec2::ZERO		( 0, 0 );
const IntVec2 IntVec2::ONE		( 1, 1 );
const IntVec2 IntVec2::NEG_ONE	( -1, -1 );

//--------------------------------------------------------------------------
/**
* IntVec2
*/
IntVec2::IntVec2( int x, int y )
{
	this->x = x; 
	this->y = y; 
}

//--------------------------------------------------------------------------
/**
* IntVec2
*/
IntVec2::IntVec2( const char* text )
{
	SetFromText( text );
}

//--------------------------------------------------------------------------
/**
* IntVec2
*/
IntVec2::IntVec2( Vec2 vec )
{
	x = (int) vec.x;
	y = (int) vec.y;
}

//--------------------------------------------------------------------------
/**
* SetFromText
*/
void IntVec2::SetFromText( const char* text )
{
	std::vector< std::string > splits = SplitStringOnDelitmiter( text, "," );
	GUARANTEE_OR_DIE( (int) splits.size() == 2, Stringf( "IntVec2 SetFromText being called with %d splits", (int) splits.size() ) );

	x = StringToInt( splits[0] );
	y = StringToInt( splits[1] );
}

//--------------------------------------------------------------------------
/**
* operator-
*/
const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	return IntVec2( x - vecToSubtract.x, y - vecToSubtract.y );
}

//--------------------------------------------------------------------------
/**
* operator!=
*/
bool IntVec2::operator!=( const IntVec2& compare ) const
{
	return ( x != compare.x ) || ( y != compare.y );
}

//--------------------------------------------------------------------------
/**
* operator==
*/
bool IntVec2::operator==( const IntVec2& compare ) const
{
	return x == compare.x && y == compare.y;
}

//--------------------------------------------------------------------------
/**
* operator=
*/
void IntVec2::operator=( const IntVec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}
