#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------
/**
* OBB2
*/
OBB2::OBB2()
{

}

//--------------------------------------------------------------------------
/**
* OBB2
*/
OBB2::OBB2( AABB2 box )
{
	m_right = Vec2( 1.0f, 0.0f ); 
	m_up = Vec2( 0.0f, 1.0f ); 
	m_center = ( box.GetBottomLeft() + box.GetTopRight() ) * 0.5f;
	m_extents = ( box.GetTopRight() - box.GetBottomLeft() ) * 0.5f; 
}

//--------------------------------------------------------------------------
/**
* ~OBB2
*/
OBB2::~OBB2()
{

}

//--------------------------------------------------------------------------
/**
* OBB2
*/
OBB2::OBB2( Vec2 center, Vec2 exstents, float rotation )
	: m_center(center), m_extents(exstents)
{
	m_right.SetAngleDegrees(rotation);
	m_up = m_right.GetRotated90Degrees();
}

//--------------------------------------------------------------------------
/**
* Translate
*/
void OBB2::Translate( Vec2 offset )
{
	m_center += offset;
}

//--------------------------------------------------------------------------
/**
* MoveTo
*/
void OBB2::MoveTo( Vec2 position )
{
	m_center = position;
}

//--------------------------------------------------------------------------
/**
* RotateBy
*/
void OBB2::RotateBy( float degrees )
{
	m_right.RotateDegrees( degrees ); 
	m_up = m_right.GetRotated90Degrees(); 
}

//--------------------------------------------------------------------------
/**
* SetRotation
*/
void OBB2::SetRotation( float degrees )
{
	m_right = Vec2( CosDegrees(degrees), SinDegrees(degrees) );
	m_up = Vec2( -m_right.y, m_right.x );}

//--------------------------------------------------------------------------
/**
* GetCenter
*/
Vec2 OBB2::GetCenter() const
{
	return m_center;
}

//--------------------------------------------------------------------------
/**
* GetCorners
*/
void OBB2::GetCorners( Vec2 *out ) const
{
	out[0] = GetBotLeft();
	out[1] = GetBotRight();
	out[2] = GetTopRight();
	out[3] = GetTopLeft();
}

//--------------------------------------------------------------------------
/**
* GetPlanes
*/
void OBB2::GetPlanes( Plane2 *out ) const
{
	out[0] = Plane2::FromPointsLH( GetBotLeft()	, GetBotRight() ); 
	out[1] = Plane2::FromPointsLH( GetBotRight()	, GetTopRight() ); 
	out[2] = Plane2::FromPointsLH( GetTopRight()	, GetTopLeft() ); 
	out[3] = Plane2::FromPointsLH( GetTopLeft()	, GetBotLeft() ); 
}

//--------------------------------------------------------------------------
/**
* ToLocalPoint
*/
Vec2 OBB2::ToLocalPoint( Vec2 worldPoint ) const
{
	Vec2 disp = worldPoint - GetCenter();  

	// world i = m_right;
	// world j = m_up
	float localI = DotProduct( disp, GetRight() ); 
	float localJ = DotProduct( disp, GetUp() ); 

	return Vec2( localI, localJ ); 
}

//--------------------------------------------------------------------------
/**
* ToWorldPoint
*/
Vec2 OBB2::ToWorldPoint( Vec2 localPoint ) const
{
	return localPoint.x * GetRight() + localPoint.y * GetUp()    // Rotate
		+ m_center;                                             // Translate
}

//--------------------------------------------------------------------------
/**
* Contains
*/
bool OBB2::Contains( Vec2 worldPoint ) const
{
	Vec2 localPoint = ToLocalPoint( worldPoint ); 
	Vec2 absLocalPoint = Vec2( fabsf( localPoint.x ), fabsf( localPoint.y ) ); 
	return (absLocalPoint.x < m_extents.x) && (absLocalPoint.y < m_extents.y); 
}

//--------------------------------------------------------------------------
/**
* GetClosestPoint
*/
Vec2 OBB2::GetClosestPoint( Vec2 worldPoint ) const
{
	Vec2 localPoint = ToLocalPoint( worldPoint ); 
	Vec2 clampedPoint = Clamp( localPoint, -1.0f * m_extents, m_extents ); 

	return ToWorldPoint( clampedPoint ); 
}

//--------------------------------------------------------------------------
/**
* Intersects
*/
bool OBB2::Intersects( OBB2 const &other ) const
{
	Plane2 planesOfThis[4];    // p0
	Plane2 planesOfOther[4];   // p1

	Vec2 cornersOfThis[4];     // c0
	Vec2 cornersOfOther[4];    // c1

	GetPlanes( planesOfThis ); 
	GetCorners( cornersOfThis ); 

	other.GetPlanes( planesOfOther ); 
	other.GetCorners( cornersOfOther ); 

	for (int planeIdx = 0; planeIdx < 4; ++planeIdx) {
		Plane2 const &planeOfThis = planesOfThis[planeIdx]; 
		Plane2 const &planeOfOther = planesOfOther[planeIdx]; 

		int inFrontOfThis = 0U; 
		int inFrontOfOther = 0U; 
		for (int cornerIdx = 0; cornerIdx < 4; ++cornerIdx) {
			Vec2 const &cornerOfThis = cornersOfThis[cornerIdx]; 
			Vec2 const &cornerOfOther = cornersOfOther[cornerIdx];

			float otherFromThis = planeOfThis.GetDistance( cornerOfOther ); 
			float thisFromOther = planeOfOther.GetDistance( cornerOfThis ); 
			inFrontOfThis += (otherFromThis >= 0.0f) ? 1U : 0U; 
			inFrontOfOther += (thisFromOther >= 0.0f) ? 1U : 0U; 
		}

		// we found a sepearting plane; 
		if ((inFrontOfThis == 4) || (inFrontOfOther == 4)) {
			return false;
		}
	}

	// if I get to the end, I intersect
	return true; 
}



//--------------------------------------------------------------------------
/**
* GetBotLeft
*/
Vec2 OBB2::GetBotLeft() const
{
	return m_center - GetRight() * m_extents.x - GetUp() * m_extents.y;
}

//--------------------------------------------------------------------------
/**
* GetBotRight
*/
Vec2 OBB2::GetBotRight() const
{
	return m_center + GetRight() * m_extents.x - GetUp() * m_extents.y;
}

//--------------------------------------------------------------------------
/**
* GetTopLeft
*/
Vec2 OBB2::GetTopLeft() const
{
	return m_center - GetRight() * m_extents.x + GetUp() * m_extents.y;
}

//--------------------------------------------------------------------------
/**
* GetTopRight
*/
Vec2 OBB2::GetTopRight() const
{
	return m_center + GetRight() * m_extents.x + GetUp() * m_extents.y;
}

//--------------------------------------------------------------------------
/**
* GetArea
*/
float OBB2::GetArea()
{
	float width = m_extents.x * 2.0f;
	float height = m_extents.y * 2.0f;
	return width * height;
}

