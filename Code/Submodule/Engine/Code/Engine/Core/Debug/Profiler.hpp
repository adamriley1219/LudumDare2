#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include <thread>
#include <atomic>

struct profiler_node_t;

bool ProfilerSystemInit();
void ProfilerSystemDeinit();

void ProfilerSetMaxHistoryTime( double seconds );

void ProfilerPause();
void ProfilerResume();


void ProfilerPush( char const* label );
void ProfilerPop();

void ProfilerAllocation( size_t byte_size = 0 );
void ProfilerFree( size_t byte_size = 0 );

void ProfilerBeginFrame(char const* label = "frame");
void ProfilerEndFrame();


profiler_node_t* ProfilerAcquirePreviousTree( std::thread::id id, uint history = 0 );
profiler_node_t* ProfilerAcquirePreviousTreeForCallingThread( uint history = 0 );
void ProfilerReleaseTree( profiler_node_t* node );

class ProfileScope
{
public:

	ProfileScope( const char* name );
	~ProfileScope();

};

#define PROFILE_SCOPE( name ) \
		ProfileScope MACRO_COMBINE(__PROFILESCOPE_,__LINE__)( name );

#define PROFILE_FUNCTION()			\
		ProfileScope MACRO_COMBINE(__PROFILESCOPE_,__LINE__)(__FUNCTION__);


//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
struct profiler_node_t
{
#ifndef ENGINE_DISABLE_PROFILER
	profiler_node_t* parent = nullptr;

	profiler_node_t* last_child = nullptr;
	profiler_node_t* prev_sibling = nullptr;

	char label[32];

	uint64_t start_time = 0;
	uint64_t end_time = 0;
	uint64_t life_time = 0;

	size_t memory_alloced = 0;
	size_t memory_freed = 0;
	int memory_remaining = 0;
	uint64_t alloc_count = 0;
	uint64_t free_count = 0;

	std::thread::id thread_id;
	std::atomic<uint> ref_count = 0;

	void add_child(profiler_node_t* child)
	{
		child->parent = this;
		child->prev_sibling = last_child;
		last_child = child;
	}
#endif
};