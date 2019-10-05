#include "Engine/Math/Frustum.hpp"
#include "Engine/Math/AABB3.hpp"

//--------------------------------------------------------------------------
/**
* Frustum
*/
Frustum::Frustum( const Vec3* corners )
{
	m_corners[0] = corners[0];
	m_corners[1] = corners[1];
	m_corners[2] = corners[2];
	m_corners[3] = corners[3];
	m_corners[4] = corners[4];
	m_corners[5] = corners[5];
	m_corners[6] = corners[6];
	m_corners[7] = corners[7];

	m_planes[FRUSTUM_RIGHT] = Plane3::FromPointsLH( corners[1], corners[3], corners[7] ); 
	m_planes[FRUSTUM_TOP] = Plane3::FromPointsLH( corners[2], corners[6], corners[7] ); 
	m_planes[FRUSTUM_FRONT] = Plane3::FromPointsLH( corners[0], corners[2], corners[3] ); 
	m_planes[FRUSTUM_LEFT] = Plane3::FromPointsLH( corners[0], corners[4], corners[6] ); 
	m_planes[FRUSTUM_BOTTOM] = Plane3::FromPointsLH( corners[0], corners[5], corners[4] ); 
	m_planes[FRUSTUM_BACK] = Plane3::FromPointsLH( corners[4], corners[5], corners[7] ); 
}

//--------------------------------------------------------------------------
/**
* Frustum
*/
Frustum::Frustum()
{
	Vec3 corners[8];

	AABB3 aabb3;

	aabb3.GetCorners( corners );

	m_corners[0] = corners[0];
	m_corners[1] = corners[1];
	m_corners[2] = corners[2];
	m_corners[3] = corners[3];
	m_corners[4] = corners[4];
	m_corners[5] = corners[5];
	m_corners[6] = corners[6];
	m_corners[7] = corners[7];

	m_planes[FRUSTUM_RIGHT] = Plane3::FromPointsLH( corners[1], corners[3], corners[7] ); 
	m_planes[FRUSTUM_TOP] = Plane3::FromPointsLH( corners[2], corners[6], corners[7] ); 
	m_planes[FRUSTUM_FRONT] = Plane3::FromPointsLH( corners[0], corners[2], corners[3] ); 
	m_planes[FRUSTUM_LEFT] = Plane3::FromPointsLH( corners[0], corners[4], corners[6] ); 
	m_planes[FRUSTUM_BOTTOM] = Plane3::FromPointsLH( corners[0], corners[5], corners[4] ); 
	m_planes[FRUSTUM_BACK] = Plane3::FromPointsLH( corners[4], corners[5], corners[7] ); 
}

//--------------------------------------------------------------------------
/**
* GetCorners
*/
void Frustum::GetCorners( Vec3* corners ) const
{
	corners[0] = m_corners[0];
	corners[1] = m_corners[1];
	corners[2] = m_corners[2];
	corners[3] = m_corners[3];
	corners[4] = m_corners[4];
	corners[5] = m_corners[5];
	corners[6] = m_corners[6];
	corners[7] = m_corners[7];
}
