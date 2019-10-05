#include "Engine/Core/Memory/MPSCRingBuffer.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <thread>

struct ringbuffer_header_t
{
	uint size : 31;
	uint ready : 1;
};

//--------------------------------------------------------------------------
/**
* MPSCRingBuffer
*/
MPSCRingBuffer::MPSCRingBuffer()
{

}

//--------------------------------------------------------------------------
/**
* ~MPSCRingBuffer
*/
MPSCRingBuffer::~MPSCRingBuffer()
{
	Deinit();
}

//--------------------------------------------------------------------------
/**
* Init
*/
bool MPSCRingBuffer::Init( size_t byte_size )
{
	Deinit();

	m_buffer = (byte*) UntrackedAlloc( byte_size );
	m_byte_size = byte_size;
	return true;
}

//--------------------------------------------------------------------------
/**
* Deinit
*/
void MPSCRingBuffer::Deinit()
{
	if (m_buffer != nullptr) 
	{
		free(m_buffer);
		m_buffer = nullptr;
		m_byte_size = 0U;
	}
}

//--------------------------------------------------------------------------
/**
* TryLockWrite
*/
void* MPSCRingBuffer::TryLockWrite( size_t size )
{
	ASSERT_RECOVERABLE( size < (1 << 31), "MPSCRingBuffer::TryLockWrite, failed asert: size < (1 << 31)" );
	size_t header_size = sizeof(ringbuffer_header_t);
	size_t total_size = 2 * header_size + size;

	//std::scoped_lock lk( m_lock );
	m_lock.lock();
	if ( GetWritableSpace() < total_size )
	{
		m_lock.unlock();
		return nullptr;
	}

	size_t toJump = total_size - header_size;
	size_t new_head = m_write_head + toJump;
	if ( new_head > m_byte_size ) {
		// need to wrap;
		ringbuffer_header_t* skip_header = (ringbuffer_header_t*)(m_buffer + m_write_head);
		skip_header->size = 0;  // 0 means skip; 
		skip_header->ready = 1;

		m_write_head = 0;
	}

	if ( GetWritableSpace() < total_size ) 
	{
		m_lock.unlock();
		return nullptr;
	}

	size_t used_head = m_write_head;
	m_write_head += (uint) toJump;

	// my usable buffer
	byte* cur_buf = m_buffer + (uint)used_head;

	ringbuffer_header_t* head = (ringbuffer_header_t*)cur_buf;
	head->size = (uint)size;
	head->ready = 0;

	m_lock.unlock();
	return head + 1;
}

//--------------------------------------------------------------------------
/**
* LockWrite
*/
void* MPSCRingBuffer::LockWrite(size_t size)
{
	void* ptr = TryLockWrite(size);
	while ( ptr == nullptr ) 
	{
		std::this_thread::yield();
		ptr = TryLockWrite(size);
	}

	return ptr;
}

//--------------------------------------------------------------------------
/**
* UnlockWrite
*/
void MPSCRingBuffer::UnlockWrite( void* ptr )
{
	ringbuffer_header_t* write_head = (ringbuffer_header_t*)ptr;
	--write_head;

	write_head->ready = 1;
}

//--------------------------------------------------------------------------
/**
* TryLockRead
*/
void* MPSCRingBuffer::TryLockRead( size_t* out_size )
{
	//std::scoped_lock lk( m_lock );
	m_lock.lock();
	while (true) {
		if (m_read_head == m_write_head) {
			m_lock.unlock();
			return nullptr;
		}

		ringbuffer_header_t* read_header = (ringbuffer_header_t*)(m_buffer + m_read_head);
		if (read_header->ready) {
			if ( read_header->size == 0 ) 
			{
				// skip case
				m_read_head = 0;
			}
			else {
				// valid case
				*out_size = read_header->size;

				// SINGLE CONSUMER CASE - nothing else happens
				void* ret_buf = read_header + 1;
				m_lock.unlock();
				return ret_buf;
			}
		}
		else {
			// only one consumer - this shouldn't happen
			m_lock.unlock();
			return nullptr;
		}
	}
}

//--------------------------------------------------------------------------
/**
* UnlockRead
*/
void MPSCRingBuffer::UnlockRead(void* ptr)
{
	//std::scoped_lock lk( m_lock );
	m_lock.lock();
	ringbuffer_header_t* read_head = (ringbuffer_header_t*)ptr;
	read_head--;

	ASSERT_RECOVERABLE( (void*)(m_buffer + m_read_head) == (void*)read_head, "MPSCRingBuffer::UnlockRead failed assert: (m_buffer + m_read_head) == read_head" );

	m_read_head += sizeof(ringbuffer_header_t) + read_head->size;
	m_lock.unlock();
}

//--------------------------------------------------------------------------
/**
* GetWritableSpace
*/
size_t MPSCRingBuffer::GetWritableSpace() const
{
	size_t remaining = 0;
	if (m_write_head >= m_read_head) {
		remaining = m_byte_size - m_write_head;
		remaining += m_read_head;
	}
	else {
		remaining = m_read_head - m_write_head;
	}

	return remaining;
}
