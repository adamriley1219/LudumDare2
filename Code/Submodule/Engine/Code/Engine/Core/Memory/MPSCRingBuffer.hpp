#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include <mutex>
#include <stdint.h>

typedef uint8_t byte;

class MPSCRingBuffer
{
public:
	MPSCRingBuffer();
	~MPSCRingBuffer();

	bool Init( size_t size_in_bytes );
	void Deinit();    // called from deconstructor

	// try will always return immediately
	// where a non-try version will block until there is 
	// room (it may still fail if the allocated was deinit'd
	// or the size requested was larger than the ring buffers capability)
	void* TryLockWrite( size_t size );
	void* LockWrite( size_t size );
	void UnlockWrite( void* ptr );

	void* TryLockRead( size_t* out_size );
	void* LockRead( size_t* out_size );
	void UnlockRead( void* ptr );

	// helpers - optional
	bool Write( void const* data, size_t byte_size );
	bool TryWrite( void const* data, size_t byte_size );

	// return 0 on failure, otherwise number of bytes read; 
	// be sure buffer is large enough.  Non-async version could do a peek; 
	size_t Read( void* out_data );
	size_t TryRead( void* out_data );

	size_t GetWritableSpace() const;

private:
	// members needed
	byte* m_buffer = nullptr;
	size_t m_byte_size = 0;

	uint m_write_head = 0;
	uint m_read_head = 0;

	std::mutex m_lock;

};

