#pragma once
#include "Engine/Math/Disc.hpp"
#include "Engine/Physics/Collider2D.hpp"

#ifdef _WIN32
#define PLATFORM_WINDOWS
class RenderContext;
#endif

struct Rgba;


class DiscCollider2D
	: public Collider2D
{
	friend class PhysicsSystem;
private:
	explicit DiscCollider2D( bool isTrigger, const Vec2& center, float radius );

public:
#ifdef PLATFORM_WINDOWS
	void DebugRender( RenderContext* renderer, const Rgba& color ) const;
#endif
	virtual tinyxml2::XMLElement* GetAsXMLElemnt( tinyxml2::XMLDocument* doc ) const;
	virtual float GetMomentOfInertia();

public:
	Disc GetLocalShape() const;
	Disc GetWorldShape() const;

private:
	Disc m_localShape;

};
