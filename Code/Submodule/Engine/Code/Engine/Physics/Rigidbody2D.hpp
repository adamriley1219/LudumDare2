#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Physics/Transform2D.hpp"
#include "Engine/Physics/PhysicsMaterial.hpp"
#include "Engine/Core/XML/XMLUtils.hpp"

class RenderContext;
class PhysicsSystem;
class Collider2D;
struct Transform2D;
struct Rgba;

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif

enum ePhysicsSimulationType
{
	PHYSICS_SIM_STATIC,	 // does not move
	PHYSICS_SIM_DYNAMIC, // does move 
	NUM_PHYSICS_SIM_TYPES
}; 


class Rigidbody2D
{
	friend class PhysicsSystem;
public:
	explicit Rigidbody2D( PhysicsSystem* controllingSystem );
	~Rigidbody2D();

public:
#ifdef PLATFORM_WINDOWS
	void DebugRender( RenderContext* renderer, const Rgba& color );
#endif

	void Move( float deltaTime );

	void AddForce( Vec2 force ) { m_frameForces += force; }
	void AddTorque( float torque ) { m_frameTorque += torque; }

	Vec2 GetImpactVelocity( Vec2 worldPos ) const;
	void ApplyImpulse( Vec2 linearImpulse, float angularImpulse );
	void ApplyImpulseAt( Vec2 impulse, Vec2 worldPos );

	// Setters
	Collider2D* SetCollider( Collider2D* collider );
	void SetSimulationType( ePhysicsSimulationType type );
	void SetOriginalSimulationType( ePhysicsSimulationType type );
	void ResetSimulationType();
	void SetObject( void* affectedObj, Transform2D* objTrans );
	void SetMass( float mass );
	void SetPhyMaterial( float restitution, float friction, float drag = 0.0f, float angularDrag = 0.0f );
	void SetRestrictions( bool xMovement, bool yMovement, bool rotation );
	void SetAngularVelocity( float angularVel );

	// Getters
	void* GetOwner() { return m_object; }
	float GetMass() const { return m_mass; };
	float GetMomentOfInertia() const { return m_momentOfI; };
	Vec2 GetVelocity() const { return m_velocity; };
	float GetAngularVelocity() const { return m_angularVelocity; };
	float GetFriction() const { return m_phyMat.m_friction; };
	float GetRestitution() const { return m_phyMat.m_restitution; };
	float GetDrag() const { return m_phyMat.m_drag; };
	float GetAngularDrag() const { return m_phyMat.m_angularDrag; };
	float IsXRestricted() const { return m_xRestructed; };
	float IsYRestricted() const { return m_yRestructed; };
	float IsRotRestricted() const { return m_rotRestructed; };

	const Transform2D& GetTransform() const;
	ePhysicsSimulationType GetSimulationType() const;

	// Updates
	void UpdateMomentOfInertia();

public:
	tinyxml2::XMLElement* GetAsXMLElement( tinyxml2::XMLDocument* doc ) const;

private:
	PhysicsSystem *m_system						= nullptr; 		// system this rigidbody belongs to; 
	void *m_object								= nullptr; 		// user (game) pointer for external use
	Transform2D *m_objectTransform				= nullptr; 		// what does this rigidbody affect

	Transform2D m_transform;									// rigidbody transform (mimics the object at start of frame, and used to tell the change to object at end of frame)
	Vec2 m_gravityScale							= Vec2::ONE;	// how much are we affected by gravity
	Vec2 m_velocity								= Vec2::ZERO;
	float m_angularVelocity						= 0.0f;
	float m_mass								= 1.f;  		// how heavy am I
	float m_momentOfI							= 1.0f;
	PhysicsMaterial m_phyMat;

	Collider2D *m_collider						= nullptr;		// my shape; (could eventually be made a set)
	ePhysicsSimulationType m_simType			= PHYSICS_SIM_STATIC;
	ePhysicsSimulationType m_originalSimType	= PHYSICS_SIM_STATIC;

	Vec2 m_frameForces							= Vec2::ZERO;
	float m_frameTorque							= 0.0f;

	bool m_xRestructed							= false;
	bool m_yRestructed							= false;
	bool m_rotRestructed						= false;

	bool m_isGarbage							= false;

};