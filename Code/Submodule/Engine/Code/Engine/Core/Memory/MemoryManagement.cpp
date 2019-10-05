#include "Engine/Core/Memory/MemoryManagement.hpp"
#include "Engine/Core/Debug/Callstack.hpp"
#include "Engine/Core/Memory/Allocator.hpp"
#include "Engine/Core/Debug/Profiler.hpp"

//--------------------------------------------------------------------------
#include "Game/EngineBuildPreferences.hpp"
//--------------------------------------------------------------------------

#include <mutex>
#include <map>
#include <atomic>
#include <vector>
#include <algorithm>


//--------------------------------------------------------------------------

struct MemTrackInfo_t
{
	void* original_pointer;
	size_t byte_size;
	Callstack_t callstack;
};



std::mutex& GetTrackedLock()
{
	static std::mutex s_TrackedLock;
	return s_TrackedLock;
}

std::map<void*,		// key
	MemTrackInfo_t, // value
	std::less<void*>, // less to use for map
	UntrackedAllocator<std::pair<void* const, MemTrackInfo_t>>>& // allocator  
 GetMemTracker()
{
	static std::map<void*,		// key
		MemTrackInfo_t, // value
		std::less<void*>, // less to use for map
		UntrackedAllocator<std::pair<void* const, MemTrackInfo_t>>> s_trackedMap;// allocator  
	return s_trackedMap;
};
std::atomic<size_t> g_AllocCount = 0;
std::atomic<size_t> g_AllocByteCount = 0;

//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
/**
* GetSizeToString
* take a byte count and convert to a more human-parseable
* string
* ex...
* 123 B would return "123 B"
* 1234 B would return "1.234 KB"
* 123456 B would return "123.456 KB"
* 12345678 B would return "12.346 MB"
*/
std::string GetSizeToString( size_t byte_count )
{
	if( byte_count > 1024 * 1024 * 1024 )
	{
		return Stringf( "%.3f GiB", ( (float) byte_count ) / (1024.0f * 1024.0f * 1024.0f) );
	}
	if ( byte_count > 1024 * 1024 )
	{
		return Stringf("%.3f MiB", ((float)byte_count) / (1024.0f * 1024.0f ));
	}
	if ( byte_count > 1024 )
	{
		return Stringf("%.3f KiB", ((float)byte_count) / ( 1024.0f ));
	}

	return Stringf("%u B", byte_count );
}

//--------------------------------------------------------------------------
/**
* UntrackedAlloc
*/
void* UntrackedAlloc( size_t byte_count )
{
	return ::malloc( byte_count );
}

//--------------------------------------------------------------------------
/**
* UntrackedFree
*/
void UntrackedFree( void* ptr )
{
	return ::free( ptr );
}

//--------------------------------------------------------------------------
/**
* TrackedAlloc
*/
void* TrackedAlloc( size_t byte_count )
{
	void* ptr = UntrackedAlloc(	byte_count );
	TrackAllocation( ptr, byte_count );
	return ptr;
}

//--------------------------------------------------------------------------
/**
* TrackedFree
*/
void TrackedFree( void* ptr )
{
	UntrackAllocation( ptr );
	UntrackedFree( ptr );
}

//--------------------------------------------------------------------------
/**
* TrackAllocation
*/
void TrackAllocation( void* allocation, size_t byte_count )
{
	#if defined( MEM_TRACKING )
		#if MEM_TRACKING >= MEM_TRACK_ALLOC_COUNT
			++g_AllocCount;
		#endif

		#if MEM_TRACKING >= MEM_TRACK_VERBOSE
			ProfilerAllocation( byte_count );

			MemTrackInfo_t info;

			// save callstack
			info.callstack = GetCallstack(4);

			// save size
			info.byte_size = byte_count;

			// save allocation
			info.original_pointer = allocation;

			// put this into map
			GetTrackedLock().lock();
			GetMemTracker()[info.original_pointer] = info;
			g_AllocByteCount += byte_count;			GetTrackedLock().unlock();
		#endif
	#endif
	UNUSED(allocation);
	UNUSED(byte_count);
}

//--------------------------------------------------------------------------
/**
* UntrackAllocation
*/
void UntrackAllocation( void* allocation )
{
	#if defined( MEM_TRACKING )
		#if MEM_TRACKING >= MEM_TRACK_ALLOC_COUNT
			--g_AllocCount;
		#endif

		#if MEM_TRACKING >= MEM_TRACK_VERBOSE
			// take out of this into map
			GetTrackedLock().lock();
			size_t bytes = GetMemTracker()[allocation].byte_size;
			ProfilerFree(bytes);
			g_AllocByteCount -= bytes;
			GetMemTracker().erase(allocation);
			GetTrackedLock().unlock();
		#endif
	#endif
	UNUSED(allocation);
}

//--------------------------------------------------------------------------
/**
* MemTrackGetLiveAllocationCount
*/
size_t MemTrackGetLiveAllocationCount()
{
	return g_AllocCount;
}

//--------------------------------------------------------------------------
/**
* MemTrackGetLiveByteCount
*/
size_t MemTrackGetLiveByteCount()
{
	return g_AllocByteCount;
}


// Helper structs for keeping track of information and organization in the Log
struct LogHelper_t
{
	void* ptr = nullptr;
	size_t byteSize = 0;
	uint numAllocations = 0;
	Callstack_t callstack = Callstack_t();

	void Print()
	{
		DebuggerPrintf(
			"Allocations: %u Bytes: %s\n"
			, (uint)numAllocations, GetSizeToString( byteSize ).c_str()
		);
		PrintToOutput( CallstackToString( callstack ) );
	}
};

struct CallstackComarison_t
{
	bool operator()( const Callstack_t& a, const Callstack_t& b) const
	{
		return a.m_hash > b.m_hash;
	};				
};

//--------------------------------------------------------------------------
/**
* MemTrackLogLiveAllocations
*/
void MemTrackLogLiveAllocations( uint cap /*= 100*/ )
{
#if defined( MEM_TRACKING )
	std::map<Callstack_t, LogHelper_t, CallstackComarison_t, UntrackedAllocator<std::pair<const Callstack_t, LogHelper_t>> > allocationLineMap;

	GetTrackedLock().lock();
	for ( auto mapAllocEntry : GetMemTracker() )
	{
		if ( allocationLineMap.find(mapAllocEntry.second.callstack) == allocationLineMap.end() )
		{
			LogHelper_t log;
			log.ptr = mapAllocEntry.second.original_pointer;
			log.byteSize = mapAllocEntry.second.byte_size;
			log.numAllocations = 1;
			log.callstack = mapAllocEntry.second.callstack;
			allocationLineMap[mapAllocEntry.second.callstack] = log;
		}
		else
		{
			LogHelper_t& log = allocationLineMap[mapAllocEntry.second.callstack];
			log.byteSize += mapAllocEntry.second.byte_size;
			++log.numAllocations;
		}
	}
	GetTrackedLock().unlock();

	std::vector<LogHelper_t> allocatedSortedList;

	for (auto mapAllocLine : allocationLineMap)
	{
		allocatedSortedList.push_back(mapAllocLine.second);
	}

	std::sort(allocatedSortedList.begin(), allocatedSortedList.end(), [](LogHelper_t a, LogHelper_t b) -> bool { return a.byteSize > b.byteSize; } );

	DebuggerPrintf("BEGIN MEMORY ALLOCATION PRINT\nTotal Allocations: %u Total Bytes: %s\n\n"
		, (uint)g_AllocCount, GetSizeToString(g_AllocByteCount).c_str());

	uint count = allocatedSortedList.size() > cap ? cap : (uint)allocatedSortedList.size();
	for (uint allocationInstanceIdx = 0; allocationInstanceIdx < count; ++allocationInstanceIdx)
	{
		allocatedSortedList[allocationInstanceIdx].Print();
		DebuggerPrintf("\n\n");
	}

	DebuggerPrintf("END MEMORY ALLOCATION PRINT\n");
#endif
	UNUSED(cap);
}
