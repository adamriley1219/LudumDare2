#pragma once
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Math/AABB2.hpp"

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif


class AABB2Collider2D
	: public Collider2D
{
	friend class PhysicsSystem;
private:
	explicit AABB2Collider2D( bool isTrigger, const AABB2& box );

public:
#ifdef PLATFORM_WINDOWS
	void DebugRender( RenderContext* renderer, const Rgba& color ) const;
#endif
	virtual tinyxml2::XMLElement* GetAsXMLElemnt( tinyxml2::XMLDocument* doc ) const;
	virtual float GetMomentOfInertia();

public:
	AABB2 GetLocalShape() const;
	AABB2 GetWorldShape() const;

private:
	AABB2 m_localShape;
};
