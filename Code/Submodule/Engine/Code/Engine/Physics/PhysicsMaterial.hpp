#pragma once



struct PhysicsMaterial
{
public:
	PhysicsMaterial();

public:
	float	m_restitution	= 1.0f;
	float	m_friction		= 0.0f; 
	float	m_drag			= 0.0f;
	float	m_angularDrag	= 0.0f;
};