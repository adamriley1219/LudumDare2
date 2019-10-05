#include "Engine/Renderer/Shaders/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include <d3d11.h>  
#include <DXGI.h>   
#include <dxgidebug.h>







//--------------------------------------------------------------------------
/**
* RenderBuffer
*/
RenderBuffer::RenderBuffer( RenderContext *owner )
	: m_owner( owner )
{
	
}

//--------------------------------------------------------------------------
/**
* ~RenderBuffer
*/
RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE( m_handle );
	m_owner = nullptr;
}

//--------------------------------------------------------------------------
/**
* GetSize
*/
size_t RenderBuffer::GetSize() const
{
	return m_bufferSize;
}

//--------------------------------------------------------------------------
/**
* IsStatic
*/
bool RenderBuffer::IsStatic() const
{
	return ( m_memoryUsage == GPU_MEMORY_USAGE_STATIC );
}

//--------------------------------------------------------------------------
/**
* IsDynamic
*/
bool RenderBuffer::IsDynamic() const
{
	return ( m_memoryUsage == GPU_MEMORY_USAGE_DYNAMIC );
}

//--------------------------------------------------------------------------
/**
* CreateBuffer
*/
bool RenderBuffer::CreateBuffer( void const *initialData, size_t bufferSize, size_t elementSize, eRenderBufferUsageBits usage, eGPUMemoryUsage memUsage )
{
	// Free the old handle
	DX_SAFE_RELEASE(m_handle); 

	// can not create a 0 sized buffer; 
	ASSERT_RETURN_VALUE( (bufferSize > 0U) && (elementSize > 0U), "Element and buffer sizes we're incorrect.", false );

	// static buffers MUST be supplied data.
	ASSERT_RETURN_VALUE( (memUsage != GPU_MEMORY_USAGE_STATIC) || (initialData != nullptr), "(memUsage != GPU_MEMORY_USAGE_STATIC) || (initialData != nullptr) was wrong" , false );

	// Setup the buffer
	// When creating a D3D object, we setup
	// a DESC object to describe it...
	D3D11_BUFFER_DESC bd;
	memset( &bd, 0, sizeof(D3D11_BUFFER_DESC) );
	bd.ByteWidth = (UINT) bufferSize;
	bd.StructureByteStride = (UINT) elementSize;
	bd.Usage = DXUsageFromMemoryUsage(memUsage);
	bd.BindFlags = DXBufferUsageFromBufferUsage(usage);

	// give us write access to dynamic buffers
	// and read/write access to staging buffers; 
	bd.CPUAccessFlags = 0U;
	if (memUsage == GPU_MEMORY_USAGE_DYNAMIC) {
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	} else if (usage == GPU_MEMORY_USAGE_STAGING) {
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	// TODO: support structured buffers (Summer Semester)
	//       I will probably forget about this....

	// Map initial data if relevant (something was passed in)
	D3D11_SUBRESOURCE_DATA data;
	D3D11_SUBRESOURCE_DATA *data_ptr = nullptr;
	if (initialData != nullptr) {
		memset( &data, 0, sizeof(D3D11_SUBRESOURCE_DATA) );
		data.pSysMem = initialData;
		data_ptr = &data;
	}

	// Create it - devices create resources; 
	ID3D11Device *dev = m_owner->m_D3DDevice; 
	HRESULT hr = dev->CreateBuffer( &bd, data_ptr, &m_handle );

	if (SUCCEEDED(hr)) {
		// save off options; 
		m_bufferUsage = usage;
		m_memoryUsage = memUsage; 
		m_bufferSize = bufferSize; 
		m_elementSize = elementSize; 
		return true;
	} else {
		return false;
	}
}

//--------------------------------------------------------------------------
/**
* CopyCPUToGPU
*/
bool RenderBuffer::CopyCPUToGPU( void const *data, size_t const byteSize )
{
	// staging or dynamic only & we better have room; 
	ASSERT_RECOVERABLE( !IsStatic(), "RenderBuffer::CopyCPUToGPU was called on a static RenderBuffer when it wasnt ment too." ); 
	ASSERT_RECOVERABLE( byteSize <= m_bufferSize, "RenderBuffer::CopyCPUToGPU was unable to hold that mych memory given by byteSize" ); 

	// Map and copy
	// This is a command, so runs using the context
	ID3D11DeviceContext *ctx = m_owner->m_D3DContext; 

	// Map (ie, lock and get a writable pointer)
	// Be sure to ONLY write to what you locked
	D3D11_MAPPED_SUBRESOURCE resource; 
	HRESULT hr = ctx->Map( m_handle, 
		0,    // resource index (for arrays/mip layers/etc...)
		D3D11_MAP_WRITE_DISCARD,  // says to discard (don't care) about the memory the was already there
		0U,   // option to allow this to fail if the resource is in use, 0U means we'll wait...
		&resource ); 

	if (SUCCEEDED(hr)) {
		// we're mapped!  Copy over
		memcpy( resource.pData, data, byteSize ); 

		// unlock the resource (we're done writing)
		ctx->Unmap( m_handle, 0 ); 

		return true; 

	} else {
		return false; 
	}
}
