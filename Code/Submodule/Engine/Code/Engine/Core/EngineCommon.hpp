#pragma once
#include "Engine/Core/Strings/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Memory/MemoryManagement.hpp"
#include "Engine/Core/EngineUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

#pragma warning( error: 4172 ) // Making returning a temporary variable reference an error instead of a warning.


//------------------------------------------------------------------------
// MACROS
//------------------------------------------------------------------------
#define SAFE_DELETE(ptr)   if ((ptr) != nullptr) { delete ptr; ptr = nullptr; }
#define UNUSED(x) (void)(x);
#define BIT_FLAG(b) ( (1) << b )

//To do Info
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )

// the important bits
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
       " --------------------------------------------------------------------------------------\n" \
       "|  TODO :   " ##x "\n" \
       " --------------------------------------------------------------------------------------\n" )


#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" ); ASSERT_RECOVERABLE(0, "")

// For combining macros
#define MACRO_COMBINE1(X,Y) X##Y  // helper macro
#define MACRO_COMBINE(X,Y) MACRO_COMBINE1(X,Y)

//--------------------------------------------------------------------------
typedef unsigned int uint;



//--------------------------------------------------------------------------

class NamedStrings;
extern NamedStrings g_gameConfigBlackboard;

class DevConsole;
extern DevConsole* g_theConsole;

class EventSystem;
extern EventSystem* g_theEventSystem;

class DebugRenderSystem;
extern DebugRenderSystem* g_theDebugRenderSystem;

struct AABB2;
struct IntVec2;
struct Vec2;
struct Rgba;

const unsigned int MAX_LIGHTS = 8;
