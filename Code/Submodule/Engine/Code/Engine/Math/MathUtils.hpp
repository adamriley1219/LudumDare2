#pragma once
#include "Engine/Math/Pillbox2.hpp"
#include "Engine/Math/Matrix44.hpp"

#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Disc.hpp"

#include <math.h>

struct Vertex_PCU;
struct AABB2;
struct AABB3;
struct Manifold2D;
struct Rgba;
struct Ray3;
struct OBB3;
struct Capsule3;
struct Sphere;
struct Plane3;
struct Frustum;

typedef unsigned int uint;

constexpr float g_PI = 3.14159265359f;
constexpr float fSQRT_3_OVER_3 = 0.5773502691896257645091f;

Vec2 RotatePointAboutOrigin( const Vec2& point, float rotateDegrees );
float GetAngularDisplacement( float startDegrees, float goalDegrees );

float ConvertRadiansToDegrees( float radians );
float ConvertDegreesToRadians( float radians );
float CosDegrees( float degrees );
float ArccosDegrees( float range );
float SinDegrees( float degrees );
uint QuadraticFormula( float* out, float a, float b, float c );

float ATan2Degrees( float Y, float X );

float GetDistance( const Vec2& positionA, const Vec2& positionB );
float GetDistanceSquared( const Vec2& positionA, const Vec2& positionB );

float GetDistance( const Vec3& positionA, const Vec3& positionB );
float GetDistanceSquared( const Vec3& positionA, const Vec3& positionB );
float GetDistanceXY( const Vec3& positionA, const Vec3& positionB );
float GetDistanceXYSquared( const Vec3& positionA, const Vec3& positionB );

Vec3 GetCartesianFromSpherical( float radius, float theta, float phi );

// Overlap
bool IsPointInDisc( const Vec2& point, const Vec2& discCenter, float discRadius );
bool DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB );
bool DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB );
bool DoesDiscOverlapAABB2( const Vec2& discCenter, float discRadius, const AABB2& box );
bool DoesDiscOverlapLine2D( const Vec2& discCenter, float discRadius, const Vec2& pointOnLine, const Vec2& anotherPointOnLine );
bool DoesDiscOverlapLineSegment2D( const Vec2& discCenter, float discRadius, const Vec2& lineStart, const Vec2& lineEnd );
bool DoesDiscOverlapCapsule2D( const Vec2& discCenter, float discRadius, const Vec2& capsuleStart, const Vec2& capsuleEnd, float capsuleRadius );
bool DoesAABB2OverlapDisc( const AABB2& box, float discRadius, const Vec2& discCenter );
bool DoesAABB2OverlapAABB2( const AABB2& boxA, const AABB2& boxB );
bool DoesPillboxOverlapPillbox( const Pillbox2& pillA, const Pillbox2& pillB );
bool DoesCapsule3OverlapFrustum( const Capsule3& capsule, const Frustum& frustum );
bool DoesOBB3OverlapFrustum( const OBB3& box, const Frustum& frustum );

bool IsPointInCapsule2D( const Vec2& point, const Vec2& capsuleStart, const Vec2& capsuleEnd, float capsuleRadius );
bool IsPointOnOBB2( const Vec2& point, const OBB2& pill );

// bool GetManifold( Manifold2D* manifold, const AABB2& boxA, const AABB2& boxB );
bool GetManifold( Manifold2D* manifold, const Disc& disc, const AABB2& box );
bool GetManifold( Manifold2D* manifold, const Disc& discA, const Disc& discB );
bool GetManifold( Manifold2D* manifold, const OBB2& boxA, const OBB2& boxB );
bool GetManifold( Manifold2D* manifold, const Pillbox2& pillA, const Pillbox2& pillB );

Vec2 GetClosestPointOnDisc( const Vec2& point, const Vec2& discCenter, float radius );
Vec2 GetClosestPointOnAABB2( const Vec2& point, const AABB2& box );
bool GetClosestPointOnAABB2Edge( const Vec2& point, const AABB2& box, Vec2& result );
Vec2 GetClosestPointOnOBB2( const Vec2& point, const OBB2& box );
bool GetClosestPointOnOBB2Edge( const Vec2& point, const OBB2& box, Vec2& result );
Vec2 GetClosestPointOnLine2D( const Vec2& referencePos, const Vec2& pointOnLine, const Vec2& anotherPointOnLine );
Vec3 GetClosestPointOnLine3D( const Vec3& referencePos, const Vec3& pointOnLine, const Vec3& anotherPointOnLine );
Vec2 GetClosestPointOnLineSegment2D( const Vec2& referencePos, const Vec2& lineStart, const Vec2& lineEnd );
Vec3 GetClosestPointOnLineSegment3D( const Vec3& referencePos, const Vec3& lineStart, const Vec3& lineEnd );
Vec2 GetClosestPointOnCapsule2D( const Vec2& referencePos, const Vec2& capsuleStart, const Vec2& capsuleEnd, float capsuleRadius );


Vec2 TransformPosition( const Vec2& position, float uniformScale, float rotateDergeesAboutZ, const Vec2& translationXY );
Vec3 TransformPosition( const Vec3& position, float uniformScale, float rotateDergeesAboutZ, const Vec2& translationXY );
Vec3 TransformPosition( const Vec3& position, float uniformScale, float rotateDergeesAboutZ, const Vec3& translation );



float DotProduct(const Vec2& a, const Vec2& b);
float DotProduct(const Vec3& a, const Vec3& b);
float DotProduct(const Vec4& a, const Vec4& b);

Vec3 Cross( const Vec3& left, const Vec3& right );

// Smoothing

float SmoothStart2( float inputZeroToOne );
float SmoothStart3( float inputZeroToOne );
float SmoothStart4( float inputZeroToOne );
float SmoothStart5( float inputZeroToOne );

float SmoothStop2( float inputZeroToOne );
float SmoothStop3( float inputZeroToOne );
float SmoothStop4( float inputZeroToOne );
float SmoothStop5( float inputZeroToOne );

float SmoothStep3( float inputZeroToOne );
float SmoothStep5( float inputZeroToOne );

float GetProjectedLength( const Vec2& projectThis, const Vec2& onto );
Vec2 GetProjectedVector( const Vec2& projectThis, const Vec2& onto );
Vec3 GetProjectedVector( const Vec3& projectThis, const Vec3& onto );
Vec2 ReflectVectorOffSurfaceNormal( const Vec2& incomingVector, const Vec2& surfaceNormal );
 
float GetRayImpactFractionVsDisc2D( const Vec2& rayStart, const Vec2& rayNormalizedDir, float rayLength, const Vec2& discCenter, float discRadius );

float RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd );
Vec3 RangeMapFloat( const Vec3& inValue, float inStart, float inEnd, float outStart, float outEnd );


void PushDiscsOutOfEachother( Vec2& discAPosition, float discARadius, Vec2& discBPosition, float discBRadius );
void PushDiscOutOfAnotherDisc( Vec2& discAPosition, float discARadius, const Vec2& discBPosition, float discBRadius );

int ModPositive( int value, int modBy );
int RoundToNearestInt( float value );
float Clamp( float valueToClamp, float minInclusive = 0.0f, float maxInclusive = 1.0f);
double Clamp( double valueToClamp, double minInclusive = 0.0f, double maxInclusive = 1.0f);
int Clamp( int valueToClamp, int minInclusive = 0.0f, int maxInclusive = 1.0f);
Vec2 Clamp( const Vec2& valueToClamp, const Vec2& minInclusive, const Vec2& maxInclusive );
Vec3 Clamp( const Vec3& valueToClamp, const Vec3& minInclusive, const Vec3& maxInclusive );
float GetMin( float a, float b );
int GetMin( int a, int b );
float GetMax( float a, float b );
int GetMax( int a, int b );
float Lerp( float start, float end, float percent );
Vec2 Lerp( const Vec2& start, const Vec2& end, float percent );
Vec3 Lerp( const Vec3& start, const Vec3& end, float percent );
Rgba Lerp( const Rgba& start, const Rgba& end, float percent );

uint GetRandomUINTInRange( uint a, uint b );
float GetRandomFloatInRange( float a, float b );
float GetRandomFloatFromZeroToOne();
Vec2 GetRandomDirection2D();
Vec3 GetRandomDirection3D();

// Bit functions
bool IsBitFlagSet( unsigned char bits, unsigned char bitFlags );
bool IsBitFlagSet( unsigned short bits, unsigned short bitFlags );
bool IsBitFlagSet( unsigned int bits, unsigned int bitFlags );
void SetBitFlags( unsigned char& bits, unsigned char bitFlags );
void SetBitFlags( unsigned short& bits, unsigned short bitFlags );
void SetBitFlags( unsigned int& bits, unsigned int bitFlags );
void ToggleBitFlag( unsigned char& bits, unsigned char bitFlags );
void ToggleBitFlag( unsigned short& bits, unsigned short bitFlags );
void ToggleBitFlag( unsigned int& bits, unsigned int bitFlags );
void ClearBitFlag( unsigned char& bits, unsigned char bitFlags );
void ClearBitFlag( unsigned short& bits, unsigned short bitFlags );
void ClearBitFlag( unsigned int& bits, unsigned int bitFlags );


// Raycast
uint Raycast( float *out, const Ray3& ray, Sphere const &sphere );											// sphere
uint Raycast( float *out, const Ray3& ray, const Vec3& posOnLine, const Vec3& dirOfLine, float radius );	// infinite line with radius ( cylinder with no start or end )
uint Raycast( float *out, const Ray3& ray, Capsule3 const &capsule );										// capsule
uint Raycast( float *out, const Ray3& ray, Plane3 const &plane );											// plane
uint Raycast( float *out, const Ray3& ray, OBB3 const &bounds );											// obb3