#pragma once
#include "Engine/Renderer/Shaders/RenderBuffer.hpp"

class RenderContext;
struct CameraUBOData;

class UniformBuffer
	: public RenderBuffer
{
public:
	UniformBuffer( RenderContext* owner );
	~UniformBuffer();

	static UniformBuffer* Clone( const UniformBuffer& buffer );

public:
	bool CopyCPUToGPU( const void* data, size_t const size );

};