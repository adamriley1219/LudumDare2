#pragma once
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Manifold2D.hpp"

struct Collision2D;

typedef bool (*Collision2DCheckCallback)( Collision2D* out, Collider2D* a, Collider2D* b );

struct Collision2D
{
public:

	// for now, nothing, 
	// but next week will contain information 
	// for correction; 
	Collider2D* fromCheckCollider;
	Collider2D* toCheckCollider;
	Manifold2D manifold;
	bool hit = false;



	static bool CheckAABB2ByAABB2( Collision2D* out, Collider2D* a, Collider2D* b );
	static bool CheckAABB2ByDisc( Collision2D* out, Collider2D* a, Collider2D* b );
	static bool CheckDiscByDisc( Collision2D* out, Collider2D* a, Collider2D* b );
	static bool CheckDiscByAABB2( Collision2D* out, Collider2D* a, Collider2D* b );
	static bool CheckPillboxByPillbox( Collision2D* out, Collider2D* a, Collider2D* b );
	static bool CheckDiscByPillbox( Collision2D* out, Collider2D* a, Collider2D* b );
	static bool CheckAABB2ByPillbox( Collision2D* out, Collider2D* a, Collider2D* b );
	static bool CheckPillboxByAABB2( Collision2D* out, Collider2D* a, Collider2D* b );
	static bool CheckPillboxByDisc( Collision2D* out, Collider2D* a, Collider2D* b );

	//------------------------------------------------------------------------
	// Doing this by lookup - as it is a good intro to callbacks
	// but could also do this by double dispatch:  a->collides_with( b )
	static bool GetCollisionInfo( Collision2D* out, Collider2D* a, Collider2D* b )
	{
		// 2D arrays are [Y][X] remember
		static Collision2DCheckCallback COLLISION_LOOKUP_TABLE[NUM_COLLIDER_TYPES][NUM_COLLIDER_TYPES] = {
			/*******| aabb2 | disc  | Pillbox   */
			/*aabb2*/	CheckAABB2ByAABB2, CheckAABB2ByDisc, CheckAABB2ByPillbox,
			/*disc */	CheckDiscByAABB2,  CheckDiscByDisc, CheckDiscByPillbox,
			/*Pillbox*/ CheckPillboxByAABB2, CheckPillboxByDisc, CheckPillboxByPillbox
		}; 

		eColliderType2D a_type = a->GetType(); 
		eColliderType2D b_type = b->GetType(); 
		ASSERT_OR_DIE( (a_type < NUM_COLLIDER_TYPES) && (b_type < NUM_COLLIDER_TYPES), "GetCollisionInfo recieved bad data." ); 

		Collision2DCheckCallback cb = COLLISION_LOOKUP_TABLE[a_type][b_type]; 
		if (cb == nullptr) {
			return false; // no known collision; 
		} else {
			out->fromCheckCollider = a; 
			out->toCheckCollider = b; 
			return cb( out, a, b ); 
		}
	}
};