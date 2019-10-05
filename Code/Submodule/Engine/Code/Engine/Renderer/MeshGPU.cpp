#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex/Vertex_LIT.hpp"






//--------------------------------------------------------------------------
/**
* MeshGPU
*/
MeshGPU::MeshGPU( RenderContext *ctx )
{
	m_vertexBuffer = new VertexBuffer( ctx );
	m_indexBuffer = new IndexBuffer( ctx );
}

//--------------------------------------------------------------------------
/**
* ~MeshGPU
*/
MeshGPU::~MeshGPU()
{
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;
	delete m_indexBuffer;
	m_indexBuffer = nullptr;
}

//--------------------------------------------------------------------------
/**
* SetDrawCall
*/
void MeshGPU::SetDrawCall( bool useIndexBuffer, int elemCount )
{
	if( useIndexBuffer )
	{
		ASSERT_RECOVERABLE( (int) m_indexBuffer->m_indexCount >= elemCount, "IndexBuffer had too many elements." );
	}
	else
	{
		ASSERT_RECOVERABLE( (int) m_vertexBuffer->m_vertexCount >= elemCount, "VertexBuffer had too many elements." );
	}

	m_elementCount = elemCount;
	m_useIndexBuffer = useIndexBuffer;
}

