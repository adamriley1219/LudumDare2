#include "Game/GameUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RNG.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Entity.hpp"
#include "Game/App.hpp"


//--------------------------------------------------------------------------
/**
* DrawDisc
*/
void DrawDisc( const Vertex_PCU translation, float radius )
{
	constexpr int NUM_DISC_SIDES = 64;
	constexpr int NUM_DISC_VERTS = 64 * 3;

	float toAddDegrees = 360.0f / NUM_DISC_SIDES;
	float theta = 0.0f;
	Vertex_PCU VertsToDraw[ NUM_DISC_VERTS ] = {};

	for( int i = 0; i < NUM_DISC_VERTS; i += 3 )
	{
		VertsToDraw[i] = translation;

		Vec2 point2d = Vec2::MakeFromPolarDegrees(theta, radius);
		Vec3 point3d = Vec3( point2d.x, point2d.y, 0.0f );
		VertsToDraw[i + 1] = Vertex_PCU( translation.position + point3d , translation.color, translation.uv );

		theta += toAddDegrees;
		point2d = Vec2::MakeFromPolarDegrees(theta, radius);
		point3d = Vec3( point2d.x, point2d.y, 0.0f );
		VertsToDraw[i + 2] = Vertex_PCU( translation.position + point3d , translation.color, translation.uv );
	}

	g_theRenderer->DrawVertexArray( NUM_DISC_VERTS, VertsToDraw );
}

//--------------------------------------------------------------------------
/**
* ResolveBulletVxAsteroidGetDistance
*/
float GetDistanceBetween( const Entity* entityA, const Entity* entiryB )
{
	return GetDistance( entityA->GetPosition(), entiryB->GetPosition() );
}

float GetRandomlyChosenFloat( float a, float b )
{
	int randomf = g_theRNG->GetRandomIntInRange(0, 1);
	if( randomf == 0 )
	{
		return a;
	}
	return b;
}
