#pragma once
#include "Engine/Core/Vertex/VertexMaster.hpp"

struct AABB2;


//--------------------------------------------------------------------------
//Represents a single point (vertex) of a simple 3D object intended to be rendered (drawn)
struct Vertex_PCU
{
public:
	Vertex_PCU();
	~Vertex_PCU();
	explicit Vertex_PCU( const Vec3& position, const Rgba& color, const Vec2& uvTexCoords );
public:
	Vec3 position;
	Rgba color;
	Vec2 uv;
public:
	static const BufferAttribute LAYOUT[];
	static void CopyFromMaster( void *buffer, VertexMaster const *src, unsigned int count );
};


void AddVertsForAABB2D( std::vector<Vertex_PCU>& addToVerts, const AABB2& aabb2Box, const Rgba& color, const Vec2& minUVs = Vec2::ZERO, const Vec2& maxUVs= Vec2::ONE );
void AddVertsForQuad( std::vector<Vertex_PCU>& addToVerts, const Vec3 quad[4], const Rgba& color, const Vec2& minUVs = Vec2::ZERO, const Vec2& maxUVs= Vec2::ONE );
void AddVertsForLine2D( std::vector<Vertex_PCU>& addToVerts, const Vec2& start, const Vec2& end, float thickness, const Rgba& color );
void AddVertsForDisc2D( std::vector<Vertex_PCU>& addToVerts, const Vec2& center, float radius, const Rgba& color, int numSides = 64 );
void AddVertsForRing2D( std::vector<Vertex_PCU>& addToVerts, const Vec2& center, float radius, float thickness, const Rgba& color, int numSides = 64 );

void TransformVertex2D( Vertex_PCU& position, float uniformScale, float rotateDergees, const Vec2& translation );
void TransformVertexArray2D( int sizeOfArray,  Vertex_PCU* vertexArray, float uniformScale, float rotateDergees, const Vec2& translation );
