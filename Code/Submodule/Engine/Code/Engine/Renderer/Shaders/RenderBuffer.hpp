#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderUtil.hpp"

class RenderContext;
struct ID3D11Buffer;


class RenderBuffer
{
public:
	RenderBuffer( RenderContext* owner );
	~RenderBuffer(); 

	size_t GetSize() const;    // return max byte size of this buffer; 
	bool IsStatic() const;     // has static usage?
	bool IsDynamic() const; 

protected:
	bool CreateBuffer( void const *initialData, 
		size_t bufferSize, 
		size_t elementSize, 
		eRenderBufferUsageBits usage, 
		eGPUMemoryUsage memUsage );

	bool CopyCPUToGPU( void const *data, size_t const byteSize );

public:
	RenderContext* m_owner;
	eRenderBufferUsageBits m_bufferUsage; 
	eGPUMemoryUsage m_memoryUsage;

	size_t m_bufferSize = (size_t) 0;    // total byte count of this buffer; 
	size_t m_elementSize = (size_t) 0;   // certain buffers have the idea of an element stride, which we'll store here; 

							// D3D11 Resource
	ID3D11Buffer* m_handle = nullptr;  
};