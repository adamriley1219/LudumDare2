#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include <vector>
#include <string>

#define MAX_TRACE 64

struct Callstack_t
{
public:
	void* m_trace[MAX_TRACE] = {};	// execution pointers representing where we are in code; 
	uint m_depth = 0;			// how full is the above array
	size_t m_hash = 0;

public:
	bool operator==(const Callstack_t& compare) const;				
	bool operator!=(const Callstack_t& compare) const;

};

// skip frames is the number of frames from where we are to skip (ie, ignore)
Callstack_t GetCallstack( uint skip_frames = 0 );

// Convert a callstack to strings
// with one string per line
// Strings should return in this format...
// filepath.cpp(line,offset): function_name
// for example
// Engine/Image.cpp(127,8): Image::LoadFromFile
std::vector<std::string> CallstackToString( const Callstack_t& cs );

// Prints lines to the output
void PrintToOutput( const std::vector<std::string>& lines );

// Gets the callstack and prints to output
void PrintCallstackAtLocation();