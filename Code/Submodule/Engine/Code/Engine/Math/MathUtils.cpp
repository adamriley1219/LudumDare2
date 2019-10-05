#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Physics/Manifold2D.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"
#include "Engine/Math/RNG.hpp"

#include "Engine/Math/Ray3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Capsule3.hpp"
#include "Engine/Math/Sphere.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Frustum.hpp"


//--------------------------------------------------------------------------

static RNG s_utilRNGGenerator;

//--------------------------------------------------------------------------
/**
* TransformPositionRotatePointAboutOrigin
*/
Vec2 RotatePointAboutOrigin( const Vec2& point, float rotateDegrees )
{
	float r = point.GetLength();
	float oldDegree = ATan2Degrees( point.y, point.x );
	float newDegrees = oldDegree + rotateDegrees;
	float newX = r * CosDegrees( newDegrees );
	float newY = r * SinDegrees( newDegrees );
	return Vec2( newX, newY );
}

//--------------------------------------------------------------------------
/**
* GetAngularDisplacement
*/
float GetAngularDisplacement( float startDegrees, float goalDegrees )
{
	float angularDisplacement = goalDegrees - startDegrees;
	while( angularDisplacement > 180.0f )
	{
		angularDisplacement -= 360.0f;
	}
	while( angularDisplacement < -180.0f )
	{
		angularDisplacement += 360.f;
	}
	return angularDisplacement;
}

//--------------------------------------------------------------------------
/**
* ConvertRadiansToDegrees
*/
float ConvertRadiansToDegrees( float radians )
{
	return radians * 180.f / g_PI;
}

//--------------------------------------------------------------------------
/**
* ConvertDegreesToRadians
*/
float ConvertDegreesToRadians( float radians )
{
	return radians * g_PI / 180.f;
}


//--------------------------------------------------------------------------
/**
* CosDegrees
*/
float CosDegrees( float degrees )
{
	return cosf( ConvertDegreesToRadians(degrees) );
}

//--------------------------------------------------------------------------
/**
* ArccosDegrees
*/
float ArccosDegrees( float range )
{
	 return ConvertRadiansToDegrees( acosf ( range ) ); 
}

//--------------------------------------------------------------------------
/**
* SinDegrees
*/
float SinDegrees( float degrees )
{
	return sinf( ConvertDegreesToRadians(degrees) );
}

//--------------------------------------------------------------------------
/**
* QuadraticFormula
*/
uint QuadraticFormula( float* out, float a, float b, float c )
{
	if( a == 0.0f )
	{
		if( b == 0.0f )
		{
			if( c == 0.0f )
			{
				out[0] = 0.0f;
				return 1;
			}
			else
			{
				return 0;
			}
		}
		out[0] = -c / b;
		return 1;
	}
	float underRoot = b*b - 4*a*c;
	if( underRoot < 0.0f )
	{
		return 0;
	}

	if( underRoot <= 0.0000000001f )
	{
		out[0] = -b / 2 * a;
		return 1;
	}

	out[0] = ( -b - sqrtf( underRoot ) ) / ( 2 * a ); 
	out[1] = ( -b + sqrtf( underRoot ) ) / ( 2 * a ); 
	return 2;
}

//--------------------------------------------------------------------------
/**
* ATan2Degrees
*/
float ATan2Degrees( float Y, float X )
{
	return ConvertRadiansToDegrees( (float) atan2( Y, X ) );
}

//--------------------------------------------------------------------------
/**
* GetDistance
*/
float GetDistance( const Vec2& positionA, const Vec2& positionB ) 
{
	return sqrtf( GetDistanceSquared( positionA, positionB ) );
}

//--------------------------------------------------------------------------
/**
* GetDistanceSquared
*/
float GetDistanceSquared( const Vec2& positionA, const Vec2& positionB )
{
	float xVal = positionB.x - positionA.x;
	float yVal = positionB.y - positionA.y;
	return xVal * xVal + yVal * yVal;
}

//--------------------------------------------------------------------------
/**
* GetDistance
*/
float GetDistance( const Vec3& positionA, const Vec3& positionB )
{
	return sqrtf( GetDistanceSquared( positionA, positionB ) );
}

//--------------------------------------------------------------------------
/**
* GetDistanceSquared
*/
float GetDistanceSquared( const Vec3& positionA, const Vec3& positionB )
{

	float xVal = positionB.x - positionA.x;
	float yVal = positionB.y - positionA.y;
	float zVal = positionB.z - positionA.z;
	return  xVal * xVal + yVal * yVal + zVal * zVal;
}

//--------------------------------------------------------------------------
/**
* getDistanceXY
*/
float GetDistanceXY( const Vec3& positionA, const Vec3& positionB )
{
	return sqrtf( GetDistanceXYSquared( positionA, positionB ) );
}

//--------------------------------------------------------------------------
/**
* GetDistanceXYSquared
*/
float GetDistanceXYSquared( const Vec3& positionA, const Vec3& positionB )
{

	float xVal = positionB.x - positionA.x;
	float yVal = positionB.y - positionA.y;
	return  xVal * xVal + yVal * yVal;
}

//--------------------------------------------------------------------------
/**
* GetCartesianFromSpherical
*/
Vec3 GetCartesianFromSpherical( float radius, float theta, float phi )
{
	return Vec3( CosDegrees(phi)*CosDegrees(theta), SinDegrees(phi), CosDegrees(phi)*SinDegrees(theta) ) * radius;
}

//--------------------------------------------------------------------------
/**
* IsPointInDisc
*/
bool IsPointInDisc( const Vec2& point, const Vec2& discCenter, float discRadius )
{
	return DoDiscsOverlap( point, 0.0f, discCenter, discRadius );
}

//--------------------------------------------------------------------------
/**
* DoDiscsOverlap
*/
bool DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB )
{
	float distSuared = GetDistanceSquared( centerA, centerB );
	return distSuared < ( radiusA + radiusB ) * (radiusA + radiusB );
}

//--------------------------------------------------------------------------
/**
* DoSpheresOverlap
*/
bool DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB )
{
	float distSuared = GetDistanceSquared( centerA, centerB );
	return distSuared < ( radiusA + radiusB ) * (radiusA + radiusB );
}

//--------------------------------------------------------------------------
/**
* DoesDiscOverlapAABB2
*/
bool DoesDiscOverlapAABB2( const Vec2& discCenter, float discRadius, const AABB2& box )
{
	Vec2 point = GetClosestPointOnAABB2( discCenter, box );
	return (point - discCenter).GetLength() < discRadius;
}

//--------------------------------------------------------------------------
/**
* DoesDiscOverlapLine2D
*/
bool DoesDiscOverlapLine2D( const Vec2& discCenter, float discRadius, const Vec2& pointOnLine, const Vec2& anotherPointOnLine )
{
	Vec2	normalOfLine = anotherPointOnLine - pointOnLine;
	normalOfLine.Normalize();
	Vec2	normPerpendicularOfLine = normalOfLine.GetRotated90Degrees();

	float dotDiscOntoPerp = DotProduct( normPerpendicularOfLine, discCenter );
	float distanceFromLine = abs(dotDiscOntoPerp);

	if( distanceFromLine > discRadius )
	{
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------
/**
* DoesDiscOverlapLineSegment2D
*/
bool DoesDiscOverlapLineSegment2D( const Vec2& discCenter, float discRadius, const Vec2& lineStart, const Vec2& lineEnd )
{
	const Vec2& center = discCenter;
	const Vec2& start = lineStart;
	const Vec2& end = lineEnd;
	Vec2	normLine = end - start;
	Vec2	StoC = center - start;
	Vec2	normPerp = normLine.GetRotated90Degrees();

	float CP = DotProduct( normPerp, StoC );
	float distanceFromLine = abs(CP);

	if( distanceFromLine > discRadius ) // is the disc close at all
	{
		return false;
	}

	Vec2 endToCenter = center - end;
	float ECNVal = DotProduct( endToCenter, normLine );
	float SCNVal = DotProduct( StoC, normLine ); 

	if( SCNVal > 0.0f && ECNVal < 0.0f ) // is the disc in front of line from start
	{
		return true;
	}
	else if( ECNVal >= 0.0f  ) // Center Disc is ahead of EndPoint 
	{
		return endToCenter.GetLengthSquared() < discRadius * discRadius;
	}
	else if( SCNVal <= 0.0f ) // Center is Behind Start
	{
		return StoC.GetLengthSquared() < discRadius * discRadius;
	}
	else
	{
		ERROR_AND_DIE("MathUtils - DoesDiscOverlapLineSegment2D should never get to this point.");
		return false;
	}
}

//--------------------------------------------------------------------------
/**
* DoesDiscOverlapCapsule2D
*/
bool DoesDiscOverlapCapsule2D( const Vec2& discCenter, float discRadius, const Vec2& capsuleStart, const Vec2& capsuleEnd, float capsuleRadius )
{
	return DoesDiscOverlapLineSegment2D( discCenter, discRadius + capsuleRadius, capsuleStart, capsuleEnd );
}

//--------------------------------------------------------------------------
/**
* DoesAABB2OverlapDisc
*/
bool DoesAABB2OverlapDisc( const AABB2& box, float discRadius, const Vec2& discCenter )
{
	return DoesDiscOverlapAABB2( discCenter, discRadius, box );
}

//--------------------------------------------------------------------------
/**
* DoesAABB2OverlapAABB2
*/
bool DoesAABB2OverlapAABB2( const AABB2& boxA, const AABB2& boxB )
{
	float overlapMinX = GetMax( boxA.m_bottomLeft.x, boxB.m_bottomLeft.x );
	float overlapMaxX = GetMin( boxA.m_topRight.x, boxB.m_topRight.x );

	float overlapMinY = GetMax( boxA.m_bottomLeft.y, boxB.m_bottomLeft.y );
	float overlapMaxY = GetMin( boxA.m_topRight.y, boxB.m_topRight.y );

	if( 
		overlapMaxX < overlapMinX ||
		overlapMaxY < overlapMinY 
		)
	{
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------
/**
* DoesPillboxOverlapPillbox
*/
bool DoesPillboxOverlapPillbox( const Pillbox2& pillA, const Pillbox2& pillB )
{
	Manifold2D manifold;
	bool res = GetManifold( &manifold, pillA, pillB );
	return res;
}

//--------------------------------------------------------------------------
/**
* DoesCapsule3OverlapFrustum
*/
bool DoesCapsule3OverlapFrustum( const Capsule3& capsule, const Frustum& frustum )
{
	for( uint idx = 0; idx < FRUSTUM_SIDE_COUNT; ++idx )
	{
		if(	
			!( 
			frustum.m_planes[idx].IsBehindWithBuffer( capsule.m_end, capsule.m_radius ) || 
			frustum.m_planes[idx].IsBehindWithBuffer( capsule.m_start, capsule.m_radius ) 
			) 
		)
		{
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------------------
/**
* DoesOBB3OverlapFrustum
*/
bool DoesOBB3OverlapFrustum( const OBB3& box, const Frustum& frustum )
{
	Vec3 corners[8];
	box.GetCorners( corners );
	for( uint idx = 0; idx < FRUSTUM_SIDE_COUNT; ++idx )
	{
		bool isOneCornerInsideFustum = false;
		for( uint boxCorIdx = 0; boxCorIdx < 8; ++boxCorIdx )
		{
			if( frustum.m_planes[idx].IsBehind( corners[boxCorIdx] ) )
			{
				isOneCornerInsideFustum = true;
			}
		}
		if( !isOneCornerInsideFustum )
		{
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------------------
/**
* IsPointInCapsule2D
*/
bool IsPointInCapsule2D( const Vec2& point, const Vec2& capsuleStart, const Vec2& capsuleEnd, float capsuleRadius )
{
	return DoesDiscOverlapLineSegment2D( point, capsuleRadius, capsuleStart, capsuleEnd );
}

//--------------------------------------------------------------------------
/**
* IsPointOnOBB2
*/
bool IsPointOnOBB2( const Vec2& point, const OBB2& obb )
{
	Vec2 locPoint = point - obb.GetCenter();  
	
	float dotR = DotProduct( locPoint , obb.GetRight() );
	float dotU = DotProduct( locPoint , obb.GetUp() );

	return  fabsf( dotR ) < obb.m_extents.x && fabsf( dotU ) < obb.m_extents.y;
}

//--------------------------------------------------------------------------
/**
* GetManifild
*/
bool GetManifold( Manifold2D* manifold, const Pillbox2& pillA, const Pillbox2& pillB )
{
	if( GetManifold( manifold, pillA.m_obb, pillB.m_obb ) )
	{
		manifold->contactPoint += manifold->normal * ( manifold->penetration + pillA.m_radius );
		manifold->penetration += pillA.m_radius + pillB.m_radius;
		return true;
	}
	
	Vec2 cornersA[4];
	Vec2 cornersB[4];

	pillA.m_obb.GetCorners( cornersA );
	pillB.m_obb.GetCorners( cornersB );

	Vec2 DispsFromAtoB[8];
	Vec2 startPoints[8];
	// Get all projPlanes

	startPoints[0] = GetClosestPointOnOBB2( pillB.m_obb.GetBotLeft(), pillA.m_obb );
	startPoints[1] = GetClosestPointOnOBB2( pillB.m_obb.GetBotRight(), pillA.m_obb );
	startPoints[2] = GetClosestPointOnOBB2( pillB.m_obb.GetTopLeft(), pillA.m_obb );
	startPoints[3] = GetClosestPointOnOBB2( pillB.m_obb.GetTopRight(), pillA.m_obb );

	startPoints[4] = pillA.m_obb.GetBotLeft();
	startPoints[5] = pillA.m_obb.GetBotRight();
	startPoints[6] = pillA.m_obb.GetTopLeft();
	startPoints[7] = pillA.m_obb.GetTopRight();

	DispsFromAtoB[0] = startPoints[0] - pillB.m_obb.GetBotLeft();
	DispsFromAtoB[1] = startPoints[1] - pillB.m_obb.GetBotRight();
	DispsFromAtoB[2] = startPoints[2]  - pillB.m_obb.GetTopLeft();
	DispsFromAtoB[3] = startPoints[3] - pillB.m_obb.GetTopRight();

	DispsFromAtoB[4] = startPoints[4] - GetClosestPointOnOBB2( pillA.m_obb.GetBotLeft(), pillB.m_obb );
	DispsFromAtoB[5] = startPoints[5] - GetClosestPointOnOBB2( pillA.m_obb.GetBotRight(), pillB.m_obb );
	DispsFromAtoB[6] = startPoints[6] - GetClosestPointOnOBB2( pillA.m_obb.GetTopLeft(), pillB.m_obb );
	DispsFromAtoB[7] = startPoints[7] - GetClosestPointOnOBB2( pillA.m_obb.GetTopRight(), pillB.m_obb );
	
	float distAway = 9999999.0f;
	int bestAxis = 0;
	std::vector<int> bestAxii;
	for( int axisIdx = 0; axisIdx < 8; ++axisIdx )
	{
		float sqrLen = DispsFromAtoB[axisIdx].GetLengthSquared();
		if( sqrLen < distAway )
		{
			distAway = sqrLen;
			bestAxis = axisIdx;
			bestAxii.clear();
			bestAxii.push_back( axisIdx );
		}
		else if( sqrLen <= distAway + 0.0001f && sqrLen >= distAway - 0.0001f )
		{
			bestAxii.push_back( axisIdx );
		}
	}
	
	float totalRadius = pillA.m_radius + pillB.m_radius;
	float dist = (float) sqrt( distAway );
	if( dist < totalRadius )
	{
		Vec2 avgStartPoint = Vec2::ZERO;
		for( int axisIdx = 0; axisIdx < (int) bestAxii.size(); ++axisIdx )
		{
			avgStartPoint += startPoints[bestAxii[axisIdx]];
		}
		avgStartPoint /= (float) bestAxii.size();

		manifold->penetration = totalRadius - dist;
		Vec2 normal = DispsFromAtoB[bestAxis].GetNormalized();
		manifold->normal = normal.GetLengthSquared() > 0.1f ? normal : GetRandomDirection2D();
		manifold->contactPoint = avgStartPoint - manifold->normal * pillA.m_radius;
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------
/**
* GetManifold
*/
bool GetManifold( Manifold2D* manifold, const OBB2& boxA, const OBB2& boxB )
{
	Vec2 cornersA[4];
	Vec2 cornersB[4];

	boxA.GetCorners( cornersA );
	boxB.GetCorners( cornersB );

	Vec2 axies[4];
	axies[0] = boxA.GetRight();
	axies[1] = boxA.GetUp();
	axies[2] = boxB.GetRight() * -1.0f;
	axies[3] = boxB.GetUp() * -1.0f;


	int bestAxis = -1;
	float pushDistance = 9999999.0f;

	for(  int axisIdx = 0; axisIdx < 4; ++axisIdx  )
	{
		float aMin = 999999.f;
		float aMax = -999999.f;
		float bMin = 999999.f;
		float bMax = -999999.f;

		for( int cornerIdx = 0; cornerIdx < 4; ++cornerIdx )
		{
			float dotA = DotProduct( cornersA[cornerIdx] , axies[axisIdx] );
			float dotB = DotProduct( cornersB[cornerIdx] , axies[axisIdx] );

			aMin = GetMin( aMin, dotA );
			aMax = GetMax( aMax, dotA );
			bMin = GetMin( bMin, dotB );
			bMax = GetMax( bMax, dotB );
		}

		float mins = GetMax( aMin, bMin );
		float maxs = GetMin( aMax, bMax );

		// Found axis with no overlap --- early out ---
		if( mins >= maxs)
		{
			return false;
		}

		// Find overlap distance
		float minDifs = bMax - aMin;
		float maxDifs = bMin - aMax;

		float distanceToPush;
		if( fabsf( minDifs ) < fabsf( maxDifs ) )
		{
			distanceToPush = minDifs;
		}
		else
		{
			distanceToPush = maxDifs;
		}

		// Find direction to push
		int sign = 1;
		if( distanceToPush < 0.0f )
		{
			sign = -1;
		}

		float finalPushDist = fabsf( distanceToPush );
		if( pushDistance > finalPushDist )
		{
			pushDistance = finalPushDist;
			bestAxis = axisIdx;
			manifold->normal = axies[bestAxis] * (float) sign;
		}
	}

	std::vector<Vec2> pointsWithinBox;
	for( int cornerIdx = 0; cornerIdx < 4; ++cornerIdx )
	{
		if( IsPointOnOBB2( cornersA[cornerIdx], boxB ) )
		{
			pointsWithinBox.push_back( cornersA[cornerIdx] );
		}
		if( IsPointOnOBB2( cornersB[cornerIdx], boxA ) )
		{
			pointsWithinBox.push_back( cornersB[cornerIdx] );
		}
	}	

	

	Vec2 combinedPoints = Vec2::ZERO;
	for( int pointIdx = 0; pointIdx < pointsWithinBox.size(); ++pointIdx )
	{
		combinedPoints += pointsWithinBox[pointIdx];
	}

	if( pointsWithinBox.size() > 0.0f )
	{
		combinedPoints /= (float) pointsWithinBox.size();
	}
	manifold->contactPoint = combinedPoints;

	manifold->penetration = pushDistance;

	return true;
}

//--------------------------------------------------------------------------
/**
* GetManifold
*/
bool GetManifold( Manifold2D* manifold, const Disc& discA, const Disc& discB )
{
	float distSuared = GetDistanceSquared( discA.m_center, discB.m_center );
	if( distSuared < (discA.m_radius + discB.m_radius ) * (discA.m_radius + discB.m_radius ) )
	{
		Vec2 disp = discB.m_center - discA.m_center;
		float distance = (float) sqrt( distSuared );
		manifold->contactPoint = discA.m_center + ( disp * 0.5f );
		manifold->normal = distance == 0.0f ? GetRandomDirection2D() : disp.GetNormalized();
		manifold->penetration = discA.m_radius + discB.m_radius - distance;
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* GetManifold
*/
bool GetManifold( Manifold2D* manifold, const Disc& disc, const AABB2& box )
{
	Vec2 closetPoint = GetClosestPointOnAABB2( disc.m_center, box );
	float distBetweenPoints = GetDistance( closetPoint, disc.m_center );
	float distanceToPush = disc.m_radius - distBetweenPoints;
	if( distanceToPush > 0.0f )
	{
		manifold->contactPoint = closetPoint;
		Vec2 displacement = disc.m_center - closetPoint;
		manifold->normal = displacement.GetLengthSquared() != 0.0f 
			? displacement.GetNormalized()
			: GetRandomDirection2D();
		manifold->penetration = disc.m_radius - distBetweenPoints;
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------
/**
* TransformPosition
*/
Vec2 TransformPosition( const Vec2& position, float uniformScale, float rotateDergeesAboutZ, const Vec2& translation )
{
	// Scale
	Vec2 scaled = position * uniformScale;
	// Rotate
	scaled.RotateDegrees( rotateDergeesAboutZ );
	// Translate
	return scaled + translation;
}

//--------------------------------------------------------------------------
/**
* TransformPositionTransformPosition
* About 
*/
Vec3 TransformPosition( const Vec3& position, float uniformScale, float rotateDergeesAboutZ, const Vec3& translation )
{
	Vec2 convertedPos( position.x, position.y );
	Vec2 convertedTrans( translation.x, translation.y );
	Vec2 transformedVec2 = TransformPosition( convertedPos, uniformScale, rotateDergeesAboutZ, convertedTrans );
	float scaledZ = position.z * uniformScale;
	float translatedZ = scaledZ + translation.z;
	return Vec3( transformedVec2.x, transformedVec2.y, translatedZ );
}

//--------------------------------------------------------------------------
/**
* TransformPosition
*/
Vec3 TransformPosition( const Vec3& position, float uniformScale, float rotateDergeesAboutZ, const Vec2& translationXY )
{
	Vec2 convertedPos( position.x, position.y );
	Vec2 convertedTrans( translationXY.x, translationXY.y );
	Vec2 transformedVec2 = TransformPosition( convertedPos, uniformScale, rotateDergeesAboutZ, convertedTrans );
	return Vec3( transformedVec2.x, transformedVec2.y, position.z );
}



//--------------------------------------------------------------------------
/**
* DotProduct
*/
float DotProduct( const Vec2& a, const Vec2& b )
{
	return ( a.x * b.x ) + ( a.y * b.y );
}

//--------------------------------------------------------------------------
/**
* DotProduct
*/
float DotProduct( const Vec3& a, const Vec3& b )
{
	float dot = ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z );
	return dot;
}

//--------------------------------------------------------------------------
/**
* DotProduct
*/
float DotProduct( const Vec4& a, const Vec4& b )
{
	return ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z ) + ( a.w + b.w );
}

//--------------------------------------------------------------------------
/**
* Cross
*/
Vec3 Cross( const Vec3& left, const Vec3& right )
{
	return Vec3(
		left.y * right.z - left.z * right.y,
		left.z * right.x - left.x * right.z,
		left.x * right.y - left.y * right.x
	);
}

//--------------------------------------------------------------------------
/**
* SmoothStart2
* t^2
*/
float SmoothStart2( float inputZeroToOne )
{
	return inputZeroToOne * inputZeroToOne;
}

//--------------------------------------------------------------------------
/**
* SmoothStart3
* t^3
*/
float SmoothStart3( float inputZeroToOne )
{
	return inputZeroToOne * inputZeroToOne * inputZeroToOne;
}

//--------------------------------------------------------------------------
/**
* SmoothStart4
* t^4
*/
float SmoothStart4( float inputZeroToOne )
{
	return inputZeroToOne * inputZeroToOne * inputZeroToOne * inputZeroToOne;
}

//--------------------------------------------------------------------------
/**
* SmoothStart5
* t^5
*/
float SmoothStart5( float inputZeroToOne )
{
	return inputZeroToOne * inputZeroToOne * inputZeroToOne * inputZeroToOne * inputZeroToOne;
}

//--------------------------------------------------------------------------
/**
* SmoothStop2
* 1 – (1-t)^2
*/
float SmoothStop2( float inputZeroToOne )
{
	float oppInput = 1 - inputZeroToOne;
	float smoothing = oppInput * oppInput;
	return 1 - smoothing;
}

//--------------------------------------------------------------------------
/**
* SmoothStop3
* 1 – (1-t)^3
*/
float SmoothStop3( float inputZeroToOne )
{
	float oppInput = 1 - inputZeroToOne;
	float smoothing = oppInput * oppInput * oppInput;
	return 1 - smoothing;
}

//--------------------------------------------------------------------------
/**
* SmoothStop4
* 1 – (1-t)^4
*/
float SmoothStop4( float inputZeroToOne )
{
	float oppInput = 1 - inputZeroToOne;
	float smoothing = oppInput * oppInput * oppInput * oppInput;
	return 1 - smoothing;
}

//--------------------------------------------------------------------------
/**
* SmoothStop5
* 1 – (1-t)^5
*/
float SmoothStop5( float inputZeroToOne )
{
	float oppInput = 1 - inputZeroToOne;
	float smoothing = oppInput * oppInput * oppInput * oppInput * oppInput;
	return 1 - smoothing;
}

//--------------------------------------------------------------------------
/**
* SmoothStep3
* 3t^2 - 2t^3  : "^" means "raised to the power of"
*/
float SmoothStep3( float inputZeroToOne )
{
	float inSquared = ( inputZeroToOne * inputZeroToOne );
	return  3.0f * ( inSquared ) -  2.0f * ( inSquared * inputZeroToOne );
}

//--------------------------------------------------------------------------
/**
* SmoothStep5
* 6t^5 – 15t^4 + 10t^3  
*/
float SmoothStep5( float inputZeroToOne )
{
	float inSquared = ( inputZeroToOne * inputZeroToOne );
	float inCubed	= ( inSquared * inputZeroToOne );
	float inFouth	= ( inCubed * inputZeroToOne );
	float inFifth	= ( inFouth * inputZeroToOne );

	return  6.0f * ( inFifth ) -  15.0f * ( inFouth ) + 10.0f * ( inCubed );
}

//--------------------------------------------------------------------------
/**
* GetProjectedLength
*/
float GetProjectedLength( const Vec2& projectThis, const Vec2& onto )
{
	Vec2 normOnto = onto.GetNormalized();
	return DotProduct( projectThis, normOnto );
}

//--------------------------------------------------------------------------
/**
* GetProjectedLengthGetProjectedVector
*/
Vec2 GetProjectedVector( const Vec2& projectThis, const Vec2& onto )
{
	Vec2 basis = onto.GetNormalized();
	float projLength = DotProduct( projectThis, onto );
	return basis * projLength;
}

//--------------------------------------------------------------------------
/**
* GetProjectedVector
*/
Vec3 GetProjectedVector( const Vec3& projectThis, const Vec3& onto )
{
	Vec3 basis = onto.GetNormalized();
	float projLength = DotProduct( projectThis, onto );
	return basis * projLength;
}

//--------------------------------------------------------------------------
/**
* ReflectVectorOffSurfaceNormal
*/
Vec2 ReflectVectorOffSurfaceNormal( const Vec2& incomingVector, const Vec2& surfaceNormal )
{
	Vec2 inOnN = GetProjectedVector( incomingVector, surfaceNormal );
	Vec2 perp = incomingVector - inOnN;
	return perp - inOnN;
}

//--------------------------------------------------------------------------
/**
* GetRayImpactFractionVsDisc2D
*/
float GetRayImpactFractionVsDisc2D( const Vec2& rayStart, const Vec2& rayNormalizedDir, float rayLength, const Vec2& discCenter, float discRadius )
{
	Vec2 sc = discCenter - rayStart;
	if( sc.GetLengthSquared() < discRadius * discRadius )
	{
		// Disc covering the starting point.
		return 0.0f;
	}
	
	float scOnRdLength = DotProduct( sc, rayNormalizedDir );
	if(  scOnRdLength < 0.0f || scOnRdLength > rayLength + discRadius )
	{
		// Disc behind ray or way too far away from end.
		return 1.0f;
	}
	
	Vec2 ts = rayNormalizedDir.GetRotated90Degrees();
	float scOnTsLength = DotProduct( sc, ts );

	if( scOnTsLength > discRadius )
	{
		// Disc is too far too the sides of the ray.
		return 1.0f;
	}

	// The an impact should now be on the line.
	float toSqrt = discRadius * discRadius - scOnTsLength * scOnTsLength;
	float b = (float) sqrt( toSqrt );
	float impactPoint = scOnRdLength - b;
	float normImpactPoint = impactPoint / rayLength;

	GUARANTEE_OR_DIE( normImpactPoint >= 0.0f && normImpactPoint <= 1.0f, "MathUtils::GetRayImpactFractionVsDisc2D gave an incorrect value" );

	return normImpactPoint;
}

//--------------------------------------------------------------------------
/**
* IsBitFlagSetRangeMapFloat
*/
float RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd )
{
	if( inStart == inEnd )
		return 0.5f * ( outStart + outEnd );
	
	float inRange = inEnd - inStart;
	float outRange = outEnd - outStart;
	float inDistance = inValue - inStart;

	float inFraction = inDistance / inRange;

	float outDistance = inFraction * outRange;
	float outValue = outStart + outDistance;

	return outValue;
}

//--------------------------------------------------------------------------
/**
* RangeMapFloat
*/
Vec3 RangeMapFloat( const Vec3& inValue, float inStart, float inEnd, float outStart, float outEnd )
{
	Vec3 ret;
	ret.x = RangeMapFloat( inValue.x, inStart, inEnd, outStart, outEnd );
	ret.y = RangeMapFloat( inValue.z, inStart, inEnd, outStart, outEnd );
	ret.z = RangeMapFloat( inValue.z, inStart, inEnd, outStart, outEnd );
	return ret;
}

//--------------------------------------------------------------------------
/**
* GetClosestPointOnAABB2
*/
Vec2 GetClosestPointOnAABB2( const Vec2& point, const AABB2& box )
{
	Vec2 result = point;
	if( point.x > box.m_topRight.x )
	{
		result.x = box.m_topRight.x;
	}
	else if( point.x < box.m_bottomLeft.x )
	{
		result.x = box.m_bottomLeft.x;
	}
	if( point.y > box.m_topRight.y )
	{
		result.y = box.m_topRight.y;
	}
	else if( point.y < box.m_bottomLeft.y )
	{
		result.y = box.m_bottomLeft.y;
	}
	return result;
}


//--------------------------------------------------------------------------
/**
* GetClosestPointOnOBB2
*/
Vec2 GetClosestPointOnOBB2( const Vec2& point, const OBB2& box )
{
	Vec2 locPoint = box.ToLocalPoint( point );
	AABB2 locBox = AABB2( box.m_extents.x * 2.0f, box.m_extents.y * 2.0f );
	return box.ToWorldPoint( GetClosestPointOnAABB2( locPoint, locBox ) );
}

//--------------------------------------------------------------------------
/**
* GetClosestPointOnAABB2Edge
*/
bool GetClosestPointOnAABB2Edge( const Vec2& point, const AABB2& box, Vec2& result )
{
	Vec2 closedtPoint = GetClosestPointOnAABB2( point, box );

	if( closedtPoint == point  )
	{
		float distToTop =	box.m_topRight.y	- closedtPoint.y;
		float distToBot =	closedtPoint.y		- box.m_bottomLeft.y;
		float distToLeft =	closedtPoint.x		- box.m_bottomLeft.x;
		float distToRight = box.m_topRight.x	- closedtPoint.x;


		if( distToTop < distToBot )
		{
			if( distToLeft < distToRight ) 
			{
				if( distToLeft < distToTop )
				{
					closedtPoint.x -= distToLeft; 
				}
				else
				{
					closedtPoint.y += distToTop;
				}
			}
			else 
			{
				if( distToRight < distToTop )
				{
					closedtPoint.x += distToRight; 
				}
				else
				{
					closedtPoint.y += distToTop;
				}
			}
		}
		else
		{
			if( distToLeft < distToRight ) 
			{
				if( distToLeft < distToBot )
				{
					closedtPoint.x -= distToLeft; 
				}
				else
				{
					closedtPoint.y -= distToBot;
				}
			}
			else 
			{
				if( distToRight < distToBot )
				{
					closedtPoint.x += distToRight; 
				}
				else
				{
					closedtPoint.y -= distToBot;
				}
			}
		}
		result = closedtPoint;
		return true;
	}

	result = closedtPoint;
	return false;
}


//--------------------------------------------------------------------------
/**
* GetClosestPointOnOBB2Edge
*/
bool GetClosestPointOnOBB2Edge( const Vec2& point, const OBB2& box, Vec2& result )
{
	Vec2 locPoint = box.ToLocalPoint( point );
	AABB2 locBox = AABB2( box.m_extents.x * 2.0f, box.m_extents.y * 2.0f );
	bool res = GetClosestPointOnAABB2Edge( locPoint, locBox, result );
	result = box.ToWorldPoint( result );
	return res;
}

//--------------------------------------------------------------------------
/**
* GetClosestPointOnLine2D
*/
Vec2 GetClosestPointOnLine2D( const Vec2& referencePos, const Vec2& pointOnLine, const Vec2& anotherPointOnLine )
{
	Vec2 norm = anotherPointOnLine - pointOnLine;
	norm.Normalize();
	Vec2 displacement = referencePos - pointOnLine;
	Vec2 vecFromStartToClosestPoint = GetProjectedVector( displacement, norm );
	return vecFromStartToClosestPoint + pointOnLine; // put back in world space
}

//--------------------------------------------------------------------------
/**
* GetClosestPointOnLine3D
*/
Vec3 GetClosestPointOnLine3D( const Vec3& referencePos, const Vec3& pointOnLine, const Vec3& anotherPointOnLine )
{
	Vec3 norm = anotherPointOnLine - pointOnLine;
	norm.Normalize();
	Vec3 displacement = referencePos - pointOnLine;
	Vec3 vecFromStartToClosestPoint = GetProjectedVector( displacement, norm );
	return vecFromStartToClosestPoint + pointOnLine; // put back in world space
}

//--------------------------------------------------------------------------
/**
* GetClosestPointOnLineSegment2D
*/
Vec2 GetClosestPointOnLineSegment2D( const Vec2& point, const Vec2& lineStart, const Vec2& lineEnd )
{
	Vec2 SE = lineEnd - lineStart;
	Vec2 SP = point - lineStart;
	
	// If point in behind of start
	float dotPointNorm = DotProduct( SE, SP );
	if( dotPointNorm < 0.0f )
	{
		return lineStart;
	}

	Vec2 EP = point - lineEnd;
	// If point off end of segment
	float dotPointOppNorm = DotProduct( SE, EP );
	if( dotPointOppNorm < 0.0f )
	{
		return lineEnd;
	}

	Vec2 SENorm = SE.GetNormalized();
	Vec2 vecFromStartToClosestPoint = GetProjectedVector( SP, SENorm );
	Vec2 ClosestPointOnLine = vecFromStartToClosestPoint + lineStart;

	return ClosestPointOnLine;
}

//--------------------------------------------------------------------------
/**
* GetClosestPointOnLineSegment3D
*/
Vec3 GetClosestPointOnLineSegment3D( const Vec3& referencePos, const Vec3& lineStart, const Vec3& lineEnd )
{
	Vec3 SE = lineEnd - lineStart;
	Vec3 SP = referencePos - lineStart;

	// If point in behind of start
	float dotPointNorm = DotProduct( SE, SP );
	if( dotPointNorm < 0.0f )
	{
		return lineStart;
	}

	Vec3 EP = referencePos - lineEnd;
	// If point off end of segment
	float dotPointOppNorm = DotProduct( SE, EP );
	if( dotPointOppNorm < 0.0f )
	{
		return lineEnd;
	}

	Vec3 SENorm = SE.GetNormalized();
	Vec3 vecFromStartToClosestPoint = GetProjectedVector( SP, SENorm );
	Vec3 ClosestPointOnLine = vecFromStartToClosestPoint + lineStart;

	return ClosestPointOnLine;
}

//--------------------------------------------------------------------------
/**
* GetClosestPointOnDisc
*/
Vec2 GetClosestPointOnDisc( const Vec2& point, const Vec2& discCenter, float radius )
{
	Vec2 displacement = point - discCenter;
	float pointDistance = displacement.GetLength();
	if( pointDistance <= radius )
	{
		point;
	}
	displacement.SetLength( radius );
	return displacement;
}

//--------------------------------------------------------------------------
/**
* GetClosestPointOnCapsule2D
*/
Vec2 GetClosestPointOnCapsule2D( const Vec2& referencePos, const Vec2& capsuleStart, const Vec2& capsuleEnd, float capsuleRadius )
{
	Vec2 closestToStart			= GetClosestPointOnDisc( referencePos, capsuleStart, capsuleRadius );
	Vec2 closestToEnd			= GetClosestPointOnDisc( referencePos, capsuleEnd, capsuleRadius );
	Vec2 closestCenterToLine	= GetClosestPointOnLineSegment2D( referencePos, capsuleStart, capsuleEnd );

	Vec2 closestToCaps = referencePos - closestCenterToLine;
	closestToCaps.SetLength( capsuleRadius );
	closestToCaps = closestToCaps + closestCenterToLine;

	Vec2 dispStart = referencePos - closestToStart;
	Vec2 dispEnd = referencePos - closestToEnd;
	Vec2 dispCaps = referencePos - closestToCaps;

	float magStart = dispStart.GetLengthSquared();
	float magEnd = dispEnd.GetLengthSquared();
	float magCaps = dispCaps.GetLengthSquared();

	if( magStart < magEnd && magStart < magCaps )
	{
		return closestToStart;
	}
	else if( magEnd < magStart && magEnd < magCaps )
	{
		return closestToEnd;
	}
	else
	{
		return closestToCaps;
	}
}

//--------------------------------------------------------------------------
/**
* Clamp
*/
float Clamp( float valueToClamp, float minInclusive /*= 0.0f*/, float maxInclusive /*= 1.0f*/ )
{
	if( valueToClamp < minInclusive )
	{
		valueToClamp = minInclusive;
	}
	if( valueToClamp > maxInclusive )
	{
		valueToClamp = maxInclusive;
	}
	return valueToClamp;
}

//--------------------------------------------------------------------------
/**
* Clamp
*/
Vec2 Clamp( const Vec2& valueToClamp, const Vec2& minInclusive, const Vec2& maxInclusive )
{
	return Vec2( Clamp( valueToClamp.x, minInclusive.x, maxInclusive.x ), Clamp( valueToClamp.y, minInclusive.y, maxInclusive.y ) );
}

//--------------------------------------------------------------------------
/**
* Clamp
*/
int Clamp( int valueToClamp, int minInclusive /*= 0.0f*/, int maxInclusive /*= 1.0f*/ )
{
	if( valueToClamp < minInclusive )
	{
		valueToClamp = minInclusive;
	}
	if( valueToClamp > maxInclusive )
	{
		valueToClamp = maxInclusive;
	}
	return valueToClamp;
}

//--------------------------------------------------------------------------
/**
* Clamp
*/
Vec3 Clamp( const Vec3& valueToClamp, const Vec3& minInclusive, const Vec3& maxInclusive )
{
	return Vec3( Clamp( valueToClamp.x, minInclusive.x, maxInclusive.x ), Clamp( valueToClamp.y, minInclusive.y, maxInclusive.y ), Clamp( valueToClamp.z, minInclusive.z, maxInclusive.z ) );
}

//--------------------------------------------------------------------------
/**
* Clamp
*/
double Clamp( double valueToClamp, double minInclusive /*= 0.0f*/, double maxInclusive /*= 1.0f*/ )
{
	if( valueToClamp < minInclusive )
	{
		valueToClamp = minInclusive;
	}
	if( valueToClamp > maxInclusive )
	{
		valueToClamp = maxInclusive;
	}
	return valueToClamp;
}

//--------------------------------------------------------------------------
/**
* GetMin
*/
float GetMin( float a, float b )
{
	return a > b ? b : a;
}

//--------------------------------------------------------------------------
/**
* GetMin
*/
int GetMin( int a, int b )
{
	return a > b ? b : a;
}

//--------------------------------------------------------------------------
/**
* GetMax
*/
float GetMax( float a, float b )
{
	return a > b ? a : b;
}

//--------------------------------------------------------------------------
/**
* GetMax
*/
int GetMax( int a, int b )
{
	return a > b ? a : b;
}

//--------------------------------------------------------------------------
/**
* Lerp
*/
float Lerp( float start, float end, float percent )
{
	if( percent >= 1.0f )
	{
		return end;
	}
	if( percent <= 0.0f )
	{
		return start;
	}
	float range = end - start;
	float amountRange = range * percent;
	return amountRange + start;
}

//--------------------------------------------------------------------------
/**
* Lerp
*/
Vec2 Lerp( const Vec2& start, const Vec2& end, float percent )
{
	return Vec2( Lerp( start.x, end.x, percent ), Lerp( start.y, end.y, percent ) );
}

//--------------------------------------------------------------------------
/**
* Lerp
*/
Vec3 Lerp( const Vec3& start, const Vec3& end, float percent )
{
	return Vec3( Lerp( start.x, end.x, percent ), Lerp( start.y, end.y, percent ), Lerp( start.z, end.z, percent ) );
}

//--------------------------------------------------------------------------
/**
* Lerp
*/
Rgba Lerp( const Rgba& start, const Rgba& end, float percent )
{
	return Rgba( Lerp( start.r, end.r, percent ), Lerp( start.g, end.g, percent ), Lerp( start.b, end.b, percent ), Lerp( start.a, end.a, percent ) );
}


//--------------------------------------------------------------------------
/**
* GetRandomUINTInRange
*/
uint GetRandomUINTInRange(uint a, uint b)
{
	return s_utilRNGGenerator.GetRandomIntInRange( a, b );
}

//--------------------------------------------------------------------------
/**
* GetRandomFloatInRange
*/
float GetRandomFloatInRange(float a, float b)
{
	return s_utilRNGGenerator.GetRandomFloatInRange( a, b );
}

//--------------------------------------------------------------------------
/**
* GetRandomFloatFromZeroToOne
*/
float GetRandomFloatFromZeroToOne()
{
	return s_utilRNGGenerator.GetRandomFloatZeroToOne();
}

//--------------------------------------------------------------------------
/**
* GetRandomDirection2D
*/
Vec2 GetRandomDirection2D()
{
	Vec2 dir = Vec2::ZERO;
	while( dir.GetLengthSquared() <= .001f )
	{
		dir = Vec2( s_utilRNGGenerator.GetRandomFloatZeroToOne(), s_utilRNGGenerator.GetRandomFloatZeroToOne() );
	}
	dir.Normalize();
	return dir;
}

//--------------------------------------------------------------------------
/**
* GetRandomDirection3D
*/
Vec3 GetRandomDirection3D()
{
	Vec3 dir = Vec3::ZERO;
	while( dir.GetLengthSquared() >= .001f )
	{
		dir = Vec3( s_utilRNGGenerator.GetRandomFloatZeroToOne(), s_utilRNGGenerator.GetRandomFloatZeroToOne(), s_utilRNGGenerator.GetRandomFloatZeroToOne() );
	}
	dir.Normalize();
	return dir;
}

//--------------------------------------------------------------------------
/**
* PushDiscsOutOfEachother
*/
void PushDiscsOutOfEachother( Vec2& discAPosition, float discARadius, Vec2& discBPosition, float discBRadius )
{
	Vec2 displacementBetweenCenters = discBPosition - discAPosition;
	float centerDistanceMin = discARadius + discBRadius;
	float curDistance = displacementBetweenCenters.GetLength();
	if( curDistance < centerDistanceMin ) // need to push in direction of displacement vector
	{
		float amountToPush = centerDistanceMin - curDistance;
		displacementBetweenCenters.SetLength( amountToPush / 2.0f );

		// Push B 
		discBPosition += displacementBetweenCenters;
		// Push A in opposite direction.
		discAPosition -= displacementBetweenCenters;
	}
}

//--------------------------------------------------------------------------
/**
* PushDiscOutOfAnotherDisc
*/
void PushDiscOutOfAnotherDisc( Vec2& discAPosition, float discARadius, const Vec2& discBPosition, float discBRadius )
{
	Vec2 displacementBetweenCenters = discBPosition - discAPosition;
	float centerDistanceMin = discARadius + discBRadius;
	float curDistance = displacementBetweenCenters.GetLength();
	if( curDistance < centerDistanceMin ) // need to push in direction of displacement vector
	{
		float amountToPush = centerDistanceMin - curDistance;
		displacementBetweenCenters.SetLength( amountToPush );

		// Push A
		discAPosition -= displacementBetweenCenters;
	}
}

//--------------------------------------------------------------------------
/**
* IsBitFlagSet
*/
bool IsBitFlagSet( unsigned char bits, unsigned char bitFlags )
{
	return (bits & bitFlags) != 0;
}

//--------------------------------------------------------------------------
/**
* IsBitFlagSet
*/
bool IsBitFlagSet( unsigned short bits, unsigned short bitFlags )
{
	return (bits & bitFlags) != 0;
}

//--------------------------------------------------------------------------
/**
* IsBitFlagSet
*/
bool IsBitFlagSet( unsigned int bits, unsigned int bitFlags )
{
	return (bits & bitFlags) != 0;
}

//--------------------------------------------------------------------------
/**
* SetBitFlags
*/
void SetBitFlags( unsigned char& bits, unsigned char bitFlags )
{
	bits |= bitFlags;
}

//--------------------------------------------------------------------------
/**
* SetBitFlags
*/
void SetBitFlags( unsigned short& bits, unsigned short bitFlags )
{
	bits |= bitFlags;
}

//--------------------------------------------------------------------------
/**
* SetBitFlags
*/
void SetBitFlags( unsigned int& bits, unsigned int bitFlags )
{
	bits |= bitFlags;
}

//--------------------------------------------------------------------------
/**
* ToggleBitFlag
*/
void ToggleBitFlag( unsigned char& bits, unsigned char bitFlags )
{
	bits ^= bitFlags;
}

//--------------------------------------------------------------------------
/**
* ToggleBitFlag
*/
void ToggleBitFlag( unsigned short& bits, unsigned short bitFlags )
{
	bits ^= bitFlags;
}

//--------------------------------------------------------------------------
/**
* ToggleBitFlag
*/
void ToggleBitFlag( unsigned int& bits, unsigned int bitFlags )
{
	bits ^= bitFlags;
}

//--------------------------------------------------------------------------
/**
* ClearBitFlag
*/
void ClearBitFlag( unsigned char& bits, unsigned char bitFlags )
{
	bits &= ~bitFlags;
}

//--------------------------------------------------------------------------
/**
* ClearBitFlag
*/
void ClearBitFlag( unsigned short& bits, unsigned short bitFlags )
{
	bits &= ~bitFlags;
}

//--------------------------------------------------------------------------
/**
* ClearBitFlag
*/
void ClearBitFlag( unsigned int& bits, unsigned int bitFlags )
{
	bits &= ~bitFlags;
}

//--------------------------------------------------------------------------
/**
* Raycast
*/
uint Raycast( float *out, const Ray3& ray, Sphere const &sphere )
{
	// A = p0 - c
	// A*A + 2t * A*D + D*D * t*t = r*r
	// a = 1
	// b = 2 A*D
	// c = A*A - r*r
	Vec3 A = ray.m_start - sphere.m_center;
	Vec3 D = ray.m_dir.GetNormalized();
	float a = 1.0f;
	float b = 2.0f * DotProduct( A, ray.m_dir );
	float c = DotProduct( A, A ) - sphere.m_radius * sphere.m_radius;

	uint numHits = QuadraticFormula( out, a, b, c );
	for( uint idx = 0; idx < numHits; ++idx )
	{
		if( out[idx] < 0.0f )
		{
			out[idx] = 0.0f;
		}
	}
	return numHits;
}

//--------------------------------------------------------------------------
/**
* Raycast
*/
uint Raycast( float *out, const Ray3& ray, Capsule3 const &capsule )
{
	Vec3 normCapDir = (capsule.m_end - capsule.m_start).GetNormalized();

	float raycastResults[2];
	uint numCylinderIntersections = Raycast( raycastResults, ray, capsule.m_start, normCapDir, capsule.m_radius );

	float validTimes[6];
	uint curIndex = 0;
	// Clamp cylinder
	for( uint idxCyl = 0; idxCyl < numCylinderIntersections; ++idxCyl )
	{
		Vec3 curPoint = ray.GetPointFromTime( raycastResults[idxCyl] );
		Vec3 ps = capsule.m_start - curPoint;
		Vec3 pe = capsule.m_end - curPoint;
		float psDot = DotProduct( ps, normCapDir );
		float peDot = DotProduct( pe, normCapDir );
		if( psDot < 0.0f && peDot > 0.0f )
		{
			validTimes[curIndex++] = raycastResults[idxCyl];
		}
	}

	// Intersection of the cycles 
	uint numSphereIntersections = Raycast( raycastResults, ray, Sphere( capsule.m_start, capsule.m_radius ) );

	for( uint sphereIdx = 0; sphereIdx < numSphereIntersections; ++sphereIdx )
	{
		validTimes[curIndex++] = raycastResults[sphereIdx];
	}

	numSphereIntersections = Raycast( raycastResults, ray, Sphere( capsule.m_end, capsule.m_radius ) );
	for( uint sphereIdx = 0; sphereIdx < numSphereIntersections; ++sphereIdx )
	{
		validTimes[curIndex++] = raycastResults[sphereIdx];
	}

	// Early outs
	if( curIndex == 0 )
	{
		return 0;
	}
	if( curIndex == 1 )
	{
		out[0] = validTimes[0];
		return 1;
	}

	// Get min and max
	float& min = out[0];
	float& max = out[1];
	min = validTimes[0];
	max = validTimes[0];

	for( uint valIdx = 0; valIdx < curIndex; ++valIdx )
	{
		min = GetMin( min, validTimes[valIdx] );
		max = GetMax( max, validTimes[valIdx] );
	}

	return 2;
}

//--------------------------------------------------------------------------
/**
* Raycast
*/
uint Raycast( float *out, const Ray3& ray, Plane3 const &plane )
{
	// p*n = d
	// ( p0 + vt ) * n = d
	// p0*n + ( v * n )t = d
	// t = ( d - p0 * n ) / v * n
	float denom = DotProduct( ray.m_dir, plane.capsilated_data.m_normal );
	if( denom == 0.0f )
	{
		if( DotProduct( ray.m_start, plane.capsilated_data.m_normal ) == plane.capsilated_data.m_distance )
		{
			out[0] = 0.0f;
			return 1;
		}
		return 0;
	}
	float t = ( plane.capsilated_data.m_distance - DotProduct( ray.m_start, plane.capsilated_data.m_normal ) ) / denom;
	out[0] = t;
	return 1;
}

//--------------------------------------------------------------------------
/**
* Raycast
*/
uint Raycast( float *out, const Ray3& ray, OBB3 const &bounds )
{
	UNUSED(out);
	UNUSED(ray);
	UNUSED(bounds);
	return 0;
}

//--------------------------------------------------------------------------
/**
* Raycast
*/
uint Raycast( float *out, const Ray3& ray, const Vec3& posOnLine, const Vec3& dirOfLine, float radius )
{
	// p = p0 + vt
	// | s + ( ( p - s ) * n ) n - p | = r
	// | s + ( ( ( p0 + vt ) - s ) * n ) n - (p0 + vt) | = r
	// | t( (v*n)n - v ) + s + ( p0*n )n - (s*n)n - p0 | = r
	// a = (v*n)n - v
	// b = s + ( p0*n )n - (s*n)n - p0 
	// | ta + b | = r
	// ( ta + b ) * ( ta + b ) = r*r
	// t*t*a*a + 2t*a*b + b*b - r*r = 0
	// A = a*a
	// B = 2*a*b
	// C = b*b - r*r
	// t = QuadraticFormula( A, B, C )

	const Vec3& v = ray.m_dir;
	const Vec3& s = posOnLine;
	const Vec3& p0 = ray.m_start;
	Vec3 n = dirOfLine.GetNormalized();
	Vec3 a = ( DotProduct( v, n ) * n - v );
	Vec3 b = s + ( DotProduct( p0, n ) * n ) - ( DotProduct( s, n ) * n ) - p0;
	float A = DotProduct( a, a );
	float B = 2.0f * DotProduct( a, b );
	float C = DotProduct( b, b ) - radius * radius;
	return QuadraticFormula( out, A, B, C );
}

//--------------------------------------------------------------------------
/**
* ModPositive
*/
int ModPositive( int value, int modBy )
{
	if( value < 0 )
	{
		return ( value % modBy ) + modBy;
	}
	return value % modBy;
}

//--------------------------------------------------------------------------
/**
* RoundToNearestInt
*/
int RoundToNearestInt( float value )
{
	float val = value + 0.5f;
	return (int) val;
}

