#pragma once
#include "Engine/Renderer/Debug/DebugRenderObject.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"

class MeshCPU;

class DebugRenderObjectMesh
	: public DebugRenderObject
{
	friend DebugRenderSystem;

public:
	explicit DebugRenderObjectMesh( MeshCPU* mesh, eDebugRenderMode renderMode, Vec3 position = Vec3::ZERO,
		TextureView* textureView = nullptr, Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE, float lifeSpan = 0.0f, bool billboarded = true );
	virtual ~DebugRenderObjectMesh();

private:
	virtual void Update( float deltaTime );
	virtual void Render( RenderContext* ctx, Camera* camera  ) const;
	virtual void RenderXRayOverlay( RenderContext* ctx, Camera* camera  ) const;

private:
	void SetColor( Rgba color );

private:
	MeshCPU*		m_mesh;
	MeshCPU*		m_xRayOverlayMesh;
	TextureView*	m_textureView;
};
