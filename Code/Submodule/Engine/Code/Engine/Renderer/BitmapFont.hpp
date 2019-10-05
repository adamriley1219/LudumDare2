#pragma once
#include "Engine/Core/Graphics/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"
#include <vector>

struct Vertex_PCU;
struct AABB2;
class RenderContext;
class SpriteSheet;
class TextureView;

enum eBitmapMode
{
	BITMAP_MODE_UNCHANGED,
	BITMAP_MODE_SHRINK_TO_FIT
};

class BitmapFont
{
	friend RenderContext;

private:
	explicit BitmapFont( const char* fontName, TextureView* fontTexture );
	~BitmapFont();

public:
	void AddVertsFor2DText
	(		std::vector<Vertex_PCU>& verts
		,	const Vec2& position,  float height, const char* text
		,	float aspect = 1.0f, const Rgba tint = Rgba::WHITE ) const;

	void AddVertsFor2DTextAlignedInBox
	(		std::vector<Vertex_PCU>& verts, float cellHeight, const char* text
		,	AABB2 box, const Vec2& pivotAnchorAlignment = Vec2::ALIGN_CENTERED
		,	eBitmapMode mode = BITMAP_MODE_UNCHANGED, float aspect = 1.0f
		,	const Rgba tint = Rgba::WHITE, int MaxGlyphsToDraw = 99999999 ) const;

	TextureView* GetTextureView() const;

protected:
	float GetGlyphAspect();

protected:
	SpriteSheet*	m_spriteSheet;
	const char*			m_fontName;

};

