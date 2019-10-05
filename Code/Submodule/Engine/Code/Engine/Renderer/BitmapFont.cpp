#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"

//--------------------------------------------------------------------------
/**
* BitmapFont
*/
BitmapFont::BitmapFont( const char* fontName, TextureView* fontTexture )
	: m_fontName( fontName )
	, m_spriteSheet( new SpriteSheet( fontTexture, IntVec2( 16, 16 ), Vec2::ZERO, Vec2::ZERO ) )
{
}

//--------------------------------------------------------------------------
/**
* ~BitmapFont
*/
BitmapFont::~BitmapFont()
{
	SAFE_DELETE(m_spriteSheet);
}

//--------------------------------------------------------------------------
/**
* AddVertsFor2DText
*/
void BitmapFont::AddVertsFor2DText( std::vector<Vertex_PCU>& verts, const Vec2& position, float height, const char* text, float aspect, const Rgba tint) const
{
	int textLength = (int) strlen(text);
	float stepX = height * aspect;
	float positionX = position.x;
	float positionY = position.y;
	for( int indexOfText = 0; indexOfText < textLength; indexOfText++ )
	{
		int spriteIndex = text[indexOfText];
		const SpriteDefinition& spriteDef = m_spriteSheet->GetSpriteDefinition( spriteIndex );
		Vec2 mins = Vec2::ZERO;
		Vec2 maxs = Vec2::ONE;
		spriteDef.GetUVs( mins, maxs );

		AABB2 box( positionX + stepX * indexOfText, positionY, positionX + stepX + stepX * indexOfText, positionY + height );

		AddVertsForAABB2D( verts, box, tint, mins, maxs );
	}
}



//--------------------------------------------------------------------------
/**
* AddVertsFor2DTextAlignedInBox
*/
void BitmapFont::AddVertsFor2DTextAlignedInBox
(	std::vector<Vertex_PCU>& verts
	, float glyphHeight, const char* text
	, AABB2 box,  const Vec2& pivotAnchorAlignment /*= Vec2::ALIGN_CENTERED*/
	, eBitmapMode mode /*= BITMAP_MODE_UNCHANGED*/, float aspect /*= 1.0f*/
	, const Rgba tint /*= Rgba::WHITE*/ 
	, int maxGlyphsToDraw /*= 99999999*/) const
{
	// Gather known information
	std::vector<std::string> individualLines = SplitStringOnDelitmiter( text, "\n" );
	float glyphWidth = glyphHeight * aspect;
	bool shrinkToFit = ( mode == BITMAP_MODE_SHRINK_TO_FIT );


	// Get new box to place text in
	int boxMaxCharLength = 0;
	int boxMaxLineCount = (int) individualLines.size();
	for( int lineIndex = 0; lineIndex < boxMaxLineCount; lineIndex++ )
	{
		int lineLength = (int) strlen( individualLines[lineIndex].c_str() );
		if( lineLength > boxMaxCharLength )
		{
			boxMaxCharLength = lineLength;
		}
	}

	
	Vec2 newBoxDims( glyphWidth * (float) boxMaxCharLength, glyphHeight * (float) boxMaxLineCount );
	AABB2 newBox = box.GetBoxWithin( newBoxDims, pivotAnchorAlignment, shrinkToFit );
	float newGlyphHeight = newBox.GetHeight() / (float) boxMaxLineCount;
	float newGlyphWidth  = newGlyphHeight * aspect;

	int glyphCount = 0;
	for( int lineIndex = 0; lineIndex < boxMaxLineCount; lineIndex++ )
	{
 		std::string line = individualLines[lineIndex];
 		AABB2 lineBox = newBox.CarveBoxOffTop( 0.0f, newGlyphHeight );

		bool lastLine = false;
		int lineGlyphCount = (int) line.size();
		if( glyphCount + lineGlyphCount > maxGlyphsToDraw )
		{
			// Too many characters in line.
			line = line.substr( 0, lineGlyphCount - ( ( glyphCount + lineGlyphCount ) - maxGlyphsToDraw ) );
			lastLine = true;
		}
		glyphCount += (int) line.size(); // could be different.

		float textWidth = (float) lineGlyphCount * newGlyphWidth;
		Vec2 dims( textWidth, newGlyphHeight );

		AABB2 textBox = lineBox.GetBoxWithin( dims, pivotAnchorAlignment, shrinkToFit );

		AddVertsFor2DText( verts, textBox.m_bottomLeft, textBox.GetHeight(), line.c_str(), aspect, tint );

		if( lastLine )
		{
			return;
		}
	}
}

//--------------------------------------------------------------------------
/**
* AddVertsFor2DTextGetTexture
*/
TextureView* BitmapFont::GetTextureView() const
{
	return m_spriteSheet->GetTextureView();
}

//--------------------------------------------------------------------------
/**
* GetGlyphAspect
*/
float BitmapFont::GetGlyphAspect()
{
	return 1.0f;
}
