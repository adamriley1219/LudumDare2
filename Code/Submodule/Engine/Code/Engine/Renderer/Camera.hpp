#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Ray3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Matrix44.hpp"

class ColorTargetView;
class UniformBuffer;
class RenderContext;
class DepthStencilTargetView;
class TimeBuffer;
struct Frustum;

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// all values are intended to be used in the form of 0 - 1
//--------------------------------------------------------------------------
struct CameraViewportInfo
{
	Vec2 topLeftPivot	= Vec2::ZERO;
	Vec2 screenDims		= Vec2::ONE;
	Vec2 depthMinMax	= Vec2( 0.0f, 1.0f );
};
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
class Camera
{
public:
	friend class RenderContext;

public:
	Camera();
	explicit Camera( const char* text );
	~Camera();

	void SetPerspectiveProjection( float fov, float nearZ, float farZ );
	void SetOrthographicProjection( Vec2 min, Vec2 max, float nearZ = 0.0f, float farZ = 1000.0f );
	
public:
	void SetModelMatrix( Vec3 pos );
	void SetModelMatrix( Vec3 pos, Vec3 euler );
	void SetModelMatrix( Matrix44 camModle );
	Matrix44 GetModelMatrix() const { return m_model; }
	Matrix44 GetViewMatrix() const { return m_view; }
	Matrix44 GetProjectionMatrix() const { return m_projection; }

public:
	Vec3 GetClientToWorld( const IntVec2& input, float depth = 1.0f ) const;
	Vec3 GetClientToNDC( const IntVec2& input, float depth = 1.0f ) const;
	Vec3 GetNDCToWorld( const Vec3& input ) const;
	IntVec2 GetWorldToClient( const Vec3& input ) const;
	IntVec2 GetNDCToClient( const Vec3& input ) const;
	Vec3 GetWorldToNDC( const Vec3& input ) const;
	Ray3 GetRawScreenPointToWorldRay( const IntVec2& pos ) const;

public:
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	float GetAspectRatio() const;
	void SetColorTargetView( ColorTargetView* colorTargetView );
	void SetDepthTargetView( DepthStencilTargetView* DTV );

	void UpdateUniformBuffer( RenderContext* ctx ); 
	void SetViewportInfo( const Vec2& topLeftPivot, const Vec2& screenDimentions, const Vec2& depthMinMax = Vec2( 0.0f, 1.0f ) );
	void SetViewportInfo( const CameraViewportInfo& viewportInfo );
	CameraViewportInfo GetViewportInfo() const;

public:
	Frustum GetWorldFrustum() const;
	Frustum GetWorldFrustumFromClientRegion( const AABB2& clientRegion ) const;

public:
	Vec3 GetForward() const;
	Vec3 GetUp() const;
	Vec3 GetRight() const;
	Vec3 GetPosition() const;

public:
	float GetNear() const { return m_nearZ; };
	float GetFar() const { return m_farZ; };
	float GetFOV() const { return m_fov; };

private:
	Vec2 m_bottomLeft	= Vec2::ONE * -1.0f;
	Vec2 m_topRight		= Vec2::ONE;	
	float m_nearZ		= 1.0f;
	float m_farZ		= 100.0f;
	float m_fov			= 90.0f;
	float m_aspectRatio = 1.0f;
	int m_clientHeight	= 1;
	int m_clientWidth	= 1;
	CameraViewportInfo m_viewportInfo;

private:
	Matrix44 m_projection	= Matrix44::IDENTITY;
	Matrix44 m_view			= Matrix44::IDENTITY;
	Matrix44 m_model		= Matrix44::IDENTITY;

private:
	ColorTargetView* m_colorTargetView			= nullptr;
	DepthStencilTargetView* m_depthTargetView	= nullptr;
	UniformBuffer *m_cameraProjUBO				= nullptr;
	UniformBuffer* m_timeUBO					= nullptr;
};
