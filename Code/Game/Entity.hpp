#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/GameCommon.hpp"

class Entity
{
public:
	Entity();
	virtual ~Entity();

	virtual void Die();
	bool IsAlive() const;
	bool IsGarbage() const;
	void DebugRenderCosmetic() const;
	void DebugRenderPhysics() const;
	virtual void Render() const;
	virtual void Update( float deltaSeconds ) = 0;
	bool IsOffScreen() const;

	// Setters
	void SetAcceleration( bool on );
	void SetVelocity( Vec2 velocity );
	void SetRotationDirection( int rotationDirection );
	void SetRotation( float rotationDegrees ); 

	// Getters
	Vec2 GetForwardVector() const;
	Vec2 GetPosition() const;
	float GetPhysicsRadius() const;
	float GetCosmeticRadius() const;
	float GetRotationDegrees() const;
	Rgba GetTint() const;

	// Game play
	void TakeDamage( float damage );
	float GetCollisionDamage(); 

protected:
	Vec2 m_velocity;
	Vec2 m_position;

	// Child should define before using.
	float m_physicsRadius;
	float m_cosmeticRadius;
	float m_acceleration;	
	float m_angularAcceleration;

	// Preset
	float m_orientationDegrees = 0.0f;
	float m_angularVelocity = 0.0f;

	float m_uniformScale = 1.0f;
	bool m_isAccelerating = false;
	int m_rotateDirection = 0; // 1 for counter clockwise, -1 for clockwise, 0 for no movement.

	bool m_isDead = false;
	bool m_isGarbage = false;

	// Gameplay
	float m_health = 0.0f;
	float m_collisionDamage = 1.0f;

	Rgba m_tint;
};

