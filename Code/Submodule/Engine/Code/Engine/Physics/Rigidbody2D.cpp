#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Math/OBB2.hpp"



//--------------------------------------------------------------------------
/**
* Rigidbody2D
*/
Rigidbody2D::Rigidbody2D( PhysicsSystem* controllingSystem )
{
	m_system = controllingSystem;
}

//--------------------------------------------------------------------------
/**
* ~Rigidbody2D
*/
Rigidbody2D::~Rigidbody2D()
{

}

//--------------------------------------------------------------------------
/**
* Move
*/
void Rigidbody2D::Move( float deltaTime )
{
	if( m_simType == ePhysicsSimulationType::PHYSICS_SIM_DYNAMIC )
	{
		m_frameForces += m_gravityScale * m_system->m_gravity;
		Vec2 acc				= m_frameForces * m_mass;
		m_velocity				+= acc * deltaTime;
		m_velocity				= m_velocity * ( 1.0f - m_phyMat.m_drag * deltaTime );
		if( m_xRestructed )
		{
			m_velocity.x = 0.0f;
		}
		if( m_yRestructed )
		{
			m_velocity.y = 0.0f;
		}
		m_transform.m_position	+= m_velocity * deltaTime;



		if( !m_rotRestructed )
		{
			float angularAcceleration = m_frameTorque / m_momentOfI; 
			m_angularVelocity += ConvertRadiansToDegrees( angularAcceleration ) * deltaTime; 
			m_angularVelocity = m_angularVelocity * ( 1.0f - m_phyMat.m_angularDrag * deltaTime );
			m_transform.m_rotation += m_angularVelocity * deltaTime; 
		}
		else
		{
			m_angularVelocity = 0.0f;
		}
	}
	m_frameForces = Vec2::ZERO;
	m_frameTorque = 0.0f;
}

//--------------------------------------------------------------------------
/**
* DebugRender
*/
#ifdef PLATFORM_WINDOWS
void Rigidbody2D::DebugRender( RenderContext* renderer, const Rgba& color )
{
	m_collider->DebugRender( renderer, color );
}
#endif // PLATFORM_WINDOWS



//--------------------------------------------------------------------------
/**
* GetImpactVelocity
*/
Vec2 Rigidbody2D::GetImpactVelocity( Vec2 worldPos ) const
{
	Vec2 disp = m_transform.m_position - worldPos;
	return m_velocity - disp.GetRotated90Degrees() * ConvertDegreesToRadians( m_angularVelocity );
}

//--------------------------------------------------------------------------
/**
* ApplyImpulse
*/
void Rigidbody2D::ApplyImpulse( Vec2 linearImpulse, float angularImpulse )
{
	m_velocity += linearImpulse / m_mass;
	if( m_xRestructed )
	{
		m_velocity.x = 0.0f;
	}
	if( m_yRestructed )
	{
		m_velocity.y = 0.0f;
	}
	if( !m_rotRestructed )
	{
		m_angularVelocity += ConvertRadiansToDegrees( angularImpulse / m_momentOfI );
	}
}

//--------------------------------------------------------------------------
/**
* ApplyImpulseAt
*/
void Rigidbody2D::ApplyImpulseAt( Vec2 linearImpulse, Vec2 worldPos )
{
	// apply angular impulse
	Vec2 localImpact = m_transform.m_position - worldPos; 
	Vec2 directionOfTorque = localImpact.GetRotated90Degrees() * -1.0f; // Possibly *-1
	float impulseTorque = DotProduct( linearImpulse, directionOfTorque ); 

	ApplyImpulse( linearImpulse, impulseTorque );
}

//--------------------------------------------------------------------------
/**
* SetCollider
*/
Collider2D* Rigidbody2D::SetCollider( Collider2D* collider )
{
	m_collider = collider;
	m_collider->SetRigidbody( this );
	UpdateMomentOfInertia();
	return m_collider;
}

//--------------------------------------------------------------------------
/**
* SetSimulationType
*/
void Rigidbody2D::SetSimulationType( ePhysicsSimulationType type )
{
	m_system->SetRBSimType( this, type );
}

//--------------------------------------------------------------------------
/**
* SetOriginalSimulationType
*/
void Rigidbody2D::SetOriginalSimulationType( ePhysicsSimulationType type )
{
	m_system->SetRBSimType( this, type );
	m_originalSimType = type;
}

//--------------------------------------------------------------------------
/**
* ResetSimulationType
*/
void Rigidbody2D::ResetSimulationType()
{
	m_system->SetRBSimType( this, m_originalSimType );
	m_velocity = Vec2::ZERO;
}

//--------------------------------------------------------------------------
/**
* SetObject
*/
void Rigidbody2D::SetObject( void* affectedObj, Transform2D* objTrans )
{
	m_object = affectedObj;
	m_objectTransform = objTrans;
	m_transform = *objTrans;
}

//--------------------------------------------------------------------------
/**
* SetMass
*/
void Rigidbody2D::SetMass( float mass )
{
	m_mass = mass;
	UpdateMomentOfInertia();
}

//--------------------------------------------------------------------------
/**
* SetPhyMaterial
*/
void Rigidbody2D::SetPhyMaterial( float restitution, float friction, float drag /*= 0.0f*/, float angularDrag /*= 0.0f */ )
{
	m_phyMat.m_restitution = restitution;
	m_phyMat.m_friction = friction;
	m_phyMat.m_drag = drag;
	m_phyMat.m_angularDrag = angularDrag;
}

//--------------------------------------------------------------------------
/**
* SetRestrictions
*/
void Rigidbody2D::SetRestrictions( bool xMovement, bool yMovement, bool rotation )
{
	m_xRestructed = xMovement;
	m_yRestructed = yMovement;
	m_rotRestructed = rotation;
}

//--------------------------------------------------------------------------
/**
* SetAngularVelocity
*/
void Rigidbody2D::SetAngularVelocity( float angularVel )
{
	m_angularVelocity = angularVel;
}

//--------------------------------------------------------------------------
/**
* GetTransform
*/
const Transform2D& Rigidbody2D::GetTransform() const
{
	return m_transform;
}

//--------------------------------------------------------------------------
/**
* GetSimulationType
*/
ePhysicsSimulationType Rigidbody2D::GetSimulationType() const
{
	return m_simType;
}

//--------------------------------------------------------------------------
/**
* UpdateMomentOfInertia
*/
void Rigidbody2D::UpdateMomentOfInertia()
{
	m_momentOfI = m_collider->GetMomentOfInertia();
}

//--------------------------------------------------------------------------
/**
* GetAsXMLElement
*/
tinyxml2::XMLElement* Rigidbody2D::GetAsXMLElement( tinyxml2::XMLDocument* doc ) const
{

	tinyxml2::XMLElement* rbElement = doc->NewElement( "rigidbody" );

	rbElement->SetAttribute( "type", Stringf( "%s", GetSimulationType() == PHYSICS_SIM_STATIC ? "static" : "dynamic" ).c_str() );
	rbElement->SetAttribute( "mass", Stringf( "%f", GetMass() ).c_str() );
	rbElement->SetAttribute( "velocity", Stringf( "%f,%f", GetVelocity().x, GetVelocity().y ).c_str() );
	rbElement->SetAttribute( "angularVelocity", Stringf( "%f", GetAngularVelocity() ).c_str() );
	rbElement->SetAttribute( "friction", Stringf( "%f", GetFriction() ).c_str() );
	rbElement->SetAttribute( "restitution", Stringf( "%f", GetRestitution() ).c_str() );
	rbElement->SetAttribute( "drag", Stringf( "%f", GetDrag() ).c_str() );
	rbElement->SetAttribute( "angularDrag", Stringf( "%f", GetAngularDrag() ).c_str() );
	rbElement->SetAttribute( "xRestricted", Stringf( "%s", IsXRestricted() ? "true" : "false" ).c_str() );
	rbElement->SetAttribute( "yRestricted", Stringf( "%s", IsYRestricted()  ? "true" : "false" ).c_str() );
	rbElement->SetAttribute( "rotRestricted", Stringf( "%s", IsRotRestricted() ? "true" : "false" ).c_str() );

	return rbElement;
}
