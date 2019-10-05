#pragma once
#include "Engine/Renderer/Shaders/RenderBuffer.hpp"

class RenderContext;


class IndexBuffer
	: public RenderBuffer
{
public:
	IndexBuffer( RenderContext* owner );
	~IndexBuffer();

public:
	bool CreateStaticFor( const unsigned int* vertices, unsigned int count );

	bool CopyCPUToGPU( void const *data, const size_t size );

public:
	unsigned int m_indexCount = 0;

};