#include "Engine/Renderer/Shaders/VertexBuffer.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Core/EngineCommon.hpp"


//--------------------------------------------------------------------------
/**
* VertexBuffer
*/
VertexBuffer::VertexBuffer( RenderContext* owner )
	: RenderBuffer( owner )
{

}

//--------------------------------------------------------------------------
/**
* ~VertexBuffer
*/
VertexBuffer::~VertexBuffer()
{

}

//--------------------------------------------------------------------------
/**
* CreateStaticFor
*/
bool VertexBuffer::CreateStaticFor( const void* vertices, unsigned int count, size_t dataSize )
{
	// just always create for static a new static buffer; 
	size_t sizeNeeded = count * dataSize;

	bool result = CreateBuffer( vertices, 
		sizeNeeded,        // total size needed for buffer?
		dataSize, // stride - size from one vertex to another
		RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT, 
		GPU_MEMORY_USAGE_STATIC ); // probably want dynamic if we're using copy

	if (result) {
		m_vertexCount = count; 
		return true; 
	} else {
		m_vertexCount = 0U; 
		return false; 
	}
}

//--------------------------------------------------------------------------
/**
* CopyCPUToGPU
*/
bool VertexBuffer::CopyCPUToGPU( void const *data, const size_t size )
{
	size_t sizeNeeded = size * sizeof(Vertex_PCU);
	if (sizeNeeded > GetSize() || IsStatic()) {
		bool result = CreateBuffer( data, 
			sizeNeeded,        // total size needed for buffer?
			sizeof(Vertex_PCU),        // 1U or byteSize is fine - not used for a UniformBuffer
			RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT, 
			GPU_MEMORY_USAGE_DYNAMIC ); // probably want dynamic if we're using copy

		if (result) {
			m_vertexCount = (unsigned int) size; 
		} else {
			m_vertexCount = 0U; 
		}

		return result; 

	} else {
		// non-static and we have enough room, so call down 
		// to our base classes CopyGPUToGPU that assumes
		// enough room is already allocated; 
		ASSERT_RECOVERABLE( IsDynamic(), "Creating memory on GPU that broke." ); 
		if (RenderBuffer::CopyCPUToGPU( data, sizeNeeded )) {
			m_vertexCount = (unsigned int) size; 
			return true; 
		}
	}

	return false; 
}
