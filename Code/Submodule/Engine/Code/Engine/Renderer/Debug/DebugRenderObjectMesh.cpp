#include "Engine/Renderer/Debug/DebugRenderObjectMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Renderer/Shaders/VertexBuffer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex/MeshCPU.hpp"



//--------------------------------------------------------------------------
/**
* DebugRenderObjectMesh
*/
DebugRenderObjectMesh::DebugRenderObjectMesh( MeshCPU* mesh, eDebugRenderMode renderMode, Vec3 position /*= Vec3::ZERO*/, TextureView* textureView/*= nullptr*/, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, float lifeSpan /*= 0.0f */, bool billboarded /*= true*/ )
	: DebugRenderObject( renderMode, position, startColor, endColor, lifeSpan, billboarded ), m_mesh( mesh ), m_textureView( textureView )
{
	m_xRayOverlayMesh = new MeshCPU( m_mesh );
	SetColor( startColor );
}

//--------------------------------------------------------------------------
/**
* ~DebugRenderObjectMesh
*/
DebugRenderObjectMesh::~DebugRenderObjectMesh()
{
	DebugRenderObject::~DebugRenderObject();
	delete m_mesh;
	m_mesh = nullptr;
	delete m_xRayOverlayMesh;
	m_xRayOverlayMesh = nullptr;
}

//--------------------------------------------------------------------------
/**
* Update
*/
void DebugRenderObjectMesh::Update( float deltaTime )
{
	DebugRenderObject::Update( deltaTime );
	SetColor( GetCurrentColor() );
}

//--------------------------------------------------------------------------
/**
* Render
*/
void DebugRenderObjectMesh::Render( RenderContext* ctx, Camera* camera ) const
{
	ctx->BindTextureViewWithSampler( 0, m_textureView );	

	Matrix44 mat = m_billboarded ? camera->GetModelMatrix() : Matrix44::IDENTITY;
	mat.SetT( m_position );
	ctx->BindModelMatrix( mat );

	MeshGPU gpuMesh( ctx );
	gpuMesh.CopyFromCPUMesh<Vertex_PCU>( m_mesh );
	ctx->DrawMesh( &gpuMesh );
}

//--------------------------------------------------------------------------
/**
* RenderXRayOverlay
*/
void DebugRenderObjectMesh::RenderXRayOverlay( RenderContext* ctx, Camera* camera ) const
{
	ctx->BindTextureViewWithSampler( 0, m_textureView );	

	Matrix44 mat = m_billboarded ? camera->GetModelMatrix() : Matrix44::IDENTITY;
	mat.SetT( m_position );
	ctx->BindModelMatrix( mat );

	MeshGPU gpuMesh( ctx );
	gpuMesh.CopyFromCPUMesh<Vertex_PCU>( m_xRayOverlayMesh );
	ctx->DrawMesh( &gpuMesh );
}



//--------------------------------------------------------------------------
/**
* SetColor
*/
void DebugRenderObjectMesh::SetColor( Rgba color )
{
	std::vector<VertexMaster>& vertBuffer = m_mesh->m_vertices;
	for( int vertIndex = 0; vertIndex < (int) vertBuffer.size(); ++vertIndex )
	{
		vertBuffer[vertIndex].color = color;
	}

	float percentDif = 0.5f;
	std::vector<VertexMaster>& xrayBuffer = m_xRayOverlayMesh->m_vertices;
	for( int vertIndex = 0; vertIndex < (int) xrayBuffer.size(); ++vertIndex )
	{
		xrayBuffer[vertIndex].color = color * percentDif;
	}
}

