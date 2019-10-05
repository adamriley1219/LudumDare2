#include "Engine/Core/Memory/BlockAllocator.hpp"
#include "Engine/Core/EngineCommon.hpp"

typedef uint8_t byte;



//--------------------------------------------------------------------------
struct block_t
{
	block_t* next = nullptr;
};

struct chunk_t
{
	chunk_t* next = nullptr;
};
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
* init
*/
bool BlockAllocator::init( Allocator* base, size_t block_size, size_t alignment, uint blocks_per_chunk )
{
	m_base = base;
	m_block_size = block_size;
	m_alignment = alignment;
	m_blocks_per_chunk = blocks_per_chunk;

	m_free_blocks = nullptr;
	m_chunk_list = nullptr;

	allocate_chunk();  

	return true;
}

//--------------------------------------------------------------------------
/**
* init
*/
bool BlockAllocator::init( void* buffer, size_t buffer_size, size_t block_size, size_t alignment )
{
	m_block_size = block_size;
	m_alignment = alignment;
	m_blocks_per_chunk = buffer_size / block_size; 

	m_base = nullptr;
	m_free_blocks = nullptr;
	m_chunk_list = nullptr;

	break_up_chunk( buffer );

	return true;
}

//--------------------------------------------------------------------------
/**
* deinit
*/
void BlockAllocator::deinit()
{
	// if we have a base pool, free all chunks
	if (m_base) {
		// free chunks
		m_block_lock.lock();
		m_chunk_lock.lock();
		while (m_chunk_list != nullptr) 
		{
			chunk_t* list = m_chunk_list;
			m_chunk_list = m_chunk_list->next;
			m_base->free(list);
		}
		m_chunk_lock.unlock();
		m_block_lock.unlock();
	} 

	  // reset
	m_base = nullptr;
	m_free_blocks = nullptr;
	m_block_size = 0U;
	m_blocks_per_chunk = 0U;
}

//--------------------------------------------------------------------------
/**
* alloc
*/
void* BlockAllocator::alloc( size_t byte_size )
{
	UNUSED( byte_size );
	return alloc_block();
}

//--------------------------------------------------------------------------
/**
* free
*/
void BlockAllocator::free( void* ptr )
{
	free_block( ptr );
}

void BlockAllocator::push_free_block( block_t* block )
{
	m_block_lock.lock();
	block->next = m_free_blocks;
	m_free_blocks = block;
	m_block_lock.unlock();
}

//--------------------------------------------------------------------------
/**
* alloc_block
*/
void* BlockAllocator::alloc_block()
{
	block_t* block = pop_free_block();
	while (block == nullptr) 
	{
		if ( !allocate_chunk() ) 
		{
			return nullptr;
		}

		block = pop_free_block();
	}
	memset( block, 0, m_block_size );
	return block;
}

//--------------------------------------------------------------------------
/**
* free_block
*/
void BlockAllocator::free_block( void* ptr )
{
	block_t* block = (block_t*)ptr;
	push_free_block(block);
}

//--------------------------------------------------------------------------
/**
* allocate_chunk
*/
bool BlockAllocator::allocate_chunk()
{
	if( !m_base )
	{
		return false;
	}

	if ( m_chunk_lock.try_lock() ) 
	{
		// allocate a chunk of memory from base if able

		size_t chunk_size = m_blocks_per_chunk * m_block_size + sizeof(chunk_t);

		chunk_t* chunk = (chunk_t*)m_base->alloc( chunk_size );
		if (chunk == nullptr) 
		{
			m_chunk_lock.unlock();
			return false;
		}

		// track this chunk so we can free it later
		chunk->next = m_chunk_list;
		m_chunk_list = chunk;

		// break chunk 
		break_up_chunk( chunk + 1 );

		m_chunk_lock.unlock();
	}

	return true;
}

//--------------------------------------------------------------------------
/**
* break_up_chunk
*/
void BlockAllocator::break_up_chunk( void* chunk )
{
	byte* buf = (byte*)chunk;

	block_t* first = (block_t*)buf;
	block_t* head = nullptr;

	for (uint i = 0; i < m_blocks_per_chunk; ++i)
	{
		block_t* node = (block_t*)buf;
		buf += m_block_size;

		node->next = head;
		head = node;
	}
	
	m_block_lock.lock();
	first->next = m_free_blocks;
	m_free_blocks = head;
	m_block_lock.unlock();
}

//--------------------------------------------------------------------------
/**
* pop_free_block
*/
block_t* BlockAllocator::pop_free_block()
{
	m_block_lock.lock();
	block_t* head = m_free_blocks;
	if (head != nullptr) 
	{
		m_free_blocks = head->next;
	}
	m_block_lock.unlock();

	return head;
}


