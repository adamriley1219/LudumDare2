#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif

class Rigidbody2D;
class Collider2D;
struct Collision2D;
class RenderContext;

class PhysicsSystem
{
public:
	friend class Rigidbody2D;
	friend class Collider2D;

public:
	PhysicsSystem();
	~PhysicsSystem();

public:
	void BeginFrame();
	void Update( float deltaTime );
	void EndFrame();

	void Startup();
	void Shutdown();

public:
	Rigidbody2D* CreateRigidbody( float mass );
	Collider2D* CreateCollider( bool isTrigger = false, const Vec2& center = Vec2::ZERO, float radius = 0.0f, const Vec2& exstents = Vec2::ZERO, float rotation = 0.0f );
	Collider2D* CreateColliderAABB2( bool isTrigger = false, float width = 1.0f, float height = 1.0f, Vec2 center = Vec2::ZERO );
	Collider2D* CreateColliderDisc(  bool isTrigger = false, const Vec2& center = Vec2::ZERO, float radius = 1.0f );

	const Vec2& GetGravity() { return m_gravity; };
	void SetGravity( Vec2 gravity ) { m_gravity = gravity; };
	void SetRBSimType( Rigidbody2D* ridgidbody, ePhysicsSimulationType type );

#ifdef PLATFORM_WINDOWS
	void DebugRender( RenderContext* renderer, const Rgba& color ) const;
#endif

	void AddCollider( Collider2D* collider );
	void RemoveCollider( Collider2D* collider );
	void AddRigidbody( Rigidbody2D* rigidbody );
	void RemoveRigidbody( Rigidbody2D* rigidbody );
	void RemoveRigidbodyWoutDeleting( Rigidbody2D* rigidbody );

private:
	void RunStep( float deltaTime );
	void PushTransformsToGame();
	void PushTransformsToEngine();

	void ResolveALLDynamicVSStatic( bool handleResponse );
	void MoveDynamicVSStatic( Rigidbody2D* dynamicRB, Rigidbody2D* staticRB, Collision2D* collisionInfo );
	void RespondDynamicVSStatic( Rigidbody2D* dynamicRB, Rigidbody2D* staticRB, const Collision2D* collisionInfo );
	void ResolveALLDynamicVSDynamic( bool handleResponse );
	void MoveDynamicVSDynamic( Rigidbody2D* dynamicRBa, Rigidbody2D* dynamicRBb, Collision2D* collisionInfo );
	void RespondDynamicVSDynamic( Rigidbody2D* dynamicRBa, Rigidbody2D* dynamicRBb, const Collision2D* collisionInfo );
	void ResolveTirggers();

	void Cleanup();

private:
	std::vector<Collider2D*> m_colliders;
	std::vector<Rigidbody2D*> m_rigidbodies[NUM_PHYSICS_SIM_TYPES];
	Vec2 m_gravity = Vec2( 0.0f, -9.8f );
};


