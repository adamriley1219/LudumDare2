#pragma once
#include "Engine/Math/Vec2.hpp"

struct Transform2D;

struct AABB2
{
public:
	Vec2 m_bottomLeft;
	Vec2 m_topRight;

	const static AABB2 ONE_BY_ONE;
	const static AABB2 ZERO_TO_ONE;

public:
	AABB2();
	explicit AABB2( float bottomLeftX, float bottomLeftY, float topRightX, float topRightY );
	explicit AABB2( Vec2 bottomLeft, Vec2 topRight );
	explicit AABB2( float width, float height, Vec2 center = Vec2::ZERO );
	explicit AABB2( const char* text );

	void ScaleUniform( float scale );
	Vec2 GetDimention() const;
	Vec2 GetCenter() const;
	float GetWidth() const;
	float GetHeight() const;
	float GetArea() const;
	
	void AddPadding( float paddingAmount );
	void AddPaddingXY( float paddingX, float paddingY );
	
	float GetScaleToFitWithin( float width, float height ) const;
	void SetFromText( const char* text );

 	void	Resize( const Vec2& newDimensions, const Vec2& pivotAnchorAlignment = Vec2::ALIGN_CENTERED );
 	void	AlignWithinBox( const AABB2& box, const Vec2& alignment );
 	AABB2	GetBoxWithin( const Vec2& dimensions, const Vec2& alignment, bool shrinkToFit = true ) const;
 
	AABB2	GetBoxAtLeft(	float fractionOfWidth, 		float additionalWidth	= 0.f ) const;
	AABB2	GetBoxAtRight(	float fractionOfWidth, 		float additionalWidth	= 0.f ) const;
	AABB2	GetBoxAtBottom( float fractionOfHeight, 	float additionalHeight	= 0.f ) const;
	AABB2	GetBoxAtTop(	float fractionOfHeight, 	float additionalHeight	= 0.f ) const;

	AABB2	CarveBoxOffLeft(	float fractionOfWidth, 	float additionalWidth	= 0.f );
	AABB2	CarveBoxOffRight(	float fractionOfWidth, 	float additionalWidth	= 0.f );
	AABB2	CarveBoxOffBottom(	float fractionOfHeight, float additionalHeight	= 0.f );
	AABB2	CarveBoxOffTop(		float fractionOfHeight, float additionalHeight	= 0.f );

	AABB2	GetTransformed( const Transform2D& transform ) const;

	Vec2	GetTopLeft() const;
	Vec2	GetTopRight() const;
	Vec2	GetBottomRight() const;
	Vec2	GetBottomLeft() const;
	
	void	AddPosition( Vec2 posToAdd );
	void	AddPosition( float x, float y );

// 	Vec2	GetUVForPoint( const Vec2& point ) const;
// 	Vec2	GetPointForUV( const Vec2& normalizedLocalPosition ) const;
};
