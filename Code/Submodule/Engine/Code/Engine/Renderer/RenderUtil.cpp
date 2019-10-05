#include "Engine/Renderer/RenderUtil.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Matrix44.hpp"
#include <d3d11.h>  
#include <DXGI.h> 
#include <dxgidebug.h>


//--------------------------------------------------------------------------
/**
* DXUsageFromMemoryUsage
*/
D3D11_USAGE DXUsageFromMemoryUsage( eGPUMemoryUsage const usage )
{
	switch (usage) {
	case GPU_MEMORY_USAGE_GPU:      return D3D11_USAGE_DEFAULT;
	case GPU_MEMORY_USAGE_STATIC:   return D3D11_USAGE_IMMUTABLE;
	case GPU_MEMORY_USAGE_DYNAMIC:  return D3D11_USAGE_DYNAMIC;
	case GPU_MEMORY_USAGE_STAGING:  return D3D11_USAGE_STAGING;
	default:
		GUARANTEE_RECOVERABLE( false, "YOUR BAD AT MEMORY IN GPUs!!!" ); 
		return D3D11_USAGE_DYNAMIC;
	}
}

//--------------------------------------------------------------------------
/**
* DXBufferUsageFromBufferUsage
*/
unsigned int DXBufferUsageFromBufferUsage( eRenderBufferUsageBits const usage )
{
	unsigned int ret = 0U;

	if (usage & RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT) {
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if (usage & RENDER_BUFFER_USAGE_INDEX_STREAM_BIT) {
		ret |= D3D11_BIND_INDEX_BUFFER;
	}

	if (usage & RENDER_BUFFER_USAGE_UNIFORMS_BIT) {
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}

	return ret;
}

//--------------------------------------------------------------------------
/**
* DXGetFilter
*/
D3D11_FILTER DXGetFilter( eFilterMode min, eFilterMode mag )
{
	
	if (min == FILTER_MODE_POINT) 
	{
		switch (mag) {
		case FILTER_MODE_POINT:  return D3D11_FILTER_MIN_MAG_MIP_POINT; 
		case FILTER_MODE_LINEAR: return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; 
				}; 
	} 
	else 
	{
		switch (mag) {
		case FILTER_MODE_POINT:  return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT; 
		case FILTER_MODE_LINEAR: return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; 
				};
	}

	ASSERT_RECOVERABLE(false, "RenderUtil D3D11_FILTER DXGetFilter Combination not handled."); // this combination was not handled?  
	return D3D11_FILTER_MIN_MAG_MIP_POINT; 
	
}

//--------------------------------------------------------------------------
/**
* DXGetCompareFunc
*/
D3D11_COMPARISON_FUNC DXGetCompareFunc( eCompareOp op )
{
	switch( op )
	{
	case COMPARE_NEVER:
		return D3D11_COMPARISON_NEVER;
		break;
	case COMPARE_ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
		break;
	case COMPARE_EQUAL:
		return D3D11_COMPARISON_EQUAL;
		break;
	case COMPARE_NOTEQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;
		break;
	case COMPARE_LESS:
		return D3D11_COMPARISON_LESS;
		break;
	case COMPARE_LEQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;
		break;
	case COMPARE_GREATER:
		return D3D11_COMPARISON_GREATER;
		break;
	case COMPARE_GEQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;
		break;
	default:
		return D3D11_COMPARISON_LESS_EQUAL;
		break;
	}
}

//--------------------------------------------------------------------------
/**
* DXGetCullMode
*/
D3D11_CULL_MODE DXGetCullMode( eRasterCullMode mode )
{
	switch( mode )
	{
	case CULL_FRONT:
		return D3D11_CULL_FRONT;
		break;
	case CULL_BACK:
		return D3D11_CULL_BACK;
		break;
	case CULL_NONE:
		return D3D11_CULL_NONE;
		break;
	default:
		return D3D11_CULL_NONE;
		break;
	}
}

//--------------------------------------------------------------------------
/**
* DXGetFillMode
*/
D3D11_FILL_MODE DXGetFillMode( bool fill )
{
	if( fill )
	{
		return D3D11_FILL_SOLID;
	}
	return D3D11_FILL_WIREFRAME;
}
