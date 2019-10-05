#pragma once
#include <vector>
#include <string>

#include "Engine/Core/XML/XMLUtils.hpp"
//--------------------------------------------------------------------------
#ifdef _WIN32
#define PLATFORM_WINDOWS

class RenderContext;
struct Rgba;

#endif
//--------------------------------------------------------------------------

enum eColliderType2D 
{
	COLLIDER_DEFAULT,
	COLLIDER_AABB2, 
	COLLIDER_DISC,
	COLLIDER_PILLBOX,
	NUM_COLLIDER_TYPES

};

class Rigidbody2D;
struct Collision2D;

class Collider2D
{
	friend class PhysicsSystem;
public:
	Collider2D( bool isTrigger ) : m_isTrigger(isTrigger){}
	~Collider2D(){}

public:
	void SetRigidbody( Rigidbody2D* rigidbody );
	Rigidbody2D* GetRigidbody() { return m_rigidbody; }
	void SetTrigger( bool isTrigger );

	bool IsTouching( Collider2D* otherCollider, Collision2D* collisionInfo = nullptr );
	eColliderType2D GetType() { return m_type; };
	bool IsColliding() { return m_isColliding; }
	virtual float GetMomentOfInertia() = 0;
	virtual tinyxml2::XMLElement* GetAsXMLElemnt( tinyxml2::XMLDocument* doc ) const = 0;

#ifdef PLATFORM_WINDOWS
	// Note, only define DebugRender if you are in windows.
	virtual void DebugRender( RenderContext* renderer, const Rgba& color ) const = 0;
#endif

	void SetOnEnter( const std::string& onEnter ) { m_onEnterEvent = onEnter; };
	void SetOnExit( const std::string& onExit ) { m_onExitEvent = onExit; };
	void SetOnHit( const std::string& onHit ) { m_hitEvent = onHit; };
	void SetOnStay( const std::string& onStay ) { m_onStayEvent = onStay; };

	void FireHitEvent( Collider2D* colWith );
	void FireEnterEvent( Collider2D* colWith );
	void FireExitEvent (Collider2D* colWith );
	void FireStayEvent( Collider2D* colWith );
	void ExitAllColliders();

private:
	bool IsOverlapped( Collider2D* collider ) const;
	bool EnterCollider( Collider2D* collider );
	bool ExitCollider( Collider2D* collider );

private:
	std::string m_hitEvent = "";
	std::string m_onEnterEvent = "";
	std::string m_onStayEvent = "";
	std::string m_onExitEvent = "";

protected:
	Rigidbody2D *m_rigidbody = nullptr;   // rigidbody I belong to
	eColliderType2D m_type = COLLIDER_DEFAULT; 
	bool m_isColliding = false;
	bool m_isTrigger = false;
	bool m_isGarbage = false;

	std::vector<Collider2D*> m_curColliding;

};
