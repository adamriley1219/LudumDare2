#pragma once
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"

class DebugRenderObject
{
	friend DebugRenderSystem;

public:
	explicit DebugRenderObject( eDebugRenderMode renderMode, Vec3 position = Vec3::ZERO, Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE, float lifeSpan = 0.0f, bool billboarded = true );
	virtual ~DebugRenderObject();

protected:
	virtual void Update( float deltaTime );
	virtual void Render( RenderContext* ctx, Camera* camera ) const = 0;
	virtual void RenderXRayOverlay( RenderContext* ctx, Camera* camera ) const = 0;

	Rgba GetCurrentColor() const;

protected:
	eDebugRenderMode	m_renderMode		= DEBUG_RENDER_ALWAYS;
	float				m_lifetime			= 0.0f;
	float				m_lifeSpan			= 0.0f;
	bool				m_garbage			= false;
	Rgba				m_startColor		= Rgba::WHITE;
	Rgba				m_endColor			= Rgba::WHITE;
	Vec3				m_position			= Vec3::ZERO;
	bool				m_billboarded		= true;
};
