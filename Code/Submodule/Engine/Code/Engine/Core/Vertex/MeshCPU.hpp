#pragma once
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex/VertexMaster.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Vec3.hpp"
#include <vector>



class MeshCPU
{
	friend class MeshGPU;
public:
	MeshCPU();
	explicit MeshCPU( MeshCPU* Mesh );
	~MeshCPU();
public:
	void Clear();         
	bool Empty();			

	void LoadFromObjFile( std::string fileName, bool invert = false, bool tangents = false, float scale = 1.0f, std::string transform = "x y z" );

	void SetColor( Rgba color ); 
	void SetNormal( Vec3 normal );
	void SetTanget( Vec4 tanget );
	void SetUV( Vec2 uv );                

	int AddVertex( const VertexMaster& m );     
	int AddVertex( const Vec3& pos );           

	void AddIndexedTriangle( int i0, int i1, int i2 );   
	void AddIndexedQuad( int topLeft, int topRight, int bottomLeft, int bottomRight );    
																	  
	int GetVertexCount() const { return (int) m_vertices.size(); };            
	int GetIndexCount() const { return (int) m_indices.size(); };                

	void RotateFromEuler( Vec3 euler, eRotationOrder rot );
	void Translate( const Vec3 translateBy );
	void UniformScale( float scale );

	void Flatten();
	bool ContainsTangents();

public:
	std::vector<VertexMaster>  m_vertices;       
	std::vector<unsigned int>  m_indices;        
	std::string m_defaultMaterial = "default"; 


	VertexMaster m_stamp; 
private:
	bool m_containsTangents = false;

};

void CPUMeshAddCylinder( MeshCPU* out, Vec3 p0, Vec3 p1, float p0radius, float p1radius, int numSides = 64, const Rgba& tint = Rgba::WHITE );
void CPUMeshAddRing3D( MeshCPU* out, Vec3 center, Vec3 eulerRot, float radius, float thickness, const Rgba& color = Rgba::WHITE, int numSides = 64 );
void CPUMeshAddCone( MeshCPU* out, Vec3 p0, Vec3 p1, float radius, int numSides = 64 );
void CPUMeshAddPlain( MeshCPU* out, AABB2 square, const Rgba& tint = Rgba::WHITE );
void CPUMeshAddLine3D( MeshCPU* out, Vec3 p0, Vec3 p1, float width );
void CPUMeshAddLine2D( MeshCPU* out, Vec2 p0, Vec2 p1, float width, const Rgba& tint = Rgba::WHITE );
void CPUMeshAddTriangle2D( MeshCPU* out, Vec2 p0, Vec2 p1, float width );
void CPUMeshAddCube( MeshCPU* out, const AABB3& box );    
void CPUMeshAddFrustum( MeshCPU* out, const Frustum& frustum );
void CPUMeshAddLinedFrustum( MeshCPU* out, const Frustum& frustum, float thickness = 0.1f );
void CPUMeshAddUVSphere( MeshCPU* out, Vec3 center, float radius, int wedges = 32, int slices = 16 );  
void CPUMeshAddCapsule( MeshCPU* out, const Vec3& start, const Vec3& end, float radius, int wedges = 32, int slices = 16 );
