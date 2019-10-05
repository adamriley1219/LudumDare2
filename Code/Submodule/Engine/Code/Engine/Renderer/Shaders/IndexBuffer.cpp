#include "Engine/Renderer/Shaders/IndexBuffer.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Core/EngineCommon.hpp"


//--------------------------------------------------------------------------
/**
* IndexBuffer
*/
IndexBuffer::IndexBuffer( RenderContext* owner )
	: RenderBuffer( owner )
{

}

//--------------------------------------------------------------------------
/**
* ~IndexBuffer
*/
IndexBuffer::~IndexBuffer()
{

}

//--------------------------------------------------------------------------
/**
* CreateStaticFor
*/
bool IndexBuffer::CreateStaticFor( const unsigned int* indices, unsigned int count )
{
	// just always create for static a new static buffer; 
	size_t sizeNeeded = count * sizeof(unsigned int);

	bool result = CreateBuffer( indices, 
		sizeNeeded,        // total size needed for buffer?
		sizeof(unsigned int), // stride - size from one vertex to another
		RENDER_BUFFER_USAGE_INDEX_STREAM_BIT, 
		GPU_MEMORY_USAGE_STATIC ); // probably want dynamic if we're using copy

	if (result) {
		m_indexCount= count; 
		return true; 
	} else {
		m_indexCount = 0U; 
		return false; 
	}
}

//--------------------------------------------------------------------------
/**
* CopyCPUToGPU
*/
bool IndexBuffer::CopyCPUToGPU( void const *data, const size_t size )
{
	size_t sizeNeeded = size * sizeof(unsigned int);
	if (sizeNeeded > GetSize() || IsStatic()) {
		bool result = CreateBuffer( data, 
			sizeNeeded,        // total size needed for buffer?
			sizeof(unsigned int),        // 1U or byteSize is fine - not used for a UniformBuffer
			RENDER_BUFFER_USAGE_INDEX_STREAM_BIT, 
			GPU_MEMORY_USAGE_DYNAMIC ); // probably want dynamic if we're using copy

		if (result) {
			m_indexCount = (unsigned int) size; 
		} else {
			m_indexCount = 0U; 
		}

		return result; 

	} else {
		// non-static and we have enough room, so call down 
		// to our base classes CopyGPUToGPU that assumes
		// enough room is already allocated; 
		ASSERT_RECOVERABLE( IsDynamic(), "Creating memory on GPU that broke." ); 
		if (RenderBuffer::CopyCPUToGPU( data, sizeNeeded )) {
			m_indexCount = (unsigned int) size; 
			return true; 
		}
	}

	return false; 
}
