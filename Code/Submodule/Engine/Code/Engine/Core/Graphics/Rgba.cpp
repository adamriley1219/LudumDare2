#include "Engine/Core/Graphics/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>


//--------------------------------------------------------------------------
const Rgba Rgba::WHITE				( 1.0f, 1.0f, 1.0f );
const Rgba Rgba::GRAY				( 0.7f, 0.7f, 0.7f );
const Rgba Rgba::DARK_GRAY			( 0.3f, 0.3f, 0.3f );
const Rgba Rgba::BLACK				( 0.0f, 0.0f, 0.0f );
const Rgba Rgba::RED				( 1.0f, 0.0f, 0.0f );
const Rgba Rgba::DARK_RED			( 0.7f, 0.0f, 0.0f );
const Rgba Rgba::LIGHT_RED			( 1.0f, 0.3f, 0.3f );
const Rgba Rgba::YELLOW				( 1.0f, 1.0f, 0.0f );
const Rgba Rgba::GREEN				( 0.0f, 1.0f, 0.0f );
const Rgba Rgba::DARK_GREEN			( 0.0f, 100.0f/255.0f, 0.0f );
const Rgba Rgba::BLUE				( 0.0f, 0.0f, 1.0f );
const Rgba Rgba::LIGHT_BLUE			( 0.3f, 0.3f, 1.0f );
const Rgba Rgba::CYAN				( 0.0f, 1.0f, 1.0f );
const Rgba Rgba::MAGENTA			( 1.0f, 0.0f, 1.0f );
const Rgba Rgba::FADED_GRAY			( 0.8f, 0.8f, 0.8f, 0.5f );
const Rgba Rgba::FADED_DARK_GRAY	( 0.3f, 0.3f, 0.3f, 0.5f );
const Rgba Rgba::FADED_BLACK		( 0.0f, 0.0f, 0.0f, 0.5f );
const Rgba Rgba::INVISABLE			( 0.0f, 0.0f, 0.0f, 0.0f );


//--------------------------------------------------------------------------
/**
* RgbaRgba
*/
Rgba::Rgba()
{
	r = 1.f;
	g = 1.f;
	b = 1.f;
	a = 1.f;
}

//--------------------------------------------------------------------------
/**
* Rgba
*/
Rgba::Rgba( float red, float green, float blue, float alpha )
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

//--------------------------------------------------------------------------
/**
* Rgba
*/
Rgba::Rgba( const char* text )
{
	SetFromText( text );
}

//--------------------------------------------------------------------------
/**
* Rgba
*/
Rgba::Rgba( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha /*= 1.f*/ )
{
	r = ( (float) red ) / 255.f;
	g = ( (float) green ) / 255.f;
	b = ( (float) blue ) / 255.f;
	a = ( (float) alpha ) / 255.f;
}

//--------------------------------------------------------------------------
/**
* operator=
*/
void Rgba::operator=( const Rgba& copyFrom )
{
	r = copyFrom.r;
	g = copyFrom.g;
	b = copyFrom.b;
	a = copyFrom.a;
}


//--------------------------------------------------------------------------
/**
* operator==
*/
bool Rgba::operator==( const Rgba& compare ) const
{
	return r == compare.r && g == compare.g && b == compare.b && a == compare.a;
}

//--------------------------------------------------------------------------
/**
* operator*
*/
const Rgba Rgba::operator*( const float uniformScale ) const
{
	return Rgba( Clamp( r * uniformScale ), Clamp( g * uniformScale ), Clamp( b * uniformScale ), Clamp( a * uniformScale ) );
}

//--------------------------------------------------------------------------
/**
* SetFromText
*/
void Rgba::SetFromText( const char* text )
{
	std::vector<std::string> splits = SplitStringOnDelitmiter( text, "," );
	int size = (int) splits.size();
	GUARANTEE_OR_DIE( size == 3 || size == 4, Stringf( "RGBA SetFromText being called with %d splits", size ) );

	r = StringToFloat( splits[0] ) / 255.0f;
	g = StringToFloat( splits[1] ) / 255.0f;
	b = StringToFloat( splits[2] ) / 255.0f;
	if( size == 3 )
		a = 1.0f;
	else
		a = StringToFloat( splits[3] ) / 255.0f;
}
