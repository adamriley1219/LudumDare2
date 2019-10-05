#pragma once
#include "Engine/Math/Vec2.hpp"


class SpriteDefinition
{
public:
	SpriteDefinition();
	explicit SpriteDefinition( const Vec2& uvBottomLeft, const Vec2& uvTopRight, const Vec2& pivot, const Vec2& dimentions );

	void GetUVs( Vec2& out_uvBottomLeft, Vec2& out_uvTopRight ) const;
	Vec2 GetPivot() const { return m_pivot; };
	Vec2 GetDims() const { return m_dimensions; };

protected:
	Vec2 m_uvAtBotLeft;
	Vec2 m_uvAtTopRight;
	// TODO: Implement
	Vec2 m_dimensions;
	Vec2 m_pivot;
};
