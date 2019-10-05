#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif

#ifdef PLATFORM_WINDOWS
struct Rgba;
#endif

struct IntVec2;
struct Vec2;
struct Vec3;
struct Vec4;
struct FloatRange;
struct IntRange;

//--------------------------------------------------------------------------
typedef std::vector< std::string > Strings;

//--------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

std::vector< std::string > SplitStringOnDelitmiter( const char* text, const char* delimiter );

std::string FloatToString( float convert );
std::string IntToString( int convert );
float StringToFloat( const std::string& convert );
int StringToInt( const std::string& convert );

float EquateString( std::string string );



//--------------------------------------------------------------------------
// For NamedProperty use.
//--------------------------------------------------------------------------

std::string ToString( bool mBool );
std::string ToString( float mFloat );
std::string ToString( int mInt );
std::string ToString( const std::string& string );
std::string ToString( void const*const ptr );
std::string ToString( const IntVec2& vec );
std::string ToString( const Vec2& vec );
std::string ToString( const Vec3& vec );
std::string ToString( const Vec4& vec );
std::string ToString( const FloatRange& range );
std::string ToString( const IntRange& range );

#ifdef PLATFORM_WINDOWS
std::string ToString( const Rgba& color );
#endif
//--------------------------------------------------------------------------