#pragma once
#include "Engine/Renderer/RenderUtil.hpp"
#include "Engine/Core/Vertex/MeshCPU.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Renderer/Shaders/VertexBuffer.hpp"
#include "Engine/Renderer/Shaders/IndexBuffer.hpp"
#include "Engine/Math/Matrix44.hpp"

class RenderContext;


class MeshGPU
{
	friend class RenderContext;
public:
	MeshGPU( RenderContext *ctx );
	~MeshGPU();

public: // Template functions
	template<typename VERTEX_TYPE>
	void CreateFromCPUMesh( const MeshCPU* mesh, eGPUMemoryUsage mem = GPU_MEMORY_USAGE_STATIC );

	template<typename VERTEX_TYPE>
	void LoadFromObjFile( std::string fileName, bool invert = false, bool tangents = false, float scale = 1.0f, std::string transform = "x y z" );

	template<typename VERTEX_TYPE>
	void CopyFromCPUMesh( const MeshCPU* mesh, eGPUMemoryUsage mem = GPU_MEMORY_USAGE_STATIC ); 

public:
	void CopyFromVertexArrays( const Vertex_PCU* vertices, int count );                          
	void CopyIndices( const int* indices, int count );                                          

	void SetDrawCall( bool useIndexBuffer, int elemCount ); 

public:
	VertexBuffer* GetVertexBuffer() { return m_vertexBuffer; };
	inline std::string const& GetDefaultMaterialName() const    { return m_defaultMaterial; } 

private:
	VertexBuffer* m_vertexBuffer; 
	IndexBuffer* m_indexBuffer; 

	// information for drawing; 
	int m_elementCount = 0; 
	bool m_useIndexBuffer = false; 

	std::string m_defaultMaterial = "default"; 

};


//--------------------------------------------------------------------------
// Template functions
//--------------------------------------------------------------------------
template<typename VERTEX_TYPE>
void MeshGPU::CreateFromCPUMesh( const MeshCPU* mesh, eGPUMemoryUsage mem /*= GPU_MEMORY_USAGE_STATIC*/ )
{
	UNUSED(mem);

	m_defaultMaterial = mesh->m_defaultMaterial;
	int vcount = mesh->GetVertexCount(); 

	size_t stride = sizeof(VERTEX_TYPE);
	size_t gpuBufferSize = sizeof(VERTEX_TYPE) * vcount; //ComputeGPUBuffer( VERTEX_TYPE::LAYOUT, vcount );
	void* buffer = TrackedAlloc( gpuBufferSize );
	VERTEX_TYPE::CopyFromMaster( buffer, &(mesh->m_vertices)[0], vcount );

	m_vertexBuffer->m_layout = VERTEX_TYPE::LAYOUT; 
	m_vertexBuffer->m_stride = stride;
	m_vertexBuffer->CreateStaticFor( buffer, vcount, stride ); 
	m_indexBuffer->CreateStaticFor( mesh->m_indices.data(), mesh->GetIndexCount() ); 

	TrackedFree( buffer );

	m_useIndexBuffer = mesh->GetIndexCount() > 0;
	if( m_useIndexBuffer )
	{
		m_elementCount = mesh->GetIndexCount();
	}
	else
	{
		m_elementCount = vcount;
	}
}

//--------------------------------------------------------------------------
/**
* LoadFromObjFile
* https://en.wikipedia.org/wiki/Wavefront_.obj_file#Face_elements
* Not completely supported. 
* TODO: Update to allow for more defaults.
* Note: default UVs may not be correct.
*/
template<typename VERTEX_TYPE>
void MeshGPU::LoadFromObjFile( std::string fileName, bool invert /*= false*/, bool tangents /*= false*/, float scale /*= 1.0f*/, std::string transform /*= "x y z" */ )
{
	m_useIndexBuffer = true;

	MeshCPU mesh;
	mesh.LoadFromObjFile( fileName, invert, tangents, scale, transform );

	CreateFromCPUMesh<VERTEX_TYPE>( &mesh );
}


//--------------------------------------------------------------------------
/**
* CopyFromCPUMesh
*/
template<typename VERTEX_TYPE>
void MeshGPU::CopyFromCPUMesh( const MeshCPU* mesh, eGPUMemoryUsage mem /*= GPU_MEMORY_USAGE_STATIC */ )
{
	CreateFromCPUMesh<VERTEX_TYPE>( mesh, mem );
}
