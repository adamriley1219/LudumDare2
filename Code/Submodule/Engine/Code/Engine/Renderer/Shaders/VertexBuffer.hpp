#pragma once
#include "Engine/Renderer/Shaders/RenderBuffer.hpp"
#include "Engine/Core/Vertex/VertexMaster.hpp"

class RenderContext;
struct CameraUBOData;
struct Vertex_PCU;

class VertexBuffer
	: public RenderBuffer
{
public:
	VertexBuffer( RenderContext* owner );
	~VertexBuffer();

public:
	bool CreateStaticFor( const void* vertices, unsigned int count, size_t dataSize );
	bool CopyCPUToGPU( void const *data, const size_t size );

public:
	unsigned int m_vertexCount = 0;
	const BufferAttribute* m_layout;
	size_t m_stride = 0;

};