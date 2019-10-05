#include "Engine/Physics/AABB2Collider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"

#ifdef PLATFORM_WINDOWS
#include "Engine/Renderer/RenderContext.hpp"
#endif


//--------------------------------------------------------------------------
/**
* AABB2Collider2D
*/
AABB2Collider2D::AABB2Collider2D( bool isTrigger, const AABB2& box )
	: Collider2D( isTrigger )
{
	m_localShape = box;
	m_type = eColliderType2D::COLLIDER_AABB2;
}


//--------------------------------------------------------------------------
/**
* GetLocalShape
*/
AABB2 AABB2Collider2D::GetLocalShape() const
{
	return m_localShape;
}

//--------------------------------------------------------------------------
/**
* GetWorldShape
*/
AABB2 AABB2Collider2D::GetWorldShape() const
{
	if( !m_rigidbody )
	{
		return m_localShape;
	}
	return m_localShape.GetTransformed( m_rigidbody->GetTransform() );
}

//--------------------------------------------------------------------------
/**
* DebugRender
*/
#ifdef PLATFORM_WINDOWS
void AABB2Collider2D::DebugRender( RenderContext* renderer, const Rgba& color ) const
{
	std::vector<Vertex_PCU> verts;
	const AABB2 worldBox = GetWorldShape();
	AddVertsForLine2D( verts, worldBox.m_bottomLeft,  worldBox.GetTopLeft(), 0.05f, color );
	AddVertsForLine2D( verts, worldBox.GetTopLeft(),  worldBox.m_topRight, 0.05f, color );
	AddVertsForLine2D( verts, worldBox.m_topRight,  worldBox.GetBottomRight(), 0.05f, color );
	AddVertsForLine2D( verts, worldBox.GetBottomRight(),  worldBox.m_bottomLeft, 0.05f, color );

	renderer->DrawVertexArray( verts );
}
#endif

//--------------------------------------------------------------------------
/**
* GetMomentOfInertia
*/
float AABB2Collider2D::GetMomentOfInertia()
{
	if( !m_rigidbody )
	{
		return 1.0f;
	}
	float twelfth = 1.0f / 12.0f;
	float mass = m_rigidbody->GetMass();
	AABB2& curBox = m_localShape;
	float doubleWidth = curBox.GetWidth() * curBox.GetWidth() ;
	float doubleHeight = curBox.GetHeight() * curBox.GetHeight();
	return twelfth * mass * ( doubleHeight + doubleWidth ) + ( curBox.GetCenter() - curBox.GetCenter() ).GetLengthSquared() * mass;

}

//--------------------------------------------------------------------------
/**
* GetAsXMLElemnt
*/
tinyxml2::XMLElement* AABB2Collider2D::GetAsXMLElemnt( tinyxml2::XMLDocument* doc ) const
{
	tinyxml2::XMLElement* shapeColEle = doc->NewElement( "collider" );

	shapeColEle->SetAttribute( "radius", Stringf( "%f", 0.0f ).c_str() );
	shapeColEle->SetAttribute( "extents", Stringf( "%f,%f",		m_localShape.GetWidth() * 0.5f,	m_localShape.GetHeight() * 0.5f ).c_str() );
	shapeColEle->SetAttribute( "locCenter", Stringf( "%f,%f",	m_localShape.GetCenter().x,	m_localShape.GetCenter().y ).c_str() );
	shapeColEle->SetAttribute( "locRight", Stringf( "%f,%f", 1.0f, 0.0f ).c_str() );

	return shapeColEle;
}
