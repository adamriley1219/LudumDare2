#include "Engine/Renderer/SpriteDefinition.hpp"

//--------------------------------------------------------------------------
/**
* SpriteDefinition
*/
SpriteDefinition::SpriteDefinition( const Vec2& uvBottomLeft, const Vec2& uvTopRight, const Vec2& pivot, const Vec2& dimentions )
{
	m_uvAtBotLeft = uvBottomLeft;
	m_uvAtTopRight = uvTopRight;
	
	m_dimensions = dimentions;

	m_pivot = pivot;
}

//--------------------------------------------------------------------------
/**
* SpriteDefinition
*/
SpriteDefinition::SpriteDefinition()
{
	m_uvAtBotLeft = Vec2::ZERO;
	m_uvAtTopRight = Vec2::ONE;

	m_dimensions = Vec2::ONE;

	m_pivot = Vec2( 0.5f, 0.5f );
}

//--------------------------------------------------------------------------
/**
* SpriteDefinitionGetUVs
*/
void SpriteDefinition::GetUVs( Vec2& out_uvBottomLeft, Vec2& out_uvTopRight ) const
{
	out_uvBottomLeft = m_uvAtBotLeft;
	out_uvTopRight = m_uvAtTopRight;
}
