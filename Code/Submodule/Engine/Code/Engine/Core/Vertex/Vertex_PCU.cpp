#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"

//--------------------------------------------------------------------------
const BufferAttribute Vertex_PCU::LAYOUT[] = {
	BufferAttribute( "POSITION",  RDF_VEC3,      offsetof(Vertex_PCU, position) ), 
	BufferAttribute( "COLOR",     RDF_RGBA32,    offsetof(Vertex_PCU, color) ), 
	BufferAttribute( "TEXCOORD",  RDF_VEC2,      offsetof(Vertex_PCU, uv) ), 
	BufferAttribute() // end
};


//--------------------------------------------------------------------------
/**
* Vertex_PCU
*/
Vertex_PCU::Vertex_PCU()
{

}

//--------------------------------------------------------------------------
/**
* ~Vertex_PCUVertex_PCU
*/
Vertex_PCU::Vertex_PCU( const Vec3& position, const Rgba& color, const Vec2& uvTexCoords )
{
	this->position = position;
	this->color = color;
	this->uv = uvTexCoords;
}

//--------------------------------------------------------------------------
/**
* ~Vertex_PCU
*/
Vertex_PCU::~Vertex_PCU()
{

}

//--------------------------------------------------------------------------
/**
* CopyFromMaster
*/
void Vertex_PCU::CopyFromMaster( void *buffer, VertexMaster const *src, unsigned int count )
{
	Vertex_PCU *dst = (Vertex_PCU*)buffer; 

	for ( unsigned int i = 0; i < count; ++i ) 
	{
		dst[i].position = src[i].position;
		dst[i].color = src[i].color; 
		dst[i].uv = src[i].uv; 
	}
}

//--------------------------------------------------------------------------
/**
* AddVertsForAABB2D
*/
void AddVertsForAABB2D( std::vector<Vertex_PCU>& addToVerts, const AABB2& aabb2Box, const Rgba& color, const Vec2& minUVs, const Vec2& maxUVs )
{
	Vec3 pointBL_pos( aabb2Box.m_bottomLeft.x	, aabb2Box.m_bottomLeft.y, 0.0f );
	Vec3 pointBR_pos( aabb2Box.m_topRight.x		, aabb2Box.m_bottomLeft.y, 0.0f );
	Vec3 pointTR_pos( aabb2Box.m_topRight.x		, aabb2Box.m_topRight.y, 0.0f );
	Vec3 pointTL_pos( aabb2Box.m_bottomLeft.x	, aabb2Box.m_topRight.y, 0.0f );

	Rgba pointBL_color = color;
	Rgba pointBR_color = color;
	Rgba pointTR_color = color;
	Rgba pointTL_color = color;

	Vec2 pointBL_UVs = Vec2( minUVs.x, minUVs.y );
	Vec2 pointBR_UVs = Vec2( maxUVs.x, minUVs.y );
	Vec2 pointTR_UVs = Vec2( maxUVs.x, maxUVs.y );
	Vec2 pointTL_UVs = Vec2( minUVs.x, maxUVs.y );

	Vertex_PCU pointBL( pointBL_pos, pointBL_color, pointBL_UVs );
	Vertex_PCU pointBR( pointBR_pos, pointBR_color, pointBR_UVs );
	Vertex_PCU pointTR( pointTR_pos, pointTR_color, pointTR_UVs );
	Vertex_PCU pointTL( pointTL_pos, pointTL_color, pointTL_UVs );

	addToVerts.push_back( pointBL );
	addToVerts.push_back( pointBR );
	addToVerts.push_back( pointTR );
	
	addToVerts.push_back( pointBL );
	addToVerts.push_back( pointTR );
	addToVerts.push_back( pointTL );
}

//--------------------------------------------------------------------------
/**
* AddVertsForQuad
* tl(3) - tr(2)
* |			| 
* bl(0) - br(1)
*/
void AddVertsForQuad( std::vector<Vertex_PCU>& addToVerts, const Vec3 quad[4], const Rgba& color, const Vec2& minUVs /*= Vec2::ZERO*/, const Vec2& maxUVs/*= Vec2::ONE */ )
{
	Vec3 pointBL_pos = quad[0];
	Vec3 pointBR_pos = quad[1];
	Vec3 pointTR_pos = quad[2];
	Vec3 pointTL_pos = quad[3];

	Rgba pointBL_color = color;
	Rgba pointBR_color = color;
	Rgba pointTR_color = color;
	Rgba pointTL_color = color;

	Vec2 pointBL_UVs = Vec2( minUVs.x, minUVs.y );
	Vec2 pointBR_UVs = Vec2( maxUVs.x, minUVs.y );
	Vec2 pointTR_UVs = Vec2( maxUVs.x, maxUVs.y );
	Vec2 pointTL_UVs = Vec2( minUVs.x, maxUVs.y );

	Vertex_PCU pointBL( pointBL_pos, pointBL_color, pointBL_UVs );
	Vertex_PCU pointBR( pointBR_pos, pointBR_color, pointBR_UVs );
	Vertex_PCU pointTR( pointTR_pos, pointTR_color, pointTR_UVs );
	Vertex_PCU pointTL( pointTL_pos, pointTL_color, pointTL_UVs );

	addToVerts.push_back( pointBL );
	addToVerts.push_back( pointBR );
	addToVerts.push_back( pointTR );

	addToVerts.push_back( pointBL );
	addToVerts.push_back( pointTR );
	addToVerts.push_back( pointTL );}

//--------------------------------------------------------------------------
/**
* AddVertsForLine2D
*/
void AddVertsForLine2D( std::vector<Vertex_PCU>& addToVerts, const Vec2& start, const Vec2& end, float thickness, const Rgba& color )
{
	float halfThickness = thickness / 2.0f;
	Vec2 displacement = end - start;
	displacement.SetLength( halfThickness );
	Vec2 rotated90Displacement = displacement.GetRotated90Degrees();

	Vec2 frontLeft	= end	+ displacement + rotated90Displacement;
	Vec2 frontRight = end	+ displacement - rotated90Displacement;
	Vec2 backLeft	= start - displacement + rotated90Displacement;
	Vec2 backRight	= start - displacement - rotated90Displacement;

	Vec3 pointBL_pos( backLeft.x	, backLeft.y	, 0.0f );
	Vec3 pointBR_pos( backRight.x	, backRight.y	, 0.0f );
	Vec3 pointTR_pos( frontRight.x	, frontRight.y	, 0.0f );
	Vec3 pointTL_pos( frontLeft.x	, frontLeft.y	, 0.0f );

	Rgba pointBL_color = color;
	Rgba pointBR_color = color;
	Rgba pointTR_color = color;
	Rgba pointTL_color = color;

	Vec2 pointBL_UVs = Vec2( 0.0f, 0.0f );
	Vec2 pointBR_UVs = Vec2( 0.0f, 0.0f );
	Vec2 pointTR_UVs = Vec2( 0.0f, 0.0f );
	Vec2 pointTL_UVs = Vec2( 0.0f, 0.0f );

	Vertex_PCU pointBL( pointBL_pos, pointBL_color, pointBL_UVs );
	Vertex_PCU pointBR( pointBR_pos, pointBR_color, pointBR_UVs );
	Vertex_PCU pointTR( pointTR_pos, pointTR_color, pointTR_UVs );
	Vertex_PCU pointTL( pointTL_pos, pointTL_color, pointTL_UVs );

	addToVerts.push_back( pointBL );
	addToVerts.push_back( pointBR );
	addToVerts.push_back( pointTR );

	addToVerts.push_back( pointBL );
	addToVerts.push_back( pointTR );
	addToVerts.push_back( pointTL );
}

//--------------------------------------------------------------------------
/**
* AddVertsForDisc2D
* Don't draw with no texture
*/
void AddVertsForDisc2D( std::vector<Vertex_PCU>& addToVerts, const Vec2& center, float radius, const Rgba& color, int numSides /*= 64 */ )
{
	int numDiscVerts = numSides * 3;

	float toAddDegrees = 360.0f / numSides;
	float theta = 0.0f;
	Vec2 zeroVec2( 0.0f, 0.0f ); // Disc drawn on no texture.

	for( int i = 0; i < numDiscVerts; i += 3 )
	{
		addToVerts.push_back( Vertex_PCU( Vec3( center.x, center.y, 0.0f ), color, zeroVec2 ) );

		Vec2 point2d = Vec2::MakeFromPolarDegrees(theta, radius);
		Vec3 point3d = Vec3( point2d.x, point2d.y, 0.0f );
		addToVerts.push_back( Vertex_PCU( Vec3( center.x, center.y, 0.0f ) + point3d , color, zeroVec2 ) );

		theta += toAddDegrees;
		point2d = Vec2::MakeFromPolarDegrees(theta, radius);
		point3d = Vec3( point2d.x, point2d.y, 0.0f );
		addToVerts.push_back( Vertex_PCU( Vec3( center.x, center.y, 0.0f ) + point3d , color, zeroVec2 ) );
	}
}

//--------------------------------------------------------------------------
/**
* AddVertsForRing2D
*/
void AddVertsForRing2D( std::vector<Vertex_PCU>& addToVerts, const Vec2& center, float radius, float thickness, const Rgba& color, int numSides /*= 64 */ )
{
	float toAddDegrees = 360.0f / numSides;
	float theta = 0.0f;
	Vec2 zeroVec2( 0.0f, 0.0f ); // Disc drawn on no texture.
	float halfThickness = thickness / 2.0f;
	Vec3 convertedCenter = Vec3( center.x, center.y, 0.0f );

	for( int i = 0; i < numSides; i++ )
	{
		Vec3 posPrevInner = ( Vec3( CosDegrees( theta ), SinDegrees( theta ), 0.0f ) * ( radius - halfThickness ) ) + convertedCenter;
		Vec3 posPrevOuter = ( Vec3( CosDegrees( theta ), SinDegrees( theta ), 0.0f ) * ( radius + halfThickness ) ) + convertedCenter;
		theta += toAddDegrees;
		Vec3 posNextInner = ( Vec3( CosDegrees( theta ), SinDegrees( theta ), 0.0f ) * ( radius - halfThickness ) ) + convertedCenter;
		Vec3 posNextOuter = ( Vec3( CosDegrees( theta ), SinDegrees( theta ), 0.0f ) * ( radius + halfThickness ) ) + convertedCenter;

		Vertex_PCU pointPrevInner = Vertex_PCU( posPrevInner, color, zeroVec2 );
		Vertex_PCU pointPrevOuter = Vertex_PCU( posPrevOuter, color, zeroVec2 );
		Vertex_PCU pointNextInner = Vertex_PCU( posNextInner, color, zeroVec2 );
		Vertex_PCU pointNextOuter = Vertex_PCU( posNextOuter, color, zeroVec2 );

		addToVerts.push_back( pointPrevInner );
		addToVerts.push_back( pointPrevOuter );
		addToVerts.push_back( pointNextOuter );

		addToVerts.push_back( pointPrevInner );
		addToVerts.push_back( pointNextOuter );
		addToVerts.push_back( pointNextInner );
	}
}

//--------------------------------------------------------------------------
/**
* TransformVertex2D
*/
void TransformVertex2D( Vertex_PCU& position, float uniformScale, float rotateDergees, const Vec2& translation )
{
	const Vec3& toChange = position.position;
	Vec3 newPos = TransformPosition( toChange, uniformScale, rotateDergees, Vec3( translation.x, translation.y, 0.0f ) );
	position.position = newPos;
}

//--------------------------------------------------------------------------
/**
* TransformVertexArray2D
*/
void TransformVertexArray2D( int sizeOfArray, Vertex_PCU* vertexArray, float uniformScale, float rotateDergees, const Vec2& translation )
{
	for( int i = 0; i < sizeOfArray; i++ )
	{
		TransformVertex2D(vertexArray[i], uniformScale, rotateDergees, translation);
	}
}
