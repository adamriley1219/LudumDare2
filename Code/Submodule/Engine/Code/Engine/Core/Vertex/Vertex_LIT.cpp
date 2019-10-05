#include "Engine/Core/Vertex/Vertex_LIT.hpp"


//--------------------------------------------------------------------------
const BufferAttribute Vertex_LIT::LAYOUT[] = {
	BufferAttribute( "POSITION",  RDF_VEC3,      offsetof(Vertex_LIT, position) ), 
	BufferAttribute( "COLOR",     RDF_RGBA32,    offsetof(Vertex_LIT, color) ), 
	BufferAttribute( "TEXCOORD",  RDF_VEC2,      offsetof(Vertex_LIT, uv) ), 
	BufferAttribute( "NORMAL",    RDF_VEC3,      offsetof(Vertex_LIT, normal) ), 
	BufferAttribute( "TANGENT",   RDF_VEC4,      offsetof(Vertex_LIT, tangent) ), 
	BufferAttribute() // end
};

//--------------------------------------------------------------------------
/**
* Vertex_LIT
*/
Vertex_LIT::Vertex_LIT()
{

}

//--------------------------------------------------------------------------
/**
* Vertex_LIT
*/
Vertex_LIT::Vertex_LIT( const Vec3& position, const Vec3& normal, const Vec4& tangent, const Rgba& color, const Vec2& uvTexCoords )
{
	this->position = position;
	this->normal = normal;
	this->color = color;
	this->uv = uvTexCoords;
	this->tangent = tangent;
}

//--------------------------------------------------------------------------
/**
* CopyFromMaster
*/
void Vertex_LIT::CopyFromMaster( void *buffer, VertexMaster const *src, unsigned int count )
{
	Vertex_LIT *dst = (Vertex_LIT*)buffer; 

	for ( unsigned int i = 0; i < count; ++i ) 
	{
		dst[i].position = src[i].position;
		dst[i].normal = src[i].normal;
		dst[i].tangent = src[i].tangent;
		dst[i].color = src[i].color; 
		dst[i].uv = src[i].uv; 
	}
}

//--------------------------------------------------------------------------
/**
* ~Vertex_LIT
*/
Vertex_LIT::~Vertex_LIT()
{

}
