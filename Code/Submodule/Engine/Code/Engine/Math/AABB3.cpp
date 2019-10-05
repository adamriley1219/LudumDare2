#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"


const AABB3 AABB3::ONE_BY_ONE_BY_ONE( 1.0f, 1.0f, 1.0f );

//--------------------------------------------------------------------------
/**
* AABB3
*/
AABB3::AABB3()
	: m_mins(Vec3::ONE), m_maxs(Vec3::ONE)
{

}

//--------------------------------------------------------------------------
/**
* AABB3
*/
AABB3::AABB3( float width, float height, float depth, Vec3 center /*= Vec3::ZERO*/ )
{
	float halfWidth = width * .5f;
	float halfHeight = height * .5f;
	float halfDepth = depth * .5f;
	m_maxs = Vec3( center.x + halfWidth, center.y + halfHeight, center.z + halfDepth );
	m_mins = Vec3( center.x - halfWidth, center.y - halfHeight, center.z - halfDepth );
}

//--------------------------------------------------------------------------
/**
* AABB3
*/
AABB3::AABB3( const Vec3& mins, const Vec3& maxs )
{
	m_mins = mins; //Vec3( GetMin( mins.x, maxs.x ), GetMin( mins.y, maxs.y ), GetMin( mins.z, maxs.z ) );
	m_maxs = maxs; //Vec3( GetMax( mins.x, maxs.x ), GetMax( mins.y, maxs.y ), GetMax( mins.z, maxs.z ) );
}

//--------------------------------------------------------------------------
/**
* GetCorners
*/
void AABB3::GetCorners( Vec3* corners )
{
	corners[0] = m_mins;
	corners[1] = Vec3( m_maxs.x, m_mins.y, m_mins.z );
	corners[2] = Vec3( m_mins.x, m_maxs.y, m_mins.z );
	corners[3] = Vec3( m_maxs.x, m_maxs.y, m_mins.z );
	corners[4] = Vec3( m_mins.x, m_mins.y, m_maxs.z );
	corners[5] = Vec3( m_maxs.x, m_mins.y, m_maxs.z );
	corners[6] = Vec3( m_mins.x, m_maxs.y, m_maxs.z );
	corners[7] = m_maxs;
}
