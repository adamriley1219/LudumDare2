#pragma once
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"
#include <vector>
#include <string>



enum eRenderDataFormat
{
	RDF_NULL, 

	RDF_FLOAT, 
	RDF_VEC2, 
	RDF_VEC3, 
	RDF_VEC4, 
	RDF_RGBA32, 
};

size_t GetSizeFromRenderDataFormat( eRenderDataFormat format );

struct BufferAttribute
{
	std::string name; 
	eRenderDataFormat format; 
	size_t memberOffset; 

	BufferAttribute();
	BufferAttribute( std::string name, eRenderDataFormat format, size_t offset );

	inline bool is_null() const { return format == RDF_NULL; }
};

struct VertexMaster
{
	Vec3 position	= Vec3::ZERO;          
	Vec3 normal		= Vec3( 0.0f, 1.0f, 0.0f );   
	Rgba color		= Rgba::WHITE;             
	Vec2 uv			= Vec2::ONE;                
	Vec4 tangent	= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );      
}; 


size_t ComputeGPUBuffer( const BufferAttribute* attArray, unsigned int vertCount );
size_t ComputeAttBufferStride( const BufferAttribute* attArray );
size_t ComputeAttBufferCount( const BufferAttribute* attArray );

