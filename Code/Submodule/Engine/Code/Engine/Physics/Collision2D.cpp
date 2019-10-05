#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Physics/AABB2Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PillboxCollider2D.hpp"



//--------------------------------------------------------------------------
/**
* CheckAABB2ByAABB2
*/
bool Collision2D::CheckAABB2ByAABB2( Collision2D* out, Collider2D* a, Collider2D* b )
{
	AABB2 boxA = static_cast<AABB2Collider2D*>(a)->GetWorldShape();
	AABB2 boxB = static_cast<AABB2Collider2D*>(b)->GetWorldShape();
	Pillbox2 pillA( boxA.GetCenter(), 0.0f, Vec2( boxA.GetWidth() * 0.5f, boxA.GetHeight() * 0.5f ), 0.0f );
	Pillbox2 pillB( boxB.GetCenter(), 0.0f, Vec2( boxB.GetWidth() * 0.5f, boxB.GetHeight() * 0.5f ), 0.0f );

	out->hit = GetManifold( &out->manifold, pillA, pillB );
	return out->hit;
}

//--------------------------------------------------------------------------
/**
* CheckAABB2ByDisc
*/
bool Collision2D::CheckAABB2ByDisc( Collision2D* out, Collider2D* a, Collider2D* b )
{
	bool sucess = CheckDiscByAABB2( out, b, a );
	out->manifold.normal *= -1.0f;
	return sucess;
}

//--------------------------------------------------------------------------
/**
* CheckDiscByDisc
*/
bool Collision2D::CheckDiscByDisc( Collision2D* out, Collider2D* a, Collider2D* b )
{
	Disc discA = static_cast<DiscCollider2D*>(a)->GetWorldShape();
	Disc discB = static_cast<DiscCollider2D*>(b)->GetWorldShape();

	Pillbox2 pillA( discA.m_center, discA.m_radius, Vec2::ZERO, 0.0f );
	Pillbox2 pillB( discB.m_center, discB.m_radius, Vec2::ZERO, 0.0f );
	out->hit = GetManifold( &out->manifold, pillA, pillB );
	return out->hit;
}

//--------------------------------------------------------------------------
/**
* CheckDiscByAABB2
*/
bool Collision2D::CheckDiscByAABB2( Collision2D* out, Collider2D* a, Collider2D* b )
{
	Disc disc = static_cast<DiscCollider2D*>(a)->GetWorldShape();
	Pillbox2 pillA( disc.m_center, disc.m_radius, Vec2::ZERO, 0.0f );
	AABB2 box = static_cast<AABB2Collider2D*>(b)->GetWorldShape();
	Pillbox2 pillB( box.GetCenter(), 0.0f, Vec2( box.GetWidth() * 0.5f, box.GetHeight() * 0.5f ), 0.0f );
	out->hit = GetManifold( &out->manifold, pillA, pillB );
	return out->hit;
}

//--------------------------------------------------------------------------
/**
* CheckPillboxByPillbox
*/
bool Collision2D::CheckPillboxByPillbox( Collision2D* out, Collider2D* a, Collider2D* b )
{
	out->hit = GetManifold( &out->manifold, static_cast<PillboxCollider2D*>(a)->GetWorldShape(), static_cast<PillboxCollider2D*>(b)->GetWorldShape() );
	return out->hit;
}

//--------------------------------------------------------------------------
/**
* CheckDiscByPillbox
*/
bool Collision2D::CheckDiscByPillbox( Collision2D* out, Collider2D* a, Collider2D* b )
{
	Disc disc = static_cast<DiscCollider2D*>(a)->GetWorldShape();
	Pillbox2 pill( disc.m_center, disc.m_radius, Vec2::ZERO, 0.0f );
	out->hit = GetManifold( &out->manifold, pill, static_cast<PillboxCollider2D*>(b)->GetWorldShape() );
	return out->hit;
}

//--------------------------------------------------------------------------
/**
* CheckAABB2ByPillbox
*/
bool Collision2D::CheckAABB2ByPillbox( Collision2D* out, Collider2D* a, Collider2D* b )
{
	AABB2 box = static_cast<AABB2Collider2D*>(a)->GetWorldShape();
	Pillbox2 pill( box.GetCenter(), 0.0f, Vec2( box.GetWidth() * 0.5f, box.GetHeight() * 0.5f ), 0.0f );
	out->hit = GetManifold( &out->manifold, pill, static_cast<PillboxCollider2D*>(b)->GetWorldShape() );
	return out->hit;
}

//--------------------------------------------------------------------------
/**
* CheckPillboxByAABB2
*/
bool Collision2D::CheckPillboxByAABB2( Collision2D* out, Collider2D* a, Collider2D* b )
{
	AABB2 box = static_cast<AABB2Collider2D*>(b)->GetWorldShape();
	Pillbox2 pill( box.GetCenter(), 0.0f, Vec2( box.GetWidth() * 0.5f, box.GetHeight() * 0.5f ), 0.0f );
	out->hit = GetManifold( &out->manifold, static_cast<PillboxCollider2D*>(a)->GetWorldShape(), pill );
	return out->hit;
}

//--------------------------------------------------------------------------
/**
* CheckPillboxByDisc
*/
bool Collision2D::CheckPillboxByDisc( Collision2D* out, Collider2D* a, Collider2D* b )
{
	Disc disc = static_cast<DiscCollider2D*>(b)->GetWorldShape();
	Pillbox2 pill( disc.m_center, disc.m_radius, Vec2::ZERO, 0.0f );
	out->hit = GetManifold( &out->manifold, static_cast<PillboxCollider2D*>(a)->GetWorldShape(), pill );
	return out->hit;
}
