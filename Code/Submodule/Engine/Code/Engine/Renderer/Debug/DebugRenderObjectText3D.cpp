#include "Engine/Renderer/Debug/DebugRenderObjectText3D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Renderer/Shaders/VertexBuffer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex/MeshCPU.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Renderer/BitmapFont.hpp"



//--------------------------------------------------------------------------
/**
* DebugRenderObjectMesh
*/
DebugRenderObjectText3D::DebugRenderObjectText3D( 
	std::string text, eDebugRenderMode renderMode, Vec3 position /*= Vec3::ZERO*/, Vec2 alignment /*= Vec2::ZERO*/, Vec3 eulerRot /*= Vec3::ZERO*/, float textHeight /*= 1.0f*/
	, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, float lifeSpan /*= 0.0f */, bool billboarded /*= true*/ )
	
	: DebugRenderObject( renderMode, position, startColor, endColor, lifeSpan, billboarded )
	, m_text( text ), m_alignment( alignment ), m_eulerRot( eulerRot ), m_textHeight( textHeight )
{

}

//--------------------------------------------------------------------------
/**
* ~DebugRenderObjectMesh
*/
DebugRenderObjectText3D::~DebugRenderObjectText3D()
{
	DebugRenderObject::~DebugRenderObject();
}

//--------------------------------------------------------------------------
/**
* Update
*/
void DebugRenderObjectText3D::Update( float deltaTime )
{
	DebugRenderObject::Update( deltaTime );
}

//--------------------------------------------------------------------------
/**
* Render
*/
void DebugRenderObjectText3D::Render( RenderContext* ctx, Camera* camera ) const
{
	Matrix44 mat;
	if( m_billboarded  )
	{
		mat = camera->GetModelMatrix();
	}
	else
	{
		mat = Matrix44::FromEuler( m_eulerRot, ROTATION_ORDER_ZXY );
	}
	mat.SetT( m_position );
	ctx->BindModelMatrix( mat );


	std::vector<Vertex_PCU> verts;
	BitmapFont* font = g_theDebugRenderSystem->GetFont();

	float glyphWidth = m_textHeight * ( 1/ camera->GetAspectRatio() );
	float boxWidth = glyphWidth * m_text.length();
	AABB2 textBox( boxWidth, m_textHeight, Vec2( ( boxWidth * .5f ) - boxWidth * m_alignment.x, ( m_textHeight * .5f ) - m_textHeight * m_alignment.y ) );

	ctx->BindTextureViewWithSampler( 0, g_theDebugRenderSystem->GetFont()->GetTextureView() );	

	font->AddVertsFor2DTextAlignedInBox( 
		verts
		, m_textHeight
		, m_text.c_str()
		, textBox
		, m_alignment
		, BITMAP_MODE_SHRINK_TO_FIT
		, ( 1/ camera->GetAspectRatio() )
		, GetCurrentColor()
	);

	ctx->DrawVertexArray( verts );
}

//--------------------------------------------------------------------------
/**
* RenderXRayOverlay
*/
void DebugRenderObjectText3D::RenderXRayOverlay( RenderContext* ctx, Camera* camera ) const
{

	ctx->BindTextureViewWithSampler( 0, g_theDebugRenderSystem->GetFont()->GetTextureView() );	

	Matrix44 mat;
	if( m_billboarded  )
	{
		mat = camera->GetModelMatrix();
	}
	else
	{
		mat = Matrix44::FromEuler( m_eulerRot, ROTATION_ORDER_ZXY );
	}
	mat.SetT( m_position );
	ctx->BindModelMatrix( mat );


	std::vector<Vertex_PCU> verts;
	BitmapFont* font = g_theDebugRenderSystem->GetFont();

	float glyphWidth = m_textHeight * camera->GetAspectRatio();
	float boxWidth = glyphWidth * m_text.length();
	AABB2 textBox( boxWidth, m_textHeight, Vec2( boxWidth * m_alignment.x, m_textHeight * m_alignment.y ) );

	Rgba color = GetCurrentColor();
	color = Rgba( color.r * 0.5f, color.g * 0.5f, color.b * 0.5f, color.a * 0.5f );

	font->AddVertsFor2DTextAlignedInBox( 
		verts
		, m_textHeight
		, m_text.c_str()
		, textBox
		, m_alignment
		, BITMAP_MODE_SHRINK_TO_FIT
		, camera->GetAspectRatio()
		, color
	);

	ctx->DrawVertexArray( verts );
}


