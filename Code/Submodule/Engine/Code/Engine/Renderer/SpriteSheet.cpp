#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"


//--------------------------------------------------------------------------
/**
* SpriteSheet
*/
SpriteSheet::SpriteSheet( TextureView* texture, const IntVec2& spriteGridLayout, const Vec2& pivot, const Vec2& spriteDimensions )
	: m_texture(texture)
{
	m_gridLayout = spriteGridLayout;

	float uPerSpriteGridStepX = 1.0f / ( (float) spriteGridLayout.x );
	float vPerSpriteGridStepY = 1.0f / ( (float) spriteGridLayout.y );

	int numSprites = spriteGridLayout.x * spriteGridLayout.y;
	const int& spritesPerRow = spriteGridLayout.x;

	for( int spriteIndex = 0; spriteIndex < numSprites; spriteIndex++ )
	{
		int spriteGridX = spriteIndex % spritesPerRow;
		int spriteGridY = spriteIndex / spritesPerRow;
		
		float minU = (float)spriteGridX * uPerSpriteGridStepX;
		float maxU = minU + uPerSpriteGridStepX;
		float maxV = 1.0f - ( (float)spriteGridY * vPerSpriteGridStepY );
		float minV = maxV - vPerSpriteGridStepY;

		m_spriteDefinitions.push_back
		( 
			SpriteDefinition
			(
				Vec2( minU +.002f, minV +.002f ),
				Vec2( maxU -.002f, maxV -.002f ),
				pivot,
				spriteDimensions
			)
		);
	}
}

//--------------------------------------------------------------------------
/**
* GetSpriteDefinition
*/
const SpriteDefinition& SpriteSheet::GetSpriteDefinition( int spriteIndex ) const
{
	return m_spriteDefinitions[spriteIndex];
}

//--------------------------------------------------------------------------
/**
* GetSpriteDefinition
*/
const SpriteDefinition& SpriteSheet::GetSpriteDefinition( const IntVec2& spriteCoords ) const
{
	return GetSpriteDefinition( spriteCoords.x + spriteCoords.y * m_gridLayout.x );
}
