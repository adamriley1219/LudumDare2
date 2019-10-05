#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Shaders/UniformBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Textures/DepthStencilTargetView.hpp"
#include "Engine/Renderer/RenderUtil.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Frustum.hpp"
#include "Engine/Math/AABB3.hpp"
#include <vector>
#include <string>

//--------------------------------------------------------------------------
/**
* Camera
*/
Camera::Camera()
{
	m_model.SetT( Vec4( 0.0f, 0.0f, 10.0f, 1.0f ) );
}

//--------------------------------------------------------------------------
/**
* Camera
*/
Camera::Camera( const char* text )
{
	std::vector<std::string> splits = SplitStringOnDelitmiter( text, "," );
	int size = (int) splits.size();
	GUARANTEE_OR_DIE( size == 4, Stringf( "Camera::Camera( const char* text ) being called with %d splits", size ) );

	m_bottomLeft.x = StringToFloat( splits[0] );
	m_bottomLeft.y = StringToFloat( splits[1] );
	m_topRight.x = StringToFloat( splits[2] );
	m_topRight.y = StringToFloat( splits[3] );
}

//--------------------------------------------------------------------------
/**
* ~Camera
*/
Camera::~Camera()
{
	delete m_cameraProjUBO;
	delete m_timeUBO;
	m_timeUBO = nullptr;
	m_cameraProjUBO = nullptr;
	m_colorTargetView = nullptr;
}

//--------------------------------------------------------------------------
/**
* SetPerspectiveProjection
*/
void Camera::SetPerspectiveProjection( float fov, float nearZ, float farZ )
{
	Vec2& dims = m_viewportInfo.screenDims;
	IntVec2& res = m_colorTargetView->m_size;
	m_aspectRatio = ( (float) res.x * dims.x ) / ( dims.y * (float) res.y );

	m_farZ = farZ;
	m_nearZ = nearZ;
	m_fov = fov;
	m_projection = Matrix44::MakeProjectionPerspective( fov, m_aspectRatio, nearZ, farZ );
}

//--------------------------------------------------------------------------
/**
* SetOrthographicProjection
*/
void Camera::SetOrthographicProjection( Vec2 min, Vec2 max, float nearZ /*= 0.0f*/, float farZ /*= 1000.0f */ )
{
	m_bottomLeft = min;
	m_topRight = max;
	m_farZ = farZ;
	m_nearZ = nearZ;
	m_aspectRatio = ( m_topRight.x - m_bottomLeft.x ) / ( m_topRight.y - m_bottomLeft.y );
	m_projection = Matrix44::MakeProjectionOrthographic( m_bottomLeft, m_topRight, nearZ, farZ );
}

//--------------------------------------------------------------------------
/**
* SetModelMatrix
*/
void Camera::SetModelMatrix( Matrix44 camModle )
{
	m_model = camModle;
	camModle.InvertOrthonormal();
	m_view = camModle;
}

//--------------------------------------------------------------------------
/**
* SetModelMatrix
*/
void Camera::SetModelMatrix( Vec3 pos )
{
	m_model.m_values[Matrix44::Tx] = pos.x;
	m_model.m_values[Matrix44::Ty] = pos.y;
	m_model.m_values[Matrix44::Tz] = pos.z;

 	m_view = m_model;
 	m_view.InvertOrthonormal();
}

//--------------------------------------------------------------------------
/**
* SetModelMatrix
*/
void Camera::SetModelMatrix( Vec3 pos, Vec3 euler )
{
	Matrix44 trans = Matrix44::MakeTranslation3D( pos );
	Matrix44 rot = Matrix44::FromEuler( euler, ROTATION_ORDER_ZXY );

	m_model = trans * rot;
	m_view = m_model;
	m_view.InvertOrthonormal();
}

//--------------------------------------------------------------------------
/**
* GetClientToWorld
*/
Vec3 Camera::GetClientToWorld( const IntVec2& input, float depth /*= 1.0f */ ) const
{
	return GetNDCToWorld( GetClientToNDC( input, depth ) );
}

//--------------------------------------------------------------------------
/**
* GetClientToNDC
*/
Vec3 Camera::GetClientToNDC( const IntVec2& input, float depth /*= 1.0f */ ) const
{
	return Vec3( RangeMapFloat( (float) input.x, 0.0f, (float) m_clientWidth, -1.0f, 1.0f ), RangeMapFloat( (float) input.y, 0.0f, (float) m_clientHeight, 1.0f, -1.0f ), depth );
}

//--------------------------------------------------------------------------
/**
* GetNDCToWorld
*/
Vec3 Camera::GetNDCToWorld( const Vec3& input ) const
{
	Vec4 res = ( m_projection * m_view ).GetInverted() * Vec4( input, 1.0f );
	return Vec3( res / res.w );
}

//--------------------------------------------------------------------------
/**
* GetWorldToClient
*/
IntVec2 Camera::GetWorldToClient( const Vec3& input ) const
{
	return GetNDCToClient( GetWorldToNDC( input ) );
}

//--------------------------------------------------------------------------
/**
* GetNDCToClient
*/
IntVec2 Camera::GetNDCToClient( const Vec3& input ) const
{
	return IntVec2( (int) RangeMapFloat( (float) input.x, -1.0f, 1.0f, 0.0f, (float) m_clientWidth ), (int) RangeMapFloat( (float) input.y, 1.0f, -1.0f, 0.0f, (float) m_clientHeight ) );
}

//--------------------------------------------------------------------------
/**
* GetWorldToNDC
*/
Vec3 Camera::GetWorldToNDC( const Vec3& input ) const
{
	Vec4 res = ( m_projection * m_view ) * Vec4( input, 1.0f );
	return Vec3( res / res.x );
}

//--------------------------------------------------------------------------
/**
* GetRawScreenPointToWorldRay
*/
Ray3 Camera::GetRawScreenPointToWorldRay( const IntVec2& pos ) const
{
	Vec3 near =	GetClientToWorld( pos, 0.0f );
	Vec3 far =	GetClientToWorld( pos, 1.0f );

	return Ray3( near, far - near );
}

//--------------------------------------------------------------------------
/**
* GetOrthoBottomLeft
*/
Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_bottomLeft;
}

//--------------------------------------------------------------------------
/**
* GetOrthoTopRight
*/
Vec2 Camera::GetOrthoTopRight() const
{
	return m_topRight;
}

//--------------------------------------------------------------------------
/**
* GetAspectRatio
*/
float Camera::GetAspectRatio() const
{
	return m_aspectRatio;
}

//--------------------------------------------------------------------------
/**
* SetColorTargetView
*/
void Camera::SetColorTargetView( ColorTargetView* colorTargetView )
{
	m_colorTargetView = colorTargetView;
}

//--------------------------------------------------------------------------
/**
* SetDepthTargetView
*/
void Camera::SetDepthTargetView( DepthStencilTargetView* DTV )
{
	m_depthTargetView = DTV;
}

//--------------------------------------------------------------------------
/**
* UpdateUniformBuffer
*/
void Camera::UpdateUniformBuffer( RenderContext* ctx )
{
	if (m_cameraProjUBO == nullptr) 
	{
		m_cameraProjUBO = new UniformBuffer( ctx ); 
	}
	if( m_timeUBO == nullptr )
	{
		m_timeUBO = new UniformBuffer( ctx );
	}

	m_clientHeight = ctx->GetDepthTargetView()->GetHeight();
	m_clientWidth = ctx->GetDepthTargetView()->GetWidth();

	CameraUBOData cpuData;
	cpuData.view = m_view; 
	cpuData.proj = m_projection; 
	cpuData.camPos =  Vec3( m_model.GetT() ); 

	frameBufferData frameData;
	frameData.time = (float) GetCurrentTimeSeconds();
	frameData.cosTime = CosDegrees( frameData.time );
	frameData.worldHeight = 50.0f;
	frameData.worldWidth = 100.0f;

	m_cameraProjUBO->CopyCPUToGPU( &cpuData, sizeof(cpuData) );
	m_timeUBO->CopyCPUToGPU( &frameData, sizeof(frameData) );
}


//--------------------------------------------------------------------------
/**
* SetViewportInfo
*/
void Camera::SetViewportInfo( const CameraViewportInfo& viewportInfo )
{
	m_viewportInfo = viewportInfo;
}

//--------------------------------------------------------------------------
/**
* SetViewportInfo
*/
void Camera::SetViewportInfo( const Vec2& topLeftPivot, const Vec2& screenDimentions, const Vec2& depthMinMax /*= Vec2( 0.0f, 1.0f ) */ )
{
	CameraViewportInfo viewportInfo;
	viewportInfo.screenDims = screenDimentions;
	viewportInfo.topLeftPivot = topLeftPivot;
	viewportInfo.depthMinMax = depthMinMax;
	SetViewportInfo( viewportInfo );
}

//--------------------------------------------------------------------------
/**
* GetViewportInfo
*/
CameraViewportInfo Camera::GetViewportInfo() const
{
	return m_viewportInfo;
}

//--------------------------------------------------------------------------
/**
* GetWorldFrustum
*/
Frustum Camera::GetWorldFrustum() const
{
	AABB3 ndc = AABB3( Vec3( -1.0f, -1.0f, 0.0f ), Vec3( 1.0f, 1.0f, 1.0f ) );
	Matrix44 worldToNDC = GetProjectionMatrix() * GetViewMatrix();
	Matrix44 NDCToWorld = worldToNDC.GetInverted();

	Vec3 corners[8];
	ndc.GetCorners( corners );
	TransformPoints( 8, corners, NDCToWorld );
	
	return Frustum( corners );
}

//--------------------------------------------------------------------------
/**
* GetWorldFrustumFromClientRegion
*/
Frustum Camera::GetWorldFrustumFromClientRegion( const AABB2& clientRegion ) const
{
	Vec3 min = GetClientToNDC( IntVec2(clientRegion.GetBottomLeft() ), 0.0f );
	Vec3 max = GetClientToNDC( IntVec2(clientRegion.GetTopRight() ), 1.0f );

	AABB3 subNdc = AABB3( min, max );
	Matrix44 worldToNDC = GetProjectionMatrix() * GetViewMatrix();
	Matrix44 NDCToWorld = worldToNDC.GetInverted();

	Vec3 corners[8];
	subNdc.GetCorners( corners );
	TransformPoints( 8, corners, NDCToWorld );

	return Frustum( corners );
}

//--------------------------------------------------------------------------
/**
* GetForward
*/
Vec3 Camera::GetForward() const
{
	return Vec3( m_model.GetK() ).GetNormalized();
}

//--------------------------------------------------------------------------
/**
* GetUp
*/
Vec3 Camera::GetUp() const
{
	return Vec3( m_model.GetJ() ).GetNormalized();
}

//--------------------------------------------------------------------------
/**
* GetRight
*/
Vec3 Camera::GetRight() const
{
	return Vec3( m_model.GetI() ).GetNormalized();
}

//--------------------------------------------------------------------------
/**
* GetPosition
*/
Vec3 Camera::GetPosition() const
{
	return Vec3( m_model.GetT() );
}

