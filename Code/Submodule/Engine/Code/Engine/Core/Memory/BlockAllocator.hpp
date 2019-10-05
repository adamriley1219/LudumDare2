#pragma once
#include "Engine/Core/Memory/MemoryManagement.hpp"
#include "Engine/Core/Memory/Allocator.hpp"

#include <mutex>

struct block_t;
struct chunk_t;

//--------------------------------------------------------------------------
class BlockAllocator : public Allocator
{
public:
	bool init( Allocator* base,
		size_t block_size,
		size_t alignment,
		uint blocks_per_chunk );

	bool init( void* buffer,
		size_t buffer_size,
		size_t block_size,
		size_t alignment );

	void deinit();

	virtual void* alloc( size_t byte_size ) final;
	virtual void free( void* ptr ) final;

	void* alloc_block();
	void free_block( void* ptr );

private:
	bool allocate_chunk();
	void break_up_chunk( void* chunk );
	block_t* pop_free_block();
	void push_free_block( block_t* block );

private:
	Allocator* m_base = nullptr; 

	block_t* m_free_blocks = nullptr;
	chunk_t* m_chunk_list = nullptr; 

	size_t m_alignment;
	size_t m_block_size;
	size_t m_blocks_per_chunk; 

	std::mutex m_chunk_lock; 
	std::mutex m_block_lock;
};
//--------------------------------------------------------------------------

template <typename OBJ_TYPE>
class ObjectAllocator
{
public:
	ObjectAllocator( uint blocks_per_chunk = 100 );
	ObjectAllocator( void* buffer, size_t buffer_size );
	~ObjectAllocator();

	OBJ_TYPE* alloc();
	void free(void* ptr);

private:
	BlockAllocator m_allocator;
	TrackedAllocator<OBJ_TYPE> m_trackedAllocator;
};

//--------------------------------------------------------------------------
/**
* ObjectAllocator
*/
template <typename OBJ_TYPE>
ObjectAllocator<OBJ_TYPE>::ObjectAllocator( void* buffer, size_t buffer_size )
{
	m_allocator.init( buffer, buffer_size, sizeof(OBJ_TYPE), alignof(OBJ_TYPE) );
}

//--------------------------------------------------------------------------
/**
* ObjectAllocator
*/
template <typename OBJ_TYPE>
ObjectAllocator<OBJ_TYPE>::ObjectAllocator( uint blocks_per_chunk /*= 100*/ )
{
	m_allocator.init( &m_trackedAllocator, sizeof(OBJ_TYPE), alignof(OBJ_TYPE), blocks_per_chunk );
}


//--------------------------------------------------------------------------
/**
* ~ObjectAllocator
*/
template <typename OBJ_TYPE>
ObjectAllocator<OBJ_TYPE>::~ObjectAllocator()
{
	m_allocator.deinit();
}

//--------------------------------------------------------------------------
/**
* alloc
*/
template <typename OBJ_TYPE>
OBJ_TYPE* ObjectAllocator<OBJ_TYPE>::alloc()
{
	OBJ_TYPE* obj = (OBJ_TYPE*)m_allocator.alloc(sizeof(OBJ_TYPE));
	return obj;
}

//--------------------------------------------------------------------------
/**
* free
*/
template <typename OBJ_TYPE>
void ObjectAllocator<OBJ_TYPE>::free(void* ptr)
{
	m_allocator.free(ptr);
}
