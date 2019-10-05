#include "Engine/Core/Vertex/VertexMaster.hpp"


//--------------------------------------------------------------------------
/**
* BufferAttribute
*/
BufferAttribute::BufferAttribute( std::string name, eRenderDataFormat format, size_t offset )
	: name( name ), format( format ), memberOffset( offset )
{

}

//--------------------------------------------------------------------------
/**
* BufferAttribute
*/
BufferAttribute::BufferAttribute()
{
	name = "UNNAMED";
	memberOffset = 0;
	format = RDF_NULL;
}

//--------------------------------------------------------------------------
/**
* GetSizeFromRenderDataFormat
*/
size_t GetSizeFromRenderDataFormat( eRenderDataFormat format )
{
	switch( format )
	{
	case RDF_NULL:
		return 0;
		break;
	case RDF_FLOAT:
		return sizeof(float);
		break;
	case RDF_VEC2:
		return sizeof(Vec2);
		break;
	case RDF_VEC3:
		return sizeof(Vec3);
		break;
	case RDF_VEC4:
		return sizeof(Vec4);
		break;
	case RDF_RGBA32:
		return sizeof(Rgba);
		break;
	default:
		return 0;
		break;
	}
}



//--------------------------------------------------------------------------
/**
* ComputeGPUBuffer
*/
size_t ComputeGPUBuffer( const BufferAttribute* attArray, unsigned int vertCount )
{
	return (size_t) ( ComputeAttBufferStride( attArray )  * vertCount );
}

//--------------------------------------------------------------------------
/**
* ComputeAttBufferStride
*/
size_t ComputeAttBufferStride( const BufferAttribute* attArray )
{
	unsigned int attCount = 0;
	// If out of bounds, check too see if there is an empty BufferAtt that is acting as the end of the array (/0)
	size_t maxAmount = 0;
	while( !attArray[attCount].is_null() )	
	{
		++attCount;
		size_t curAmount = attArray[attCount].memberOffset + GetSizeFromRenderDataFormat( attArray[attCount].format );
		if( maxAmount < curAmount )
		{
			maxAmount = curAmount;
		}
	}
	// Assumes that there is at least 1 att.
	return maxAmount;
}

//--------------------------------------------------------------------------
/**
* ComputeAttBufferCount
*/
size_t ComputeAttBufferCount( const BufferAttribute* attArray )
{
	size_t attCount = 0;
	// If out of bounds, check too see if there is an empty BufferAtt that is acting as the end of the array (/0)
	while( !attArray[attCount].is_null() )	
	{
		++attCount;
	}
	return attCount;
}

