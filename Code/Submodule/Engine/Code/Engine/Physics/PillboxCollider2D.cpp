#include "Engine/Physics/PillboxCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"




//--------------------------------------------------------------------------
/**
* PillboxCollider2D
*/
PillboxCollider2D::PillboxCollider2D( bool isTrigger, const Pillbox2& pill )
	: Collider2D( isTrigger ), m_localShape( pill )
{
	m_type = COLLIDER_PILLBOX;
}

//--------------------------------------------------------------------------
/**
* GetLocalShape
*/
Pillbox2 PillboxCollider2D::GetLocalShape() const
{
	return m_localShape;
}

//--------------------------------------------------------------------------
/**
* GetWorldShape
*/
Pillbox2 PillboxCollider2D::GetWorldShape() const
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
void PillboxCollider2D::DebugRender( RenderContext* renderer, const Rgba& color ) const
{
	std::vector<Vertex_PCU> verts;
	const Pillbox2 pill = GetWorldShape();

	Vec2 BL = pill.m_obb.m_center - pill.m_obb.m_extents.x * pill.m_obb.GetRight() - pill.m_obb.GetUp() * pill.m_obb.m_extents.y;
	Vec2 TR = pill.m_obb.m_center + pill.m_obb.m_extents.x * pill.m_obb.GetRight() + pill.m_obb.GetUp() * pill.m_obb.m_extents.y;
	Vec2 TL = pill.m_obb.m_center + pill.m_obb.GetUp() * pill.m_obb.m_extents.y - pill.m_obb.GetRight() * pill.m_obb.m_extents.x;
	Vec2 BR = pill.m_obb.m_center - pill.m_obb.GetUp() * pill.m_obb.m_extents.y + pill.m_obb.GetRight() * pill.m_obb.m_extents.x;

	// Disc
	if( BL == TL && BL == BR )
	{
		AddVertsForRing2D( verts, BL, pill.m_radius, 0.05f, color, 24 );
		renderer->DrawVertexArray( verts );
		return;
	}

	// Check if there is length along the height but none on width
	if( BL != TL && BL == BR )
	{
		Vec2 alongHeight = TR - BR;
		alongHeight.SetLength( pill.m_radius );
		alongHeight.Rotate90Degrees();
		AddVertsForLine2D( verts, BR - alongHeight, TR - alongHeight, 0.05f, color );
		AddVertsForLine2D( verts, TL + alongHeight, BL + alongHeight, 0.05f, color );
		AddVertsForRing2D( verts, BL, pill.m_radius, 0.05f, color, 24 );
		AddVertsForRing2D( verts, TL, pill.m_radius, 0.05f, color, 24 );
		renderer->DrawVertexArray( verts );
		return;
	}

	// Check to see if there is length along the width but not height
	if( BL != BR && BL == TL )
	{
		Vec2 alongWidth = BR - BL;
		alongWidth.SetLength( pill.m_radius );
		alongWidth.RotateMinus90Degrees();
		AddVertsForLine2D( verts, TR - alongWidth, TL - alongWidth, 0.05f, color );
		AddVertsForLine2D( verts, BL + alongWidth, BR + alongWidth, 0.05f, color );
		AddVertsForRing2D( verts, BL, pill.m_radius, 0.05f, color, 24 );
		AddVertsForRing2D( verts, BR, pill.m_radius, 0.05f, color, 24 );
		renderer->DrawVertexArray( verts );
		return;
	}
	

	Vec2 alongHeight = TR - BR;
	alongHeight.SetLength( pill.m_radius );
	alongHeight.Rotate90Degrees();
	AddVertsForLine2D( verts, BR - alongHeight, TR - alongHeight, 0.05f, color );
	AddVertsForLine2D( verts, TL + alongHeight, BL + alongHeight, 0.05f, color );

	Vec2 alongWidth = BR - BL;
	alongWidth.SetLength( pill.m_radius );
	alongWidth.RotateMinus90Degrees();
	AddVertsForLine2D( verts, TR - alongWidth, TL - alongWidth, 0.05f, color );
	AddVertsForLine2D( verts, BL + alongWidth, BR + alongWidth, 0.05f, color );

	AddVertsForRing2D( verts, BL, pill.m_radius, 0.05f, color, 24 );
	AddVertsForRing2D( verts, BR, pill.m_radius, 0.05f, color, 24 );
	AddVertsForRing2D( verts, TR, pill.m_radius, 0.05f, color, 24 );
	AddVertsForRing2D( verts, TL, pill.m_radius, 0.05f, color, 24 );


	renderer->DrawVertexArray( verts );
}
#endif

//--------------------------------------------------------------------------
/**
* GetMomentOfInertia
*/
float PillboxCollider2D::GetMomentOfInertia()
{
	if( !m_rigidbody )
	{
		return 1.0f;
	}
	// https://en.wikipedia.org/wiki/List_of_moments_of_inertia
	float mass = m_rigidbody->GetMass();
	float area = m_localShape.GetArea();
	float radius = m_localShape.m_radius;
	float halfRadius = radius * 0.5f;
	float twelfth = 1.0f / 12.0f;
	OBB2& box = m_localShape.m_obb;

	float localInertia = 0.0f;

	OBB2 boxes[] =
	{
		OBB2( box.GetCenter(), Vec2( box.m_extents.x, box.m_extents.y + halfRadius ), 0.0f ), // Middle Chunk
		OBB2( box.GetCenter() +	Vec2( -halfRadius - box.m_extents.x, 0.0f )	, Vec2( halfRadius, box.m_extents.y ), 0.0f ), // Left
		OBB2( box.GetCenter() +	Vec2( halfRadius + box.m_extents.x, 0.0f )	, Vec2( halfRadius, box.m_extents.y ), 0.0f ), // Right
	};
	
	float usedMass = 0.0f;
	for( int boxIdx = 0; boxIdx < 3; ++boxIdx )
	{
		OBB2& curBox = boxes[boxIdx];
		float boxArea = curBox.GetArea();
		float boxMass = mass * ( boxArea / area );
		float doubleWidth = curBox.m_extents.x * curBox.m_extents.x * curBox.m_extents.x * curBox.m_extents.x ;
		float doubleHeight = curBox.m_extents.y * curBox.m_extents.y * curBox.m_extents.y * curBox.m_extents.y ;
		localInertia += twelfth * boxMass * ( doubleHeight + doubleWidth ) + ( box.GetCenter() - curBox.GetCenter() ).GetLengthSquared() * boxMass;
		usedMass += boxMass;
	}

	float discMass = mass - usedMass;
	Vec2 discOffset = box.GetBotLeft();
	localInertia += 0.5f * discMass * radius * radius + ( box.GetCenter() - discOffset ).GetLengthSquared() * discMass;  

	Vec2 offset = m_localShape.m_obb.GetCenter();
	return localInertia + mass * offset.GetLengthSquared();
}

//--------------------------------------------------------------------------
/**
* GetAsXMLElemnt
*/
tinyxml2::XMLElement* PillboxCollider2D::GetAsXMLElemnt( tinyxml2::XMLDocument* doc ) const
{
	tinyxml2::XMLElement* shapeColEle = doc->NewElement( "collider" );
	
	shapeColEle->SetAttribute( "radius", Stringf( "%f", m_localShape.m_radius ).c_str() );
	shapeColEle->SetAttribute( "extents", Stringf( "%f,%f",		m_localShape.m_obb.m_extents.x,	m_localShape.m_obb.m_extents.y ).c_str() );
	shapeColEle->SetAttribute( "locCenter", Stringf( "%f,%f",	m_localShape.m_obb.m_center.x,	m_localShape.m_obb.m_center.y ).c_str() );
	shapeColEle->SetAttribute( "locRight", Stringf( "%f,%f",	m_localShape.m_obb.m_right.x,	m_localShape.m_obb.m_right.y ).c_str() );

	return shapeColEle;
}
