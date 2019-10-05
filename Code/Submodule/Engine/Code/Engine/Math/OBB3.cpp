#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB3.hpp"

//--------------------------------------------------------------------------
/**
* OBB3
*/
OBB3::OBB3()
{

}
//--------------------------------------------------------------------------
/**
* OBB3
*/
OBB3::OBB3( const AABB3& box )
{
	m_extents.x = box.m_maxs.x - box.m_mins.x;
	m_extents.y = box.m_maxs.y - box.m_mins.y;
	m_extents.z = box.m_maxs.z - box.m_mins.z;
	m_transform.SetT( box.m_mins + m_extents );
}

//--------------------------------------------------------------------------
/**
* OBB3
*/
OBB3::OBB3( const Vec3& center, const Vec3& exstents, const Vec3& rotation )
{
	m_extents = exstents;
	m_transform = Matrix44::FromEuler( rotation,  ROTATION_ORDER_ZXY );
	m_transform.SetT( center );
}

//--------------------------------------------------------------------------
/**
* GetCorners
*/
void OBB3::GetCorners( Vec3* corners ) const
{
	GetCornersInLocalSpace( corners );
	TransformPoints( 8, corners, m_transform );
	
}

//--------------------------------------------------------------------------
/**
* GetCornersInLocalSpace
*/
void OBB3::GetCornersInLocalSpace( Vec3* corners ) const
{
	corners[0] = Vec3( -m_extents.x, -m_extents.y, -m_extents.z );
	corners[1] = Vec3(  m_extents.x, -m_extents.y, -m_extents.z );
	corners[2] = Vec3( -m_extents.x,  m_extents.y, -m_extents.z );
	corners[3] = Vec3(  m_extents.x,  m_extents.y, -m_extents.z );
	corners[4] = Vec3( -m_extents.x, -m_extents.y,  m_extents.z );
	corners[5] = Vec3(  m_extents.x, -m_extents.y,  m_extents.z );
	corners[6] = Vec3( -m_extents.x,  m_extents.y,  m_extents.z );
	corners[7] = Vec3(  m_extents.x,  m_extents.y,	m_extents.z );
}

//--------------------------------------------------------------------------
/**
* ~OBB3
*/
OBB3::~OBB3()
{

}
