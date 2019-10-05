#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"

//--------------------------------------------------------------------------
/**
* Entity
*/
Entity::Entity()
{
	m_position = Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ); // start in middle of screen
	m_velocity = Vec2( 0.0f, 0.0f );
	m_tint = Rgba( 1.0f, 1.0f, 1.0f );
}


//--------------------------------------------------------------------------
/**
* Entity
*/
Entity::~Entity()
{
}

//--------------------------------------------------------------------------
/**
* Kill
*/
void Entity::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}

//--------------------------------------------------------------------------
/**
* isDying
*/
bool Entity::IsAlive() const
{
	return !m_isDead;
}

//--------------------------------------------------------------------------
/**
* DieisGarbage
*/
bool Entity::IsGarbage() const
{
	return m_isGarbage;
}

//--------------------------------------------------------------------------
/**
* Render
*/
void Entity::Render() const
{

}

//--------------------------------------------------------------------------
/**
* DebugRenderDebugRenderCosmetic
*/
void Entity::DebugRenderCosmetic() const
{

}

//--------------------------------------------------------------------------
/**
* DebugRenderPhysics
*/
void Entity::DebugRenderPhysics() const
{

}

//--------------------------------------------------------------------------
/**
* isGarbageisOffScreen
*/
bool Entity::IsOffScreen() const
{
	// 0.0f represents the lower left corner of the sceen.
	return 0.0f > m_position.x + GetCosmeticRadius()
		|| 0.0f > m_position.y + GetCosmeticRadius()
		|| WORLD_WIDTH < m_position.x - GetCosmeticRadius()
		|| WORLD_HEIGHT < m_position.y - GetCosmeticRadius();
}

//--------------------------------------------------------------------------
/**
* isOffScreengetForwardVector
*/
Vec2 Entity::GetForwardVector() const
{
	Vec2 forwardVec = Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) ); 
	forwardVec.Normalize();
	return forwardVec;
}

//--------------------------------------------------------------------------
/**
* setAcceleration
*/
void Entity::SetAcceleration( bool on )
{
	m_isAccelerating = on;
}

//--------------------------------------------------------------------------
/**
* setVelocity
*/
void Entity::SetVelocity( Vec2 velocity )
{
	m_velocity = velocity;
}

//--------------------------------------------------------------------------
/**
* setRotationDirection
*/
void Entity::SetRotationDirection( int rotationDirection )
{
	m_rotateDirection = rotationDirection;
}

//--------------------------------------------------------------------------
/**
* SetRotation
*/
void Entity::SetRotation( float rotationDegrees )
{
	m_orientationDegrees = rotationDegrees;
}

//--------------------------------------------------------------------------
/**
* getVelocity
*/
Vec2 Entity::GetPosition() const
{
	return m_position;
}

//--------------------------------------------------------------------------
/**
* GetPhysicsRadius
*/
float Entity::GetPhysicsRadius() const
{
	return m_physicsRadius * m_uniformScale;
}

//--------------------------------------------------------------------------
/**
* GetCosmeticRadius
*/
float Entity::GetCosmeticRadius() const
{
	return m_cosmeticRadius * m_uniformScale;
}

//--------------------------------------------------------------------------
/**
* SetRotationGetRotationDegrees
*/
float Entity::GetRotationDegrees() const
{
	return m_orientationDegrees;
}

//--------------------------------------------------------------------------
/**
* EntityGetColor
*/
Rgba Entity::GetTint() const
{
	return m_tint;
}

//--------------------------------------------------------------------------
/**
* TakeDamage
*/
void Entity::TakeDamage( float damage )
{
	if( ( m_health -= damage ) <= 0.0f )
	{
		Die();
	}
}

//--------------------------------------------------------------------------
/**
* GetCollisionDamage
*/
float Entity::GetCollisionDamage()
{
	return m_collisionDamage;
}
