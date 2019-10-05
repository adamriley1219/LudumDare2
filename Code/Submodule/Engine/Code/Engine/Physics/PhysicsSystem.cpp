#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/PillboxCollider2D.hpp"
#include "Engine/Physics/AABB2Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"

//--------------------------------------------------------------------------
/**
* PhysicsSystem
*/
PhysicsSystem::PhysicsSystem()
{

}

//--------------------------------------------------------------------------
/**
* ~PhysicsSystem
*/
PhysicsSystem::~PhysicsSystem()
{

}

//--------------------------------------------------------------------------
/**
* BeginFrame
*/
void PhysicsSystem::BeginFrame()
{
	for( Collider2D* colllider : m_colliders )
	{
		if( colllider )
		{
			colllider->m_isColliding = false;
		}
	}
}

//--------------------------------------------------------------------------
/**
* Update
*/
void PhysicsSystem::Update( float deltaTime )
{
	if( deltaTime == 0.0f )
	{
		return;
	}
	if( deltaTime >= 0.2f )
	{
		deltaTime = 0.2f;
	}
	PushTransformsToEngine();
	RunStep( deltaTime );
	PushTransformsToGame();
}

//--------------------------------------------------------------------------
/**
* PushTransformsToGame
*/
void PhysicsSystem::PushTransformsToEngine()
{
	for( int typeIndex = 0; typeIndex < NUM_PHYSICS_SIM_TYPES; typeIndex++ )
	{
		for( Rigidbody2D* rb : m_rigidbodies[typeIndex] )
		{
			if( rb )
			{
				rb->m_transform = *rb->m_objectTransform;
			}
		}
	}
}

//--------------------------------------------------------------------------
/**
* PushTransformsToEngine
*/
void PhysicsSystem::PushTransformsToGame()
{
	for( int typeIndex = 0; typeIndex < NUM_PHYSICS_SIM_TYPES; typeIndex++ )
	{
		for( Rigidbody2D* rb : m_rigidbodies[typeIndex] )
		{
			if( rb )
			{
				*rb->m_objectTransform = rb->m_transform;
			}
		}
	}
}

//--------------------------------------------------------------------------
/**
* EndFrame
*/
void PhysicsSystem::EndFrame()
{
}

//--------------------------------------------------------------------------
/**
* Startup
*/
void PhysicsSystem::Startup()
{

}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void PhysicsSystem::Shutdown()
{
	for( Collider2D* colllider : m_colliders )
	{
		if( colllider )
		{
			delete colllider;
			colllider = nullptr;
		}
	}

	for( int typeIndex = 0; typeIndex < NUM_PHYSICS_SIM_TYPES; typeIndex++ )
	{
		for( Rigidbody2D* rb : m_rigidbodies[typeIndex] )
		{
			if( rb )
			{
				delete rb;
				rb = nullptr;
			}
		}
	}

	
}

//--------------------------------------------------------------------------
/**
* CreateRigidbody
*/
Rigidbody2D* PhysicsSystem::CreateRigidbody( float mass )
{
	Rigidbody2D* rigidbody = new Rigidbody2D( this );
	rigidbody->m_mass = mass;
	AddRigidbody( rigidbody );
	return rigidbody;
}

//--------------------------------------------------------------------------
/**
* CreateCollider
*/
Collider2D* PhysicsSystem::CreateCollider( bool isTrigger /*= false*/, const Vec2& center /*= Vec2::ZERO*/, float radius /*= 0.0f*/, const Vec2& exstents /*= Vec2::ZERO*/, float rotation /*= 0.0f */ )
{
	Collider2D* collider = new PillboxCollider2D( isTrigger, Pillbox2( center, radius, exstents, rotation ) );
	AddCollider( collider );
	return collider;
}

//--------------------------------------------------------------------------
/**
* CreateColliderAABB2
*/
Collider2D* PhysicsSystem::CreateColliderAABB2( bool isTrigger /*= false*/, float width /*= 1.0f*/, float height /*= 1.0f*/, Vec2 center /*= Vec2::ZERO */ )
{
	Collider2D* collider = new AABB2Collider2D( isTrigger,  AABB2( width, height, center ) );
	AddCollider( collider );
	return collider;
}

//--------------------------------------------------------------------------
/**
* CreateColliderDisc
*/
Collider2D* PhysicsSystem::CreateColliderDisc( bool isTrigger /*= false*/, const Vec2& center /*= Vec2::ZERO*/, float radius /*= 1.0f */ )
{
	Collider2D* collider = new DiscCollider2D( isTrigger, center, radius );
	AddCollider( collider );
	return collider;
}

//--------------------------------------------------------------------------
/**
* RemoveRigidbody
*/
void PhysicsSystem::RemoveRigidbody( Rigidbody2D* rigidbody )
{
	if( rigidbody->m_collider )
	{
		RemoveCollider( rigidbody->m_collider );
	}


	for( Rigidbody2D*& rb : m_rigidbodies[rigidbody->m_simType] )
	{
		if( rb && rb == rigidbody )
		{
			rb->m_isGarbage = true;
			return;
		}
	}
}

//--------------------------------------------------------------------------
/**
* RemoveRigidbodyWoutDeleting
* WARNING: DO NOT CALL WHILE LOOPING IN PHYSICS STEP
*/
void PhysicsSystem::RemoveRigidbodyWoutDeleting( Rigidbody2D* rigidbody )
{
	if( rigidbody->m_collider )
	{
		for( Collider2D*& collider : m_colliders )
		{
			if( collider && collider == rigidbody->m_collider )
			{
				collider = nullptr;
				break;
			}
		}
	}

	for( Rigidbody2D*& rb : m_rigidbodies[rigidbody->m_simType] )
	{
		if( rb && rb == rigidbody )
		{
			rb = nullptr;
			return;
		}
	}
}

//--------------------------------------------------------------------------
/**
* SetRBSimType
*/
void PhysicsSystem::SetRBSimType( Rigidbody2D* ridgidbody, ePhysicsSimulationType type )
{
	RemoveRigidbodyWoutDeleting( ridgidbody );
	ridgidbody->m_simType = type;
	AddRigidbody( ridgidbody );
	if( ridgidbody->m_collider )
	{
		AddCollider( ridgidbody->m_collider );
		ridgidbody->UpdateMomentOfInertia();
	}
}

//--------------------------------------------------------------------------
/**
* DebugRender
*/
#ifdef PLATFORM_WINDOWS
void PhysicsSystem::DebugRender( RenderContext* renderer, const Rgba& color ) const
{
	for ( int typeIndex = 0; typeIndex < NUM_PHYSICS_SIM_TYPES; typeIndex++ )
	{
		for ( Rigidbody2D* rb : m_rigidbodies[typeIndex] )
		{
			if ( rb )
			{
				rb->DebugRender( renderer, color );
			}
		}
	}
}
#endif

//--------------------------------------------------------------------------
/**
* RunStep
*/
void PhysicsSystem::RunStep( float deltaTime )
{
	Cleanup();


	for( int typeIndex = 0; typeIndex < NUM_PHYSICS_SIM_TYPES; typeIndex++ )
	{
		for( Rigidbody2D* rb : m_rigidbodies[typeIndex] )
		{
			if( rb )
			{
				rb->Move( deltaTime );
			}
		}
	}
	
	ResolveALLDynamicVSStatic(true);
	ResolveALLDynamicVSDynamic(true);
	ResolveALLDynamicVSStatic(false);

	ResolveTirggers();

	Cleanup();
}

//--------------------------------------------------------------------------
/**
* AddCollider
*/
void PhysicsSystem::AddCollider( Collider2D* collider )
{
	for( int colIndex = 0; colIndex < (int) m_colliders.size(); colIndex++ )
	{
		if( !m_colliders[colIndex] )
		{
			m_colliders[colIndex] = collider;
			return;
		}
	}
	m_colliders.push_back( collider );
}

//--------------------------------------------------------------------------
/**
* RemoveCollider
*/
void PhysicsSystem::RemoveCollider( Collider2D* collider )
{
	for( Collider2D*& c : m_colliders )
	{
		if( c && c == collider )
		{
			c->m_isGarbage = true;
			return;
		}
	}
}



//--------------------------------------------------------------------------
/**
* AddRigidbody
*/
void PhysicsSystem::AddRigidbody( Rigidbody2D* rigidbody )
{
	ePhysicsSimulationType type = rigidbody->m_simType;
	for( int rbIndex = 0; rbIndex < (int) m_rigidbodies[type].size(); ++rbIndex )
	{
		if( m_rigidbodies[type][rbIndex] == nullptr )
		{
			m_rigidbodies[type][rbIndex] = rigidbody;
			return;
		}
	}
	 m_rigidbodies[type].push_back( rigidbody );
}

//--------------------------------------------------------------------------
/**
* ResolveALLDynamicVSStatic
*/
void PhysicsSystem::ResolveALLDynamicVSStatic( bool handleResponse )
{
	for( int dynIndex = 0; dynIndex < (int) m_rigidbodies[PHYSICS_SIM_DYNAMIC].size(); ++dynIndex )
	{
		for( int statIndex = 0; statIndex < (int) m_rigidbodies[PHYSICS_SIM_STATIC].size(); ++statIndex )
		{
			Collision2D collisionInfo;
			Rigidbody2D* a = m_rigidbodies[PHYSICS_SIM_DYNAMIC][dynIndex];
			Rigidbody2D* b = m_rigidbodies[PHYSICS_SIM_STATIC][statIndex];
			if( a && b && a != b && ( !a->m_collider->m_isTrigger && !b->m_collider->m_isTrigger ) && a->m_collider->IsTouching( b->m_collider, &collisionInfo ) )
			{
				MoveDynamicVSStatic( a, b, &collisionInfo );
			
				if( handleResponse )
				{
					RespondDynamicVSStatic( a, b, &collisionInfo );
					a->m_collider->FireHitEvent( b->m_collider );
					b->m_collider->FireHitEvent( a->m_collider );
				}
			}
		}
	}
}


//--------------------------------------------------------------------------
/**
* MoveDynamicVSStatic
*/
void PhysicsSystem::MoveDynamicVSStatic( Rigidbody2D* dynamicRB, Rigidbody2D* staticRB, Collision2D* collisionInfo )
{
	if( collisionInfo->hit )
	{
		dynamicRB->m_transform.m_position += collisionInfo->manifold.normal * collisionInfo->manifold.penetration;
		collisionInfo->manifold.contactPoint += collisionInfo->manifold.normal * collisionInfo->manifold.penetration;
		dynamicRB->m_collider->m_isColliding = true;
		staticRB->m_collider->m_isColliding = true;
	}
}

//--------------------------------------------------------------------------
/**
* RespondDynamicVSStatic
*/
void PhysicsSystem::RespondDynamicVSStatic( Rigidbody2D* dynamicRB, Rigidbody2D* staticRB, const Collision2D* collisionInfo )
{
	float restitutionFac = dynamicRB->m_phyMat.m_restitution * staticRB->m_phyMat.m_restitution;

	Vec2 normal = collisionInfo->manifold.normal;
	Vec2 tangent = normal.GetRotated90Degrees();
	float m1 = dynamicRB->GetMass();

	// Follow http://chrishecker.com/images/e/e7/Gdmphys3.pdf - pg: 16
	//--------------------------------------------------------------------------
	Vec2 velAB = dynamicRB->GetImpactVelocity( collisionInfo->manifold.contactPoint );
	float e = restitutionFac;

	// rAP
	Vec2 dispAtoP = collisionInfo->manifold.contactPoint - dynamicRB->GetTransform().m_position;
	Vec2 perpDispAtoP = dispAtoP.GetRotated90Degrees();

	float inertiaA = dynamicRB->m_momentOfI;

	float perpDispAtoPDotNormal = DotProduct( perpDispAtoP, normal );
	float impulseAlongNormal = 
		( -1.0f * ( 1 + e ) * DotProduct( velAB, normal ) )
		/ ( ( 1/m1 ) + (perpDispAtoPDotNormal * perpDispAtoPDotNormal) / inertiaA );


	float perpDispAtoPDotTangent = DotProduct( perpDispAtoP, tangent );
	float impulseAlongTangent = 
		( -1.0f * ( 1 + e ) * DotProduct( velAB, tangent ) )
		/ ( ( 1/m1 ) + (perpDispAtoPDotTangent * perpDispAtoPDotTangent) / inertiaA );

	float frictionFac = (float) sqrt( abs( dynamicRB->m_phyMat.m_friction * staticRB->m_phyMat.m_friction ) );
	float coulumbsLaw = Clamp( impulseAlongTangent, -impulseAlongNormal, impulseAlongNormal ) * frictionFac;
	dynamicRB->ApplyImpulseAt( impulseAlongNormal * normal + coulumbsLaw * tangent, collisionInfo->manifold.contactPoint );
}

//--------------------------------------------------------------------------
/**
* ResolveALLDynamicVSDynamic
*/
void PhysicsSystem::ResolveALLDynamicVSDynamic( bool handleResponse )
{
	for( int dynIndex = 0; dynIndex < (int) m_rigidbodies[PHYSICS_SIM_DYNAMIC].size(); ++dynIndex )
	{
		for( int otherDynIndex = dynIndex + 1; otherDynIndex < (int) m_rigidbodies[PHYSICS_SIM_DYNAMIC].size(); ++otherDynIndex )
		{
			Collision2D collisionInfo;
			Rigidbody2D* a = m_rigidbodies[PHYSICS_SIM_DYNAMIC][dynIndex];
			Rigidbody2D* b = m_rigidbodies[PHYSICS_SIM_DYNAMIC][otherDynIndex];
			if( a && b && a != b && ( !a->m_collider->m_isTrigger && !b->m_collider->m_isTrigger ) && a->m_collider->IsTouching( b->m_collider, &collisionInfo ) )
			{
				MoveDynamicVSDynamic( a, b, &collisionInfo );
				
				if( handleResponse )
				{
					RespondDynamicVSDynamic( a, b, &collisionInfo );
					a->m_collider->FireHitEvent( b->m_collider );
					b->m_collider->FireHitEvent( a->m_collider );
				}
			}
		}
	}
}


//--------------------------------------------------------------------------
/**
* MoveDynamicVSDynamic
*/
void PhysicsSystem::MoveDynamicVSDynamic( Rigidbody2D* dynamicRBa, Rigidbody2D* dynamicRBb, Collision2D* collisionInfo )
{
	if( collisionInfo->hit )
	{
		float totalMass = dynamicRBa->GetMass() + dynamicRBb->GetMass();
		float amountA = dynamicRBa->GetMass() / totalMass;
		float amountB = dynamicRBb->GetMass() / totalMass;

		collisionInfo->manifold.contactPoint += collisionInfo->manifold.normal * ( collisionInfo->manifold.penetration * amountA );

		Vec2 moveAmoutA = collisionInfo->manifold.normal * ( collisionInfo->manifold.penetration * amountA );
		Vec2 moveAmoutB = collisionInfo->manifold.normal * ( collisionInfo->manifold.penetration * amountB ) * -1.0f;

		dynamicRBa->m_transform.m_position += Vec2( dynamicRBa->m_xRestructed ? 0.0f : moveAmoutA.x, dynamicRBa->m_yRestructed ? 0.0f : moveAmoutA.y );
		dynamicRBb->m_transform.m_position += Vec2( dynamicRBb->m_xRestructed ? 0.0f : moveAmoutB.x, dynamicRBb->m_yRestructed ? 0.0f : moveAmoutB.y );
		dynamicRBa->m_collider->m_isColliding = true;
		dynamicRBb->m_collider->m_isColliding = true;
	}
}

//--------------------------------------------------------------------------
/**
* RespondDynamicVSDynamic
*/
void PhysicsSystem::RespondDynamicVSDynamic( Rigidbody2D* dynamicRBa, Rigidbody2D* dynamicRBb, const Collision2D* collisionInfo )
{
	Vec2 vela = dynamicRBa->GetImpactVelocity( collisionInfo->manifold.contactPoint );
	Vec2 velb = dynamicRBb->GetImpactVelocity( collisionInfo->manifold.contactPoint );
	Vec2 normal = collisionInfo->manifold.normal;
	Vec2 tangent = normal.GetRotated90Degrees();

	float restitutionFac = dynamicRBb->m_phyMat.m_restitution * dynamicRBa->m_phyMat.m_restitution;

	float m1 = dynamicRBa->GetMass();
	float m2 = dynamicRBb->GetMass();

	// Follow http://chrishecker.com/images/e/e7/Gdmphys3.pdf - pg: 16
	//--------------------------------------------------------------------------
	Vec2 velAB = vela - velb;
	float e = restitutionFac;

	// rAP
	Vec2 dispAtoP = collisionInfo->manifold.contactPoint - dynamicRBa->GetTransform().m_position;
	Vec2 dispBtoP = collisionInfo->manifold.contactPoint - dynamicRBb->GetTransform().m_position;
	Vec2 perpDispAtoP = dispAtoP.GetRotated90Degrees();
	Vec2 perpDispBtoP = dispBtoP.GetRotated90Degrees();

	float inertiaA = dynamicRBa->m_momentOfI;
	float inertiaB = dynamicRBb->m_momentOfI;

	float perpDispAtoPDotNormal = DotProduct( perpDispAtoP, normal );
	float perpDispBtoPDotNormal = DotProduct( perpDispBtoP, normal );
	float impulseAlongNormal = 
		( -1.0f * ( 1 + e ) * DotProduct( velAB, normal ) )
		/ ( ( 1/m1 + 1/m2 ) + (perpDispAtoPDotNormal * perpDispAtoPDotNormal) / inertiaA + (perpDispBtoPDotNormal * perpDispBtoPDotNormal) / inertiaB );

	float perpDispAtoPDotTangent = DotProduct( perpDispAtoP, tangent );
	float perpDispBtoPDotTangent = DotProduct( perpDispBtoP, tangent );

	float impulseAlongTangent = 
		( -1.0f * ( 1 + e ) * DotProduct( velAB, tangent ) )
		/ ( ( 1/m1 + 1/m2 ) + (perpDispAtoPDotTangent * perpDispAtoPDotTangent) / inertiaA + (perpDispBtoPDotTangent * perpDispBtoPDotTangent) / inertiaB );

	float frictionFac = (float) sqrt( abs( dynamicRBb->m_phyMat.m_friction * dynamicRBa->m_phyMat.m_friction ) );
	float coulumbsLaw = Clamp( impulseAlongTangent, -impulseAlongNormal, impulseAlongNormal ) * frictionFac;

	dynamicRBa->ApplyImpulseAt( impulseAlongNormal * normal			 + coulumbsLaw * tangent, collisionInfo->manifold.contactPoint );
	dynamicRBb->ApplyImpulseAt( impulseAlongNormal * normal * -1.0f	 - coulumbsLaw * tangent, collisionInfo->manifold.contactPoint );
}

//--------------------------------------------------------------------------
/**
* ResolveTirggers
*/
void PhysicsSystem::ResolveTirggers()
{
	for( Collider2D* c : m_colliders )
	{
		if( c && c->m_isTrigger )
		{
			for( Collider2D* other : m_colliders )
			{
				if( other && c != other )
				{
					if( c->IsTouching( other ) )
					{
						if( c->EnterCollider( other ) )
						{
							c->FireEnterEvent( other );
						}
						else
						{
							c->FireStayEvent( other );
						}
					}
					else
					{
						if( c->ExitCollider( other ) )
						{
							c->FireExitEvent( other );
						}
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------
/**
* Cleanup
*/
void PhysicsSystem::Cleanup()
{
	for( Collider2D*& colllider : m_colliders )
	{
		if( colllider && colllider->m_isGarbage )
		{
			if( colllider->m_rigidbody )
			{
				colllider->m_rigidbody->m_collider = nullptr;
			}
			colllider->ExitAllColliders();
			delete colllider;
			colllider = nullptr;
		}
	}

	for( int typeIndex = 0; typeIndex < NUM_PHYSICS_SIM_TYPES; typeIndex++ )
	{
		for( Rigidbody2D*& rb : m_rigidbodies[typeIndex] )
		{
			if( rb && rb->m_isGarbage )
			{
				if( rb->m_collider )
				{
					delete rb->m_collider;
					rb->m_collider = nullptr;
				}
				delete rb;
				rb = nullptr;
			}
		}
	}
}
