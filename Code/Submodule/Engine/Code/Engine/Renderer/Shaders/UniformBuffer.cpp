#include "Engine/Renderer/Shaders/UniformBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <d3d11.h>  
#include <DXGI.h>   
#include <dxgidebug.h>

//--------------------------------------------------------------------------
/**
* UniformBuffer
*/
UniformBuffer::UniformBuffer( RenderContext* owner )
	: RenderBuffer( owner )
{

}

//--------------------------------------------------------------------------
/**
* ~UniformBuffer
*/
UniformBuffer::~UniformBuffer()
{

}

//--------------------------------------------------------------------------
/**
* Clone
*/
UniformBuffer* UniformBuffer::Clone( const UniformBuffer& buffer )
{
	UniformBuffer* newbuffer = new UniformBuffer( buffer.m_owner );

	void* emptyData = nullptr;
	newbuffer->CreateBuffer( emptyData, buffer.m_bufferSize, buffer.m_elementSize, buffer.m_bufferUsage, buffer.m_memoryUsage );
	buffer.m_owner->m_D3DContext->CopyResource( newbuffer->m_handle, buffer.m_handle );

	return newbuffer;
}

//--------------------------------------------------------------------------
/**
* CopyCPUToGPU
*/
bool UniformBuffer::CopyCPUToGPU( const void* data, size_t const size )
{
	if (size > GetSize() || IsStatic()) {
		bool result = CreateBuffer( data, 
			size,        // total size needed for buffer?
			size,        // 1U or byteSize is fine - not used for a UniformBuffer
			RENDER_BUFFER_USAGE_UNIFORMS_BIT, 
			GPU_MEMORY_USAGE_DYNAMIC ); // probably want dynamic if we're using copy

		return result; 

	} else {
		// non-static and we have enough room, so call down 
		// to our base classes CopyGPUToGPU that assumes
		// enough room is already allocated; 
		ASSERT_RECOVERABLE( IsDynamic(), "Creating memory on GPU that's Dynamic when the RenderBuffer doesn't believe it's Dynamic." ); 
		
		if (RenderBuffer::CopyCPUToGPU( data, size )) {
			return true; 
		}
	}

	return false; 
}
