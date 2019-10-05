#include "Engine/Core/Vertex/MeshCPU.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Frustum.hpp"
#include "Engine/Core/EngineCommon.hpp"


//--------------------------------------------------------------------------
/**
* MeshCPU
*/
MeshCPU::MeshCPU()
{
	m_stamp.color		= Rgba::WHITE;
	m_stamp.position	= Vec3::ZERO;
	m_stamp.uv			= Vec2::ZERO;
	m_stamp.normal		= Vec3( 0.0f, 0.0f, 1.0f );
	m_stamp.tangent		= Vec4( 0.0f, 0.0f, 1.0f, 1.0f );
}

//--------------------------------------------------------------------------
/**
* MeshCPU
*/
MeshCPU::MeshCPU( MeshCPU* Mesh )
{
	m_stamp = Mesh->m_stamp;
	for( int vertIndex = 0; vertIndex < (int) Mesh->m_vertices.size(); ++vertIndex )
	{
		m_vertices.push_back( Mesh->m_vertices[vertIndex] );
	}
	for( int indIndex = 0; indIndex < (int) Mesh->m_indices.size(); ++indIndex )
	{
		m_indices.push_back( Mesh->m_indices[indIndex] );
	}
	m_defaultMaterial = Mesh->m_defaultMaterial;
}

//--------------------------------------------------------------------------
/**
* ~MeshCPU
*/
MeshCPU::~MeshCPU()
{

}

//--------------------------------------------------------------------------
/**
* Clear
*/
void MeshCPU::Clear()
{
	m_vertices.clear();
	m_indices.clear();
	m_stamp.color		= Rgba::WHITE;
	m_stamp.position	= Vec3::ZERO;
	m_stamp.uv			= Vec2::ZERO;
}

//--------------------------------------------------------------------------
/**
* Empty
*/
bool MeshCPU::Empty()
{
	return m_vertices.size() == 0;
}

//--------------------------------------------------------------------------
/**
* LoadFromObjFile
*/
void MeshCPU::LoadFromObjFile(std::string fileName, bool invert /*= false*/, bool tangents /*= false*/, float scale /*= 1.0f*/, std::string transform /*= "x y z" */)
{
	char* data = nullptr;
	unsigned long int sizeFile = LoadFile( fileName.c_str(), data ); 
	if( sizeFile <= 0 )
	{
		return;
	}
	std::vector<std::string> lines = SplitStringOnDelitmiter( data, "\n\r" );

	std::vector<Vec3> vertPos;
	std::vector<Vec3> vertNorm;
	std::vector<Vec2> vertTex;

	Matrix44 transfromMat = GetTransfromFromString( transform );

	m_containsTangents = tangents;

	for( unsigned int lineIdx = 0; lineIdx < ( unsigned int ) lines.size(); ++lineIdx )
	{
		std::vector<std::string> splitLine = SplitStringOnDelitmiter( lines[lineIdx].c_str(), " " );
		if( splitLine[0] == "v" )
		{
			vertPos.push_back( transfromMat.TransformPosition3D( Vec3( std::stof( splitLine[1] ), std::stof( splitLine[2] ), std::stof( splitLine[3] ) ) ) );
		}
		if( splitLine[0] == "vn" )
		{
			vertNorm.push_back( transfromMat.TransformVec3D( Vec3( std::stof( splitLine[1] ), std::stof( splitLine[2] ), std::stof( splitLine[3] ) ) * ( invert ? -1.0f : 1.0f ) ) );
		}
		if( splitLine[0] == "vt" && tangents )
		{
			vertTex.push_back( Vec2( std::stof( splitLine[1] ), std::stof( splitLine[2] ) ) );
		}

		// All v, vn, and vt must now be parsed
		if( splitLine[0] == "f" )
		{		
			unsigned int indices[4];
			for( unsigned int cornerIdx = 0; cornerIdx < (unsigned int) splitLine.size() - 1; ++cornerIdx )
			{
				std::vector<std::string> vertIdxes = SplitStringOnDelitmiter( splitLine[cornerIdx + 1].c_str(), "/" );
				if( tangents )
				{
					int normIdx = std::stoi( vertIdxes[ (unsigned int) vertIdxes.size() > 2 ? 2 : 1 ] );
					if( normIdx < 0 )
					{
						normIdx = (int) vertNorm.size() + normIdx;
					}
					else
					{
						--normIdx;
					}
					SetNormal( vertNorm[ normIdx ] );
					SetTanget( Vec4( Cross( m_stamp.normal, Vec3( 0.0f, 1.0f, 0.0f ) != m_stamp.normal ? Vec3( 0.0f, 1.0f, 0.0f ) : Vec3( 1.0f, 0.0f, 0.0f )  ), 1.0f ) );
				}

				if( (unsigned int) vertIdxes.size() > 2 )
				{
					int uvIdx = std::stoi( vertIdxes[1] );
					if( uvIdx < 0 )
					{
						uvIdx = (int) vertTex.size() + uvIdx;
					}
					else
					{
						--uvIdx;
					}
					SetUV( vertTex[ uvIdx ] );
				}

				int posIdx = std::stoi( vertIdxes[0] );
				if( posIdx < 0 )
				{
					posIdx = (int) vertPos.size() + posIdx;
				}
				else
				{
					--posIdx;
				}
				indices[cornerIdx] = AddVertex( vertPos[ posIdx ] );
			}

			// quad
			if( splitLine.size() == 5 )
			{
				if( invert )
				{
					AddIndexedQuad( indices[2], indices[3], indices[1], indices[0] );
				}
				else
				{
					AddIndexedQuad( indices[3], indices[2], indices[0], indices[1] );
				}
			}

			// triangle
			if( splitLine.size() == 4 )
			{
				if( invert )
				{
					AddIndexedTriangle( indices[2], indices[1], indices[0] );
				}
				else
				{
					AddIndexedTriangle( indices[0], indices[1], indices[2] );
				}
			}
		}
	}
	UniformScale( scale );
}

//--------------------------------------------------------------------------
/**
* SetColor
*/
void MeshCPU::SetColor( Rgba color )
{
	m_stamp.color = color;
}

//--------------------------------------------------------------------------
/**
* SetNormal
*/
void MeshCPU::SetNormal( Vec3 normal )
{
	m_stamp.normal = normal;
}

//--------------------------------------------------------------------------
/**
* SetTanget
*/
void MeshCPU::SetTanget( Vec4 tanget )
{
	m_stamp.tangent = tanget;
}

//--------------------------------------------------------------------------
/**
* SetUV
*/
void MeshCPU::SetUV( Vec2 uv )
{
	m_stamp.uv = uv;
}

//--------------------------------------------------------------------------
/**
* AddVertex
*/
int MeshCPU::AddVertex( const VertexMaster& m )
{
	m_vertices.push_back(m);
	return (int) m_vertices.size() - 1;
}

//--------------------------------------------------------------------------
/**
* AddVertex
*/
int MeshCPU::AddVertex( const Vec3& pos )
{
	VertexMaster vert;
	vert.position = pos;
	vert.color = m_stamp.color;
	vert.normal = m_stamp.normal;
	vert.tangent = m_stamp.tangent;
	vert.uv = m_stamp.uv;

	m_vertices.push_back( vert );
	return (int) m_vertices.size() - 1;
}

//--------------------------------------------------------------------------
/**
* AddIndexedTriangle
*/
void MeshCPU::AddIndexedTriangle( int i0, int i1, int i2 )
{
	m_indices.push_back( i0 );
	m_indices.push_back( i1 );
	m_indices.push_back( i2 );
}

//--------------------------------------------------------------------------
/**
* AddIndexedQuad
*/
void MeshCPU::AddIndexedQuad( int topLeft, int topRight, int bottomLeft, int bottomRight )
{
	AddIndexedTriangle( bottomLeft, topRight, topLeft );
	AddIndexedTriangle( bottomLeft, bottomRight, topRight );
}


//--------------------------------------------------------------------------
/**
* RotateFromEuler
*/
void MeshCPU::RotateFromEuler( Vec3 euler, eRotationOrder rot )
{
	Matrix44 rotationMat = Matrix44::FromEuler( euler, rot );

	for( int vertIndex = 0; vertIndex < (int) m_vertices.size(); vertIndex++ )
	{
		m_vertices[vertIndex].position = rotationMat.TransformPosition3D( m_vertices[vertIndex].position );
	}
}

//--------------------------------------------------------------------------
/**
* Translate
*/
void MeshCPU::Translate(const Vec3 translateBy)
{
	Matrix44 rotationMat = Matrix44::MakeTranslation3D( translateBy );

	for( int vertIndex = 0; vertIndex < (int) m_vertices.size(); vertIndex++ )
	{
		m_vertices[vertIndex].position = rotationMat.TransformPosition3D( m_vertices[vertIndex].position );
	}
}

//--------------------------------------------------------------------------
/**
* UniformScale
*/
void MeshCPU::UniformScale( float scale )
{
	Matrix44 rotationMat = Matrix44::MakeUniformScale3D( scale );

	for( int vertIndex = 0; vertIndex < (int) m_vertices.size(); vertIndex++ )
	{
		m_vertices[vertIndex].position = rotationMat.TransformPosition3D( m_vertices[vertIndex].position );
	}
}

//--------------------------------------------------------------------------
/**
* Flatten
*/
void MeshCPU::Flatten()
{
	for( int vertIndex = 0; vertIndex < (int) m_vertices.size(); vertIndex++ )
	{
		m_vertices[vertIndex].position = Vec3( m_vertices[vertIndex].position.x, m_vertices[vertIndex].position.y, 0.0f );
	}
}

//--------------------------------------------------------------------------
/**
* ContainsTangents
*/
bool MeshCPU::ContainsTangents()
{
	return m_containsTangents;
}

// --- Could be useful, Never used it ---
// static void AddTriToMesh( MeshCPU* out, VertexMaster botLeft, VertexMaster botRight, VertexMaster topRight )
// {
// 	int bLeft		= out->AddVertex( botLeft );
// 	int bRight		= out->AddVertex( botRight );
// 	int tRight		= out->AddVertex( topRight );
// 	out->AddIndexedTriangle( bLeft, bRight, tRight );
// }

static void AddQuadToMesh( MeshCPU *out, VertexMaster topLeft, VertexMaster topRight, VertexMaster botLeft, VertexMaster botRight )
{
	int tLeft		= out->AddVertex( topLeft );
	int tRight		= out->AddVertex( topRight );
	int bLeft		= out->AddVertex( botLeft );
	int bRight		= out->AddVertex( botRight );
	out->AddIndexedQuad( tLeft, tRight, bLeft, bRight );
}



//--------------------------------------------------------------------------
/**
* CPUMeshAddCylinder
*/
void CPUMeshAddCylinder( MeshCPU* out, Vec3 p0, Vec3 p1, float p0radius, float p1radius, int numSides /*= 64 */, const Rgba& tint /*= Rgba::WHITE*/ )
{
	float toAddDegrees = -360.0f / numSides;
	float length = GetDistance( p0, p1 );

	Vec3 startingbotPoint( 0.0f, -p0radius, 0.0f );
	Vec3 startingbtopPoint( length, -p1radius, 0.0f );

	Matrix44 curCol = Matrix44::IDENTITY;
	Vec3 disp = p1 - p0;
	Vec3 right = disp.GetNormalized();

	Matrix44 transformMat;
	if( right == Vec3( 1.0f, 0.0f, 0.0f ) )
	{
		transformMat = Matrix44::IDENTITY;
	}
	else if( right == Vec3( -1.0f, 0.0f, 0.0f ) )
	{
		transformMat;
		transformMat.m_values[Matrix44::Ix] = -1.0f;
		transformMat.m_values[Matrix44::Jy] = -1.0f;
		transformMat.m_values[Matrix44::Kz] = -1.0f;
	}
	else
	{
		transformMat = Matrix44::MakeRotationAboutAxis( Cross( Vec3( 1.0f, 0.0f, 0.0f ), right ), ArccosDegrees( DotProduct( right, Vec3( 1.0f, 0.0f, 0.0f ) ) ) );
	}
	transformMat.SetT( p0 );


	VertexMaster tMidVert;
	VertexMaster bMidVert;
	tMidVert.position = transformMat.TransformPosition3D( Vec3( length, 0.0f, 0.0f ) );
	bMidVert.position = transformMat.TransformPosition3D( Vec3::ZERO );
	int tMidIdx = out->AddVertex( tMidVert );
	int bMidIdx = out->AddVertex( bMidVert );


	for( int sideIndex = 0; sideIndex < numSides; ++sideIndex )
	{
		Matrix44 nextCol = Matrix44::FromEuler( Vec3( toAddDegrees * ( sideIndex + 1 ), 0.0f, 0.0f ), ROTATION_ORDER_ZXY );
		
		Vec3 botLeft = curCol.TransformPosition3D( startingbotPoint );
		Vec3 botRight = nextCol.TransformPosition3D( startingbotPoint );
		Vec3 topLeft = curCol.TransformPosition3D( startingbtopPoint );
		Vec3 topRight = nextCol.TransformPosition3D( startingbtopPoint );

		VertexMaster tLeft;
		VertexMaster tRight;
		VertexMaster bLeft;
		VertexMaster bRight;

		tLeft.uv			= Vec2( 0.0f, 0.0f );
		tRight.uv			= Vec2( 1.0f, 0.0f );
		bLeft.uv			= Vec2( 0.0f, 1.0f );
		bRight.uv			= Vec2( 1.0f, 1.0f );

		tLeft.color			= tint;
		tRight.color		= tint;
		bLeft.color			= tint;
		bRight.color		= tint;

		// face
		tLeft.position		= transformMat.TransformPosition3D( topLeft );
		tRight.position		= transformMat.TransformPosition3D( topRight );
		bLeft.position		= transformMat.TransformPosition3D( botLeft );
		bRight.position		= transformMat.TransformPosition3D( botRight );

		int tLeftIdx		= out->AddVertex( tLeft );
		int tRightIdx		= out->AddVertex( tRight );
		int bLeftIdx		= out->AddVertex( bLeft );
		int bRightIdx		= out->AddVertex( bRight );

		out->AddIndexedQuad( tLeftIdx, tRightIdx, bLeftIdx, bRightIdx );
		out->AddIndexedTriangle( bMidIdx, bRightIdx, bLeftIdx );
		out->AddIndexedTriangle( tMidIdx, tLeftIdx, tRightIdx );

		curCol = nextCol;
	}


}

//--------------------------------------------------------------------------
/**
* CPUMeshAddRing3D
*/
void CPUMeshAddRing3D(MeshCPU* out, Vec3 center, Vec3 eulerRot, float radius, float thickness, const Rgba& color /*= Rgba::WHITE*/, int numSides /*= 64 */)
{
	Matrix44 mat =  Matrix44::MakeTranslation3D( center ) * Matrix44::FromEuler( eulerRot, ROTATION_ORDER_ZXY );


	float toAddDegrees = 360.0f / numSides;
	float theta = 0.0f;
	Vec2 zeroVec2( 0.0f, 0.0f ); // Disc drawn on no texture.
	float halfThickness = thickness / 2.0f;

	for( int i = 0; i < numSides; i++ )
	{
		Vec3 posPrevInner = ( Vec3( CosDegrees( theta ), SinDegrees( theta ), 0.0f ) * ( radius - halfThickness ) );
		Vec3 posPrevOuter = ( Vec3( CosDegrees( theta ), SinDegrees( theta ), 0.0f ) * ( radius + halfThickness ) );
		theta += toAddDegrees;
		Vec3 posNextInner = ( Vec3( CosDegrees( theta ), SinDegrees( theta ), 0.0f ) * ( radius - halfThickness ) );
		Vec3 posNextOuter = ( Vec3( CosDegrees( theta ), SinDegrees( theta ), 0.0f ) * ( radius + halfThickness ) );

		posPrevInner = mat.TransformPosition3D( posPrevInner );
		posPrevOuter = mat.TransformPosition3D( posPrevOuter );
		posNextInner = mat.TransformPosition3D( posNextInner );
		posNextOuter = mat.TransformPosition3D( posNextOuter );


		VertexMaster pointPrevInner;
		VertexMaster pointPrevOuter;
		VertexMaster pointNextInner;
		VertexMaster pointNextOuter;

		pointPrevInner.color = color;
		pointPrevOuter.color = color;
		pointNextInner.color = color;
		pointNextOuter.color = color;

		pointPrevInner.position = posPrevInner;
		pointPrevOuter.position = posPrevOuter;
		pointNextInner.position = posNextInner;
		pointNextOuter.position = posNextOuter;

		int tLeftIdx		= out->AddVertex( pointNextOuter );
		int tRightIdx		= out->AddVertex( pointNextInner );
		int bLeftIdx		= out->AddVertex( pointPrevOuter );
		int bRightIdx		= out->AddVertex( pointPrevInner );

		out->AddIndexedQuad( tLeftIdx, tRightIdx, bLeftIdx, bRightIdx );
	}

}

//--------------------------------------------------------------------------
/**
* CPUMeshAddCone
*/
void CPUMeshAddCone( MeshCPU* out, Vec3 p0, Vec3 p1, float radius, int numSides /*= 64 */ )
{
	CPUMeshAddCylinder( out, p0, p1, radius, 0.0f, numSides );
}

//--------------------------------------------------------------------------
/**
* CPUMeshAddPlain
*/
void CPUMeshAddPlain( MeshCPU* out, AABB2 square, const Rgba& tint /*= Rgba::WHITE*/ )
{
	VertexMaster tLeft;
	VertexMaster tRight;
	VertexMaster bLeft;
	VertexMaster bRight;

	tLeft.uv			= Vec2( 0.0f, 0.0f );
	tRight.uv			= Vec2( 1.0f, 0.0f );
	bLeft.uv			= Vec2( 0.0f, 1.0f );
	bRight.uv			= Vec2( 1.0f, 1.0f );

	tLeft.color			= tint;
	tRight.color		= tint;
	bLeft.color			= tint;
	bRight.color		= tint;


	// Front
	tLeft.normal		= Vec3( 0.0f, 0.0f, -1.0f );
	tRight.normal		= Vec3( 0.0f, 0.0f, -1.0f );
	bLeft.normal		= Vec3( 0.0f, 0.0f, -1.0f );
	bRight.normal		= Vec3( 0.0f, 0.0f, -1.0f );

	tLeft.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	tRight.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	bLeft.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	bRight.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );

	tLeft.position		= Vec3( square.m_bottomLeft.x,	square.m_topRight.y, 0.0f );
	tRight.position		= Vec3( square.m_topRight.x,	square.m_topRight.y, 0.0f );
	bLeft.position		= Vec3( square.m_bottomLeft.x,	square.m_bottomLeft.y, 0.0f );
	bRight.position		= Vec3( square.m_topRight.x,	square.m_bottomLeft.y, 0.0f );
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );
}

//--------------------------------------------------------------------------
/**
* CPUMeshAddLine3D
*/
void CPUMeshAddLine3D( MeshCPU* out, Vec3 p0, Vec3 p1, float width )
{
	CPUMeshAddCylinder( out, p0, p1, width, width, 4 );
}

//--------------------------------------------------------------------------
/**
* CPUMeshAddLine2D
*/
void CPUMeshAddLine2D( MeshCPU* out, Vec2 p0, Vec2 p1, float width, const Rgba& tint /*= Rgba::WHITE*/ )
{
	CPUMeshAddCylinder( out, Vec3( p0.x, p0.y, 0.0f ), Vec3( p1.x, p1.y, 0.0f ), width, width, 4, tint );
	out->Flatten();
}

//--------------------------------------------------------------------------
/**
* CPUMeshAddTriangle2D
*/
void CPUMeshAddTriangle2D( MeshCPU* out, Vec2 p0, Vec2 p1, float width )
{
	CPUMeshAddCylinder( out, Vec3( p0.x, p0.y, 0.0f ), Vec3( p1.x, p1.y, 0.0f ), width, 0.0f, 4 );
	out->Flatten();
}

//--------------------------------------------------------------------------
/**
* CPUMeshAddCube
*/
void CPUMeshAddCube( MeshCPU *out, const AABB3& box )
{
	VertexMaster tLeft;
	VertexMaster tRight;
	VertexMaster bLeft;
	VertexMaster bRight;

	tLeft.uv			= Vec2( 0.0f, 0.0f );
	tRight.uv			= Vec2( 1.0f, 0.0f );
	bLeft.uv			= Vec2( 0.0f, 1.0f );
	bRight.uv			= Vec2( 1.0f, 1.0f );


	// Front
	tLeft.normal		= Vec3( 0.0f, 0.0f, -1.0f );
	tRight.normal		= Vec3( 0.0f, 0.0f, -1.0f );
	bLeft.normal		= Vec3( 0.0f, 0.0f, -1.0f );
	bRight.normal		= Vec3( 0.0f, 0.0f, -1.0f );

	tLeft.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	tRight.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	bLeft.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	bRight.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );

	tLeft.position		= Vec3( box.m_mins.x, box.m_maxs.y, box.m_mins.z );
	tRight.position		= Vec3( box.m_maxs.x, box.m_maxs.y, box.m_mins.z );
	bLeft.position		= Vec3( box.m_mins.x, box.m_mins.y, box.m_mins.z );
	bRight.position		= Vec3( box.m_maxs.x, box.m_mins.y, box.m_mins.z );
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );

	// Back
	tLeft.normal		= Vec3( 0.0f, 0.0f, 1.0f );
	tRight.normal		= Vec3( 0.0f, 0.0f, 1.0f );
	bLeft.normal		= Vec3( 0.0f, 0.0f, 1.0f );
	bRight.normal		= Vec3( 0.0f, 0.0f, 1.0f );

	tLeft.tangent		= Vec4( -1.0f, 0.0f, 0.0f, 1.0f );
	tRight.tangent		= Vec4( -1.0f, 0.0f, 0.0f, 1.0f );
	bLeft.tangent		= Vec4( -1.0f, 0.0f, 0.0f, 1.0f );
	bRight.tangent		= Vec4( -1.0f, 0.0f, 0.0f, 1.0f );

	tLeft.position		= Vec3( box.m_maxs.x, box.m_maxs.y, box.m_maxs.z );
	tRight.position		= Vec3( box.m_mins.x, box.m_maxs.y, box.m_maxs.z );
	bLeft.position		= Vec3( box.m_maxs.x, box.m_mins.y, box.m_maxs.z );
	bRight.position		= Vec3( box.m_mins.x, box.m_mins.y, box.m_maxs.z );
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );

	// Left
	tLeft.normal		= Vec3( -1.0f, 0.0f, 0.0f );
	tRight.normal		= Vec3( -1.0f, 0.0f, 0.0f );
	bLeft.normal		= Vec3( -1.0f, 0.0f, 0.0f );
	bRight.normal		= Vec3( -1.0f, 0.0f, 0.0f );

	tLeft.tangent		= Vec4( 0.0f, 0.0f, -1.0f, 1.0f );
	tRight.tangent		= Vec4( 0.0f, 0.0f, -1.0f, 1.0f );
	bLeft.tangent		= Vec4( 0.0f, 0.0f, -1.0f, 1.0f );
	bRight.tangent		= Vec4( 0.0f, 0.0f, -1.0f, 1.0f );

	tLeft.position		= Vec3( box.m_mins.x, box.m_maxs.y, box.m_maxs.z );
	tRight.position		= Vec3( box.m_mins.x, box.m_maxs.y, box.m_mins.z );
	bLeft.position		= Vec3( box.m_mins.x, box.m_mins.y, box.m_maxs.z );
	bRight.position		= Vec3( box.m_mins.x, box.m_mins.y, box.m_mins.z );
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );

	// Right
	tLeft.normal		= Vec3( 1.0f, 0.0f, 0.0f );
	tRight.normal		= Vec3( 1.0f, 0.0f, 0.0f );
	bLeft.normal		= Vec3( 1.0f, 0.0f, 0.0f );
	bRight.normal		= Vec3( 1.0f, 0.0f, 0.0f );

	tLeft.tangent		= Vec4( 0.0f, 0.0f, 1.0f, 1.0f );
	tRight.tangent		= Vec4( 0.0f, 0.0f, 1.0f, 1.0f );
	bLeft.tangent		= Vec4( 0.0f, 0.0f, 1.0f, 1.0f );
	bRight.tangent		= Vec4( 0.0f, 0.0f, 1.0f, 1.0f );

	tLeft.position		= Vec3( box.m_maxs.x, box.m_maxs.y, box.m_mins.z );
	tRight.position		= Vec3( box.m_maxs.x, box.m_maxs.y, box.m_maxs.z );
	bLeft.position		= Vec3( box.m_maxs.x, box.m_mins.y, box.m_mins.z );
	bRight.position		= Vec3( box.m_maxs.x, box.m_mins.y, box.m_maxs.z );
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );

	// Top
	tLeft.normal		= Vec3( 0.0f, 1.0f, 0.0f );
	tRight.normal		= Vec3( 0.0f, 1.0f, 0.0f );
	bLeft.normal		= Vec3( 0.0f, 1.0f, 0.0f );
	bRight.normal		= Vec3( 0.0f, 1.0f, 0.0f );

	tLeft.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	tRight.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	bLeft.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	bRight.tangent		= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );


	tLeft.position		= Vec3( box.m_mins.x, box.m_maxs.y, box.m_maxs.z );
	tRight.position		= Vec3( box.m_maxs.x, box.m_maxs.y, box.m_maxs.z );
	bLeft.position		= Vec3( box.m_mins.x, box.m_maxs.y, box.m_mins.z );
	bRight.position		= Vec3( box.m_maxs.x, box.m_maxs.y, box.m_mins.z );
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );

	// Bottom
	tLeft.normal		= Vec3( 0.0f, -1.0f, 0.0f );
	tRight.normal		= Vec3( 0.0f, -1.0f, 0.0f );
	bLeft.normal		= Vec3( 0.0f, -1.0f, 0.0f );
	bRight.normal		= Vec3( 0.0f, -1.0f, 0.0f );

	tLeft.tangent		= Vec4( -1.0f, 0.0f, 0.0f, 1.0f );
	tRight.tangent		= Vec4( -1.0f, 0.0f, 0.0f, 1.0f );
	bLeft.tangent		= Vec4( -1.0f, 0.0f, 0.0f, 1.0f );
	bRight.tangent		= Vec4( -1.0f, 0.0f, 0.0f, 1.0f );

	tLeft.position		= Vec3( box.m_mins.x, box.m_mins.y, box.m_mins.z );
	tRight.position		= Vec3( box.m_maxs.x, box.m_mins.y, box.m_mins.z );
	bLeft.position		= Vec3( box.m_mins.x, box.m_mins.y, box.m_maxs.z );
	bRight.position		= Vec3( box.m_maxs.x, box.m_mins.y, box.m_maxs.z );
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );
}

//--------------------------------------------------------------------------
/**
* CPUMeshAddFrustum
*/
void CPUMeshAddFrustum( MeshCPU* out, const Frustum& frustum )
{
	VertexMaster tLeft;
	VertexMaster tRight;
	VertexMaster bLeft;
	VertexMaster bRight;

	tLeft.uv			= Vec2( 0.0f, 0.0f );
	tRight.uv			= Vec2( 1.0f, 0.0f );
	bLeft.uv			= Vec2( 0.0f, 1.0f );
	bRight.uv			= Vec2( 1.0f, 1.0f );

	//    6-----7
	//   /|    /|
	//  2-----3 |
	//  | 4---|-5
	//  |/    |/
	//  0-----1

	Vec3 corners[8];
	frustum.GetCorners( corners );

	// Front
	tLeft.position		= corners[2];
	tRight.position		= corners[3];
	bLeft.position		= corners[0];
	bRight.position		= corners[1];
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );

	// Back
	tLeft.position		= corners[7];
	tRight.position		= corners[6];
	bLeft.position		= corners[5];
	bRight.position		= corners[4];
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );

	// Left
	tLeft.position		= corners[6];
	tRight.position		= corners[2];
	bLeft.position		= corners[4];
	bRight.position		= corners[0];
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );

	// Right
	tLeft.position		= corners[3];
	tRight.position		= corners[7];
	bLeft.position		= corners[1];
	bRight.position		= corners[5];
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );

	// Top
	tLeft.position		= corners[6];
	tRight.position		= corners[7];
	bLeft.position		= corners[2];
	bRight.position		= corners[3];
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );

	// Bottom
	tLeft.position		= corners[0];
	tRight.position		= corners[1];
	bLeft.position		= corners[4];
	bRight.position		= corners[5];
	AddQuadToMesh( out, tLeft, tRight, bLeft, bRight );
}

//--------------------------------------------------------------------------
/**
* CPUMeshAddLinedFrustum
*/
void CPUMeshAddLinedFrustum( MeshCPU* out, const Frustum& frustum, float thickness /*= 0.1f */ )
{
	Vec3 corners[8];
	frustum.GetCorners( corners );

	CPUMeshAddLine3D( out, corners[0], corners[1], thickness );
	CPUMeshAddLine3D( out, corners[0], corners[4], thickness );
	CPUMeshAddLine3D( out, corners[5], corners[1], thickness );
	CPUMeshAddLine3D( out, corners[5], corners[4], thickness );

	CPUMeshAddLine3D( out, corners[0], corners[2], thickness );
	CPUMeshAddLine3D( out, corners[4], corners[6], thickness );
	CPUMeshAddLine3D( out, corners[5], corners[7], thickness );
	CPUMeshAddLine3D( out, corners[1], corners[3], thickness );

	CPUMeshAddLine3D( out, corners[2], corners[3], thickness );
	CPUMeshAddLine3D( out, corners[3], corners[7], thickness );
	CPUMeshAddLine3D( out, corners[7], corners[6], thickness );
	CPUMeshAddLine3D( out, corners[6], corners[2], thickness );
}

//--------------------------------------------------------------------------
/**
* CPUMeshAddUVSphere
*/
void CPUMeshAddUVSphere( MeshCPU *out, Vec3 center, float radius, int wedges /*= 32*/, int slices /*= 16 */ )
{
	/*
	--Understanding Sphere Generation--

	<x, y, z> = S( r, theta, phi ) = S( r, pitch, yaw )
	
	for i, j, k
	r*cos(phi)( i*cos(thera) + k*sin(theta) ) + r*sin(phi)
	<r*cos(phi)cos(theta), r*sin(phi), r*cos(phi)sin(theta)>
	S( r, theta, phi ) = r<cos(phi)cos(theta), sin(phi), cos(phi)sin(theta)>
	
	u(0,1) => theta(0,2PI)
	v(0,1) => phi(PI/2,-PI/2)
	*/
	int numIniterts = (int)out->m_vertices.size();
	int ustep = wedges + 1;
	int vstep = slices + 1;

	for( int vidx = 0; vidx < vstep; ++vidx )
	{
		float v = (float) vidx / (float) slices;
		float phi = v * 180.0f + 90.0f;
		for( int uidx = 0; uidx < ustep; ++uidx )
		{
			float u = (float) uidx / (float) wedges;
			float theta = u * 360.0f;
			Vec3 pos = GetCartesianFromSpherical( radius, theta, phi ) + center;
			VertexMaster toAdd;
			out->SetUV( Vec2( (float) uidx / (float) wedges, (float) vidx / (float) slices ) );
			Vec3 normalizedDir = (pos - center).GetNormalized();
			out->SetNormal( normalizedDir );
			Vec4 tanget = Vec4( Cross( normalizedDir, Vec3( 0.0f, 1.0f, 0.0f ) ), 1.0f );
			out->SetTanget( tanget );
			out->AddVertex( pos );
		}
	}


	for( int y = 0; y < slices; ++y )
	{
		for( int x = 0; x < wedges; ++x )
		{
			int TLidx = y * ustep + x + numIniterts;
			int TRidx = TLidx + 1;
			int BLidx = TLidx + ustep;
			int BRidx = BLidx + 1;

			out->AddIndexedQuad( TLidx, TRidx, BLidx, BRidx );
		}
	}
}

//--------------------------------------------------------------------------
/**
* CPUMeshAddUVCapsule
*/
void CPUMeshAddCapsule( MeshCPU* out, const Vec3& start, const Vec3& end, float radius, int wedges /*= 32*/, int slices /*= 16 */ )
{
	CPUMeshAddCylinder( out, start, end, radius, radius, wedges * 2 );
	CPUMeshAddUVSphere( out, start, radius, wedges, slices );
	CPUMeshAddUVSphere( out, end, radius, wedges, slices );
}
