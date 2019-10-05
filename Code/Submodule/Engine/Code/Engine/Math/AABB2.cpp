#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Physics/Transform2D.hpp"
#include <vector>

const AABB2 AABB2::ONE_BY_ONE( 1.0f, 1.0f );
const AABB2 AABB2::ZERO_TO_ONE( 1.0f, 1.0f, Vec2( .5f, .5f ) );

//--------------------------------------------------------------------------
/**
* AABB2
*/
AABB2::AABB2( float bottomLeftX, float bottomLeftY, float topRightX, float topRightY )
{
	m_topRight.x = topRightX;
	m_topRight.y = topRightY;
	m_bottomLeft.x = bottomLeftX;
	m_bottomLeft.y = bottomLeftY;
}

//--------------------------------------------------------------------------
/**
* AABB2
*/
AABB2::AABB2( float width, float height, Vec2 center )
{
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	m_topRight = Vec2( center.x + halfWidth, center.y + halfHeight );
	m_bottomLeft = Vec2( center.x - halfWidth, center.y - halfHeight );
}

//--------------------------------------------------------------------------
/**
* AABB2
*/
AABB2::AABB2( Vec2 bottomLeft, Vec2 topRight )
	: m_bottomLeft(bottomLeft), m_topRight(topRight)
{
	
}

//--------------------------------------------------------------------------
/**
* AABB2
*/
AABB2::AABB2( const char* text )
{
	SetFromText( text );
}

//--------------------------------------------------------------------------
/**
* AABB2
*/
AABB2::AABB2()
	: m_bottomLeft(Vec2::ONE), m_topRight(Vec2::ONE)
{
	
}

//--------------------------------------------------------------------------
/**
* ScaleUniform
*/
void AABB2::ScaleUniform( float scale )
{
	m_bottomLeft		*= scale;
	m_topRight		*= scale;
}

//--------------------------------------------------------------------------
/**
* GetDimention
*/
Vec2 AABB2::GetDimention() const
{
	return Vec2( GetWidth(), GetHeight() );
}

//--------------------------------------------------------------------------
/**
* GetCenter
*/
Vec2 AABB2::GetCenter() const
{
	return Vec2( m_topRight.x - GetWidth() / 2.0f
				, m_topRight.y - GetHeight() / 2.0f );
}

//--------------------------------------------------------------------------
/**
* GetWidth
*/
float AABB2::GetWidth() const
{
	return m_topRight.x - m_bottomLeft.x;
}

//--------------------------------------------------------------------------
/**
* GetCenterGetHeight
*/
float AABB2::GetHeight() const
{
	return m_topRight.y - m_bottomLeft.y;
}

//--------------------------------------------------------------------------
/**
* GetArea
*/
float AABB2::GetArea() const
{
	return GetWidth() * GetHeight();
}

//--------------------------------------------------------------------------
/**
* AddPadding
*/
void AABB2::AddPadding( float paddingAmount )
{
	m_bottomLeft.x -= paddingAmount;
	m_bottomLeft.y -= paddingAmount;
	m_topRight.x += paddingAmount;
	m_topRight.y += paddingAmount;
}

//--------------------------------------------------------------------------
/**
* AddPaddingXY
*/
void AABB2::AddPaddingXY( float paddingX, float paddingY )
{
	m_bottomLeft.x -= paddingX;
	m_bottomLeft.y -= paddingY;
	m_topRight.x += paddingX;
	m_topRight.y += paddingY;
}

//--------------------------------------------------------------------------
/**
* ScaleToFitWithin
*/
float AABB2::GetScaleToFitWithin( float width, float height ) const
{
	float widthDif = GetWidth() - width;
	float heightDif = GetHeight() - height;
	float scale = 1.0f;
	if( widthDif >= heightDif && widthDif > 0.0f )
	{
		scale += widthDif;
	}
	else if( heightDif > 0.0f )
	{
		scale += heightDif;
	}
	return scale;
}

//--------------------------------------------------------------------------
/**
* SetFromText
*/
void AABB2::SetFromText( const char* text )
{
	std::vector<std::string> splits = SplitStringOnDelitmiter( text, "," );
	int size = (int) splits.size();
	GUARANTEE_OR_DIE( size == 4, Stringf( "RGBA SetFromText being called with %d splits", size ) );

	m_bottomLeft.x = StringToFloat( splits[0] );
	m_bottomLeft.y = StringToFloat( splits[1] );
	m_topRight.x = StringToFloat( splits[2] );
	m_topRight.y = StringToFloat( splits[3] );
}

//--------------------------------------------------------------------------
/**
* Resize
*/
void AABB2::Resize( const Vec2& newDimensions, const Vec2& pivotAnchorAlignment /*= Vec2::ALIGN_CENTERED */ )
{
	float shiftLeftBy = newDimensions.x * pivotAnchorAlignment.x;
	float shiftDownBy = newDimensions.y * pivotAnchorAlignment.y;
	m_topRight = Vec2( newDimensions.x - shiftLeftBy, newDimensions.y - shiftDownBy );
	m_bottomLeft = Vec2( 0.0f - shiftLeftBy, 0.0f * - shiftDownBy );
}

//--------------------------------------------------------------------------
/**
* AlignWithinBox
*/
void AABB2::AlignWithinBox( const AABB2& box, const Vec2& alignment )
{
	float curWidth	= GetWidth();
	float curHeight = GetHeight();
	float xLeftOver = box.GetWidth() - curWidth;
	float yLeftOver = box.GetHeight() - curHeight;


	m_bottomLeft.x	= box.m_bottomLeft.x	+ xLeftOver * alignment.x;	// Move over by alignment
	m_bottomLeft.y	= box.m_bottomLeft.y	+ yLeftOver * alignment.y;	// Move over by alignment
	m_topRight.x	= m_bottomLeft.x		+ curWidth;					// Move over by width
	m_topRight.y	= m_bottomLeft.y		+ curHeight;				// move over by height
}

//--------------------------------------------------------------------------
/**
* GetBoxWithin
*/
AABB2 AABB2::GetBoxWithin( const Vec2& dimensions, const Vec2& alignment, bool shrinkToFit /*= true */ ) const
{
	Vec2 dims = Vec2(dimensions);
	if( shrinkToFit )
	{
		float scaleToFitX = 1.0f;
		float scaleToFitY = 1.0f;

		if( dimensions.x > GetWidth() )
		{
			scaleToFitX = GetWidth() / dimensions.x;
		}
		if( dimensions.y > GetHeight() )
		{
			scaleToFitY = GetHeight() / dimensions.y;
		}
		float scale = GetMin( scaleToFitX, scaleToFitY );
		dims.x *= scale;
		dims.y *= scale;
	}


	AABB2 boxWithin( dims.x, dims.y );
	boxWithin.AlignWithinBox( *this, alignment );
	return boxWithin;
}

//--------------------------------------------------------------------------
/**
* GetBoxAtLeft
*/
AABB2 AABB2::GetBoxAtLeft( float fractionOfWidth, float additionalWidth/*=0.f */ ) const
{
	Vec2 cutBotLeft = m_bottomLeft;
	float cutWidth =  GetWidth() * fractionOfWidth;

	cutWidth += additionalWidth;

	Vec2 cutTopRight( cutBotLeft.x + cutWidth, m_topRight.y );

	return AABB2( cutBotLeft, cutTopRight );
}

//--------------------------------------------------------------------------
/**
* GetBoxAtRight
*/
AABB2 AABB2::GetBoxAtRight( float fractionOfWidth, float additionalWidth/*=0.f */ ) const
{
	Vec2 cutTopRight = m_topRight;
	float cutWidth = GetWidth() * fractionOfWidth;

	cutWidth += additionalWidth;

	Vec2 cutBotLeft( cutTopRight.x - cutWidth, m_bottomLeft.y );

	return AABB2( cutBotLeft, cutTopRight );
}

//--------------------------------------------------------------------------
/**
* GetBoxAtBottom
*/
AABB2 AABB2::GetBoxAtBottom( float fractionOfHeight, float additionalHeight/*=0.f */ ) const
{
	Vec2 cutBotLeft = m_bottomLeft;
	float cutHeight = GetHeight() * fractionOfHeight;

	cutHeight += additionalHeight;

	Vec2 cutTopRight( m_topRight.x, cutBotLeft.y + cutHeight );

	return AABB2( cutBotLeft, cutTopRight );
}

//--------------------------------------------------------------------------
/**
* GetBoxAtTop
*/
AABB2 AABB2::GetBoxAtTop( float fractionOfHeight, float additionalHeight/*=0.f */ ) const
{
	Vec2 cutTopRight = m_topRight;
	float cutHeight = GetHeight() * fractionOfHeight;

	cutHeight += additionalHeight;

	Vec2 cutBotLeft( m_bottomLeft.x, cutTopRight.y - cutHeight );

	return AABB2( cutBotLeft, cutTopRight );
}

//--------------------------------------------------------------------------
/**
* CarveBoxOffLeft
*/
AABB2 AABB2::CarveBoxOffLeft( float fractionOfWidth, float additionalWidth/*=0.f */ )
{
	AABB2 cut = GetBoxAtLeft( fractionOfWidth, additionalWidth );
	float cutWidth = cut.GetWidth();
	if( GetWidth() > cutWidth )
	{
		m_bottomLeft.x += cutWidth;
	}
	else
	{
		m_bottomLeft.x = m_topRight.x; // Cut all of the box off. Possibly over total width.
	}
	return cut;
}

//--------------------------------------------------------------------------
/**
* CarveBoxOffRight
*/
AABB2 AABB2::CarveBoxOffRight( float fractionOfWidth, float additionalWidth/*=0.f */ )
{
	AABB2 cut = GetBoxAtRight( fractionOfWidth, additionalWidth );
	float cutWidth = cut.GetWidth();
	if( GetWidth() > cutWidth )
	{
		m_topRight.x -= cutWidth;
	}
	else
	{
		m_topRight.x = m_bottomLeft.x; // Cut all of the box off. Possibly over total width.
	}	
	return cut;
}

//--------------------------------------------------------------------------
/**
* CarveBoxOffBottom
*/
AABB2 AABB2::CarveBoxOffBottom( float fractionOfHeight, float additionalHeight/*=0.f */ )
{
	AABB2 cut = GetBoxAtBottom( fractionOfHeight, additionalHeight );
	float cutHeight = cut.GetHeight();
	if( GetHeight() > cutHeight )
	{
		m_bottomLeft.y += cutHeight;
	}
	else
	{
		m_bottomLeft.y = m_topRight.y; // Cut all of the box off. Possibly over total Height.
	}
	return cut;
}

//--------------------------------------------------------------------------
/**
* CarveBoxOffTop
*/
AABB2 AABB2::CarveBoxOffTop( float fractionOfHeight, float additionalHeight/*=0.f */ )
{
	AABB2 cut = GetBoxAtTop( fractionOfHeight, additionalHeight );
	float cutHeight = cut.GetHeight();
	if( GetHeight() > cutHeight )
	{
		m_topRight.y -= cutHeight;
	}
	else
	{
		m_topRight.x = m_bottomLeft.x; // Cut all of the box off. Possibly over total Height.
	}	
	return cut;
}

//--------------------------------------------------------------------------
/**
* GetTransformed
*/
AABB2 AABB2::GetTransformed( const Transform2D& transform ) const
{
	return AABB2( m_bottomLeft + transform.m_position, m_topRight + transform.m_position );
}


//--------------------------------------------------------------------------
/**
* GetTopLeft
*/
Vec2 AABB2::GetTopLeft() const
{
	return Vec2( m_bottomLeft.x, m_topRight.y );
}

//--------------------------------------------------------------------------
/**
* GetTopRight
*/
Vec2 AABB2::GetTopRight() const
{
	return m_topRight;
}

//--------------------------------------------------------------------------
/**
* GetBottomRight
*/
Vec2 AABB2::GetBottomRight() const
{
	return Vec2( m_topRight.x, m_bottomLeft.y );
}

//--------------------------------------------------------------------------
/**
* GetBottomLeft
*/
Vec2 AABB2::GetBottomLeft() const
{
	return m_bottomLeft;
}

/**
* AddPosition
*/
void AABB2::AddPosition( Vec2 posToAdd )
{
	AddPosition( posToAdd.x, posToAdd.y );
}

//--------------------------------------------------------------------------
/**
* AddPosition
*/
void AABB2::AddPosition( float x, float y )
{
	m_bottomLeft.x += x;
	m_bottomLeft.y += y;
	m_topRight.x += x;
	m_topRight.y += y;
}

