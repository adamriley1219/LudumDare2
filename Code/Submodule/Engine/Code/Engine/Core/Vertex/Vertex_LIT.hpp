#pragma once
#include "Engine/Core/Vertex/VertexMaster.hpp"



//--------------------------------------------------------------------------
//Represents a single point (vertex) of a simple 3D object intended to be rendered (drawn)
//--------------------------------------------------------------------------
struct Vertex_LIT
{
public:
	Vertex_LIT();
	~Vertex_LIT();
	explicit Vertex_LIT( const Vec3& position, const Vec3& normal, const Vec4& tangent, const Rgba& color, const Vec2& uvTexCoords );
public:
	Vec3 position;
	Rgba color;
	Vec2 uv;
	Vec3 normal;
	Vec4 tangent;

public:
	static const BufferAttribute LAYOUT[]; 
	static void CopyFromMaster( void *dst, VertexMaster const *src, unsigned int count ); 

};


