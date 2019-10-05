#include "Engine/Renderer/Debug/DebugRenderObject.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"


//--------------------------------------------------------------------------
/**
* DebugRenderObject
*/
DebugRenderObject::DebugRenderObject( eDebugRenderMode renderMode, Vec3 position /*= Vec3::ZERO*/, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, float lifeSpan /*= 0.0f */, bool billboarded /*= true*/ )
	: m_renderMode( renderMode )
	, m_position( position )
	, m_startColor( startColor )
	, m_endColor( endColor )
	, m_lifeSpan( lifeSpan )
	, m_billboarded( billboarded )
{

}

//--------------------------------------------------------------------------
/**
* ~DebugRender
*/
DebugRenderObject::~DebugRenderObject()
{

}

//--------------------------------------------------------------------------
/**
* Update
*/
void DebugRenderObject::Update( float deltaTime )
{
	m_lifetime += deltaTime;
	if( !m_garbage )
	{
		if( m_lifetime >= m_lifeSpan )
		{
			m_garbage = true;
		}
	}
}


//--------------------------------------------------------------------------
/**
* GetCurrentColor
*/
Rgba DebugRenderObject::GetCurrentColor() const
{
	if( m_lifeSpan <= 0.0f )
	{
		return m_startColor;
	}
	float percent = m_lifetime / m_lifeSpan;
	return Rgba( 
		Lerp( m_startColor.r, m_endColor.r, percent),
		Lerp( m_startColor.g, m_endColor.g, percent ),
		Lerp( m_startColor.b, m_endColor.b, percent ),
		Lerp( m_startColor.a, m_endColor.a, percent )
	);
}