#include "Engine/Core/Memory/Allocator.hpp"



//------------------------------------------------------------------------
// overload new and delete to use tracked allocations
//--------------------------------------------------------------------------
/**
* new overload
*/
void* operator new( size_t size )
{
	return TrackedAlloc( size );
}

/**
* delete overload
*/
void operator delete( void* ptr )
{
	TrackedFree( ptr );
}

/**
* new overload
*/
void* operator new( size_t size, Allocator& pool ) 
{ 
	return pool.alloc( size );
}

/**
* delete overload
*/
void operator delete( void* ptr, Allocator& pool ) 
{ 
	pool.free( ptr ); 
}


