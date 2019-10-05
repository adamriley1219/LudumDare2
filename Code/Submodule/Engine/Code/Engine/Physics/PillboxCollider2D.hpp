#pragma once
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Math/Pillbox2.hpp"

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif

class PillboxCollider2D
	: public Collider2D
{
	friend class PhysicsSystem;
private:
	explicit PillboxCollider2D( bool isTrigger, const Pillbox2& pill );

public:
#ifdef PLATFORM_WINDOWS
	void DebugRender( RenderContext* renderer, const Rgba& color ) const;
#endif
	virtual float GetMomentOfInertia();
	virtual tinyxml2::XMLElement* GetAsXMLElemnt( tinyxml2::XMLDocument* doc ) const;


public:
	Pillbox2 GetLocalShape() const;
	Pillbox2 GetWorldShape() const;

private:
	Pillbox2 m_localShape;
};
