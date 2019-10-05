#pragma once
#include "Engine/Renderer/Debug/DebugRenderObject.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"

class MeshCPU;

class DebugRenderObjectText3D
	: public DebugRenderObject
{
	friend DebugRenderSystem;

public:
	explicit DebugRenderObjectText3D( std::string text, eDebugRenderMode renderMode, Vec3 position = Vec3::ZERO, Vec2 pivot = Vec2::ZERO, Vec3 eulerRot = Vec3::ZERO, float textHeight = 1.0f,
		Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE, float lifeSpan = 0.0f, bool billboarded = true );
	virtual ~DebugRenderObjectText3D();

private:
	virtual void Update( float deltaTime );
	virtual void Render( RenderContext* ctx, Camera* camera  ) const;
	virtual void RenderXRayOverlay( RenderContext* ctx, Camera* camera  ) const;

private:

private:
	std::string m_text	= "UNASSIGNED--TEXT";
	float m_textHeight	= 1.0f;
	Vec2 m_alignment	= Vec2::ZERO;
	Vec3 m_eulerRot		= Vec3::ZERO;
};
