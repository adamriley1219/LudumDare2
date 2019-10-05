#pragma once
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>

class TextureView;
struct IntVec2;

class SpriteSheet
{
public:
	explicit SpriteSheet( TextureView* texture, const IntVec2& spriteGridLayout, const Vec2& pivot, const Vec2& spriteDimensions );

	TextureView* GetTextureView() const { return m_texture; }
	const SpriteDefinition& GetSpriteDefinition ( int spriteIndex ) const;
	const SpriteDefinition& GetSpriteDefinition( const IntVec2& spriteCoords ) const;
protected:
	TextureView* m_texture = nullptr;
	std::vector<SpriteDefinition> m_spriteDefinitions;
	IntVec2 m_gridLayout;
};
