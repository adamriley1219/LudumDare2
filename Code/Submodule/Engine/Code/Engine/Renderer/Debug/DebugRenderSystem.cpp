#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"
#include "Engine/Renderer/Debug/DebugRenderObject.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Renderer/Debug/DebugRenderObjectMesh.hpp"
#include "Engine/Renderer/Debug/DebugRenderObjectText3D.hpp"
#include "Engine/Renderer/Debug/DebugRenderObjectText2D.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Shaders/Shader.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

#include "Engine/Core/Graphics/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex/MeshCPU.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Core/Time/Clock.hpp"

#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Capsule3.hpp"

#include <stdarg.h>

//--------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
* DebugRenderSystem
*/
DebugRenderSystem::DebugRenderSystem( RenderContext* ctx, float screenHeight, float screenWidth, const std::string& nameOfBitmapFont )
	: m_renderContext( ctx ), m_screenHeight( screenHeight ), m_screenWidth( screenWidth ), m_fontName( nameOfBitmapFont )
{

}

//--------------------------------------------------------------------------
/**
* ~DebugRender
*/
DebugRenderSystem::~DebugRenderSystem()
{
}

//--------------------------------------------------------------------------
/**
* Startup
*/
void DebugRenderSystem::Startup()
{
	g_theEventSystem->SubscribeEventCallbackFunction( "dr_clear", Command_Clear );
	g_theEventSystem->SubscribeEventCallbackFunction( "dr_toggle", Command_Toggle );
	g_theEventSystem->SubscribeEventCallbackFunction( "dr_open", Command_Open );
	g_theEventSystem->SubscribeEventCallbackFunction( "dr_close", Command_Close );
	m_alwaysShader = m_renderContext->CreateOrGetShaderFromXML( "Data/Shaders/alwaysDrawShader.xml" );
	m_greaterShader = m_renderContext->CreateOrGetShaderFromXML( "Data/Shaders/greaterDrawShader.xml" );
	m_lequalShader = m_renderContext->CreateOrGetShaderFromXML( "Data/Shaders/lessOrEqualDrawShader.xml" );
	m_wireShader = m_renderContext->CreateOrGetShaderFromXML( "Data/Shaders/lequalWireframeShader.xml" );

	m_wireframeSphere = new MeshCPU();
	CPUMeshAddUVSphere( m_wireframeSphere, Vec3::ZERO, 1.0f );
}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void DebugRenderSystem::Shutdown()
{
	Clear();
}

//--------------------------------------------------------------------------
/**
* BeginFrame
*/
void DebugRenderSystem::BeginFrame()
{

}

//--------------------------------------------------------------------------
/**
* Update
*/
void DebugRenderSystem::Update() const
{
	float deltaTime = (float) Clock::Master.GetFrameTime();

	for( int typeIndex = 0; typeIndex < NUM_DEBUG_RENDER_MODES; ++typeIndex  )
	{
		for( int objIndex = 0; objIndex < (int) m_renderWorldObjects[typeIndex].size(); ++objIndex )
		{
			DebugRenderObject* object = m_renderWorldObjects[typeIndex][objIndex];
			if( object )
			{
				object->Update( deltaTime );
			}
		}
	}

	for( int objIndex = 0; objIndex < (int) m_renderScreenObjects.size(); ++objIndex )
	{
		DebugRenderObject* object = m_renderScreenObjects[objIndex];
		if( object )
		{
			object->Update( deltaTime );
		}
	}

	for( int objIndex = 0; objIndex < (int) m_renderScreenDebugMessages.size(); ++objIndex )
	{
		DebugRenderObjectText2D* object = m_renderScreenDebugMessages[objIndex];
		if( object )
		{
			object->Update( deltaTime );
		}
	}
}


//--------------------------------------------------------------------------
/**
* RenderToCamera
*/
void DebugRenderSystem::RenderToCamera( Camera* camera ) const
{
	if( !m_open )
	{
		return;
	}

	camera->SetColorTargetView( m_renderContext->GetColorTargetView() );
	camera->SetDepthTargetView( m_renderContext->GetDepthTargetView() );
	m_renderContext->BeginCamera( camera );

	m_renderContext->BindShader( m_wireShader );
	m_renderContext->BindSampler( SAMPLE_MODE_POINT );

	for( int objIndex = 0; objIndex < (int) m_renderWorldObjects[DEBUG_RENDER_WIREFRAME].size(); ++objIndex )
	{
		const DebugRenderObject* object = m_renderWorldObjects[DEBUG_RENDER_WIREFRAME][objIndex];
		if( object )
		{
			object->Render( m_renderContext, camera );
		}
	}

	m_renderContext->BindShader( m_lequalShader );
	m_renderContext->BindSampler( SAMPLE_MODE_POINT );

	for( int objIndex = 0; objIndex < (int) m_renderWorldObjects[DEBUG_RENDER_USE_DEPTH].size(); ++objIndex )
	{
		const DebugRenderObject* object = m_renderWorldObjects[DEBUG_RENDER_USE_DEPTH][objIndex];
		if( object )
		{
			object->Render( m_renderContext, camera );
		}
	}

	for( int objIndex = 0; objIndex < (int) m_renderWorldObjects[DEBUG_RENDER_XRAY].size(); ++objIndex )
	{
		const DebugRenderObject* object = m_renderWorldObjects[DEBUG_RENDER_XRAY][objIndex];
		if( object )
		{
			object->Render( m_renderContext, camera );
		}
	}

	m_renderContext->BindShader( m_greaterShader );
	m_renderContext->BindSampler( SAMPLE_MODE_POINT );

	for( int objIndex = 0; objIndex < (int) m_renderWorldObjects[DEBUG_RENDER_XRAY].size(); ++objIndex )
	{
		const DebugRenderObject* object = m_renderWorldObjects[DEBUG_RENDER_XRAY][objIndex];
		if( object )
		{
			object->RenderXRayOverlay( m_renderContext, camera );
		}
	}

	m_renderContext->BindShader( m_alwaysShader );
	m_renderContext->BindSampler( SAMPLE_MODE_POINT );

	for( int objIndex = 0; objIndex < (int) m_renderWorldObjects[DEBUG_RENDER_ALWAYS].size(); ++objIndex )
	{
		const DebugRenderObject* object = m_renderWorldObjects[DEBUG_RENDER_ALWAYS][objIndex];
		if( object )
		{
			object->Render( m_renderContext, camera );
		}
	}

	m_renderContext->EndCamera();
}

//--------------------------------------------------------------------------
/**
* RenderToScreen
*/
void DebugRenderSystem::RenderToScreen() const
{
	if( !m_open )
	{
		return;
	}
	float halfHeight = m_screenHeight * 0.5f;
	Camera camera;
	camera.SetOrthographicProjection( Vec2( -halfHeight * 2.0f, -halfHeight ), Vec2( halfHeight * 2.0f,  halfHeight ) );
	camera.SetModelMatrix( Matrix44::IDENTITY );
	camera.SetColorTargetView( m_renderContext->GetColorTargetView() );
	camera.SetDepthTargetView( m_renderContext->GetDepthTargetView() );
	m_renderContext->BeginCamera( &camera );
	m_renderContext->BindShader( m_alwaysShader );
	m_renderContext->BindSampler( SAMPLE_MODE_POINT );

	for( int objIndex = 0; objIndex < (int) m_renderScreenObjects.size(); ++objIndex )
	{
		DebugRenderObject* object = m_renderScreenObjects[objIndex];
		if( object )
		{
			object->Render( m_renderContext, &camera );
		}
	}

	float curPlacement = 0.01f;
	for( int objIndex = 0; objIndex < (int) m_renderScreenDebugMessages.size(); ++objIndex )
	{
		DebugRenderObjectText2D* object = m_renderScreenDebugMessages[objIndex];
		if( object )
		{
			if( curPlacement < m_screenHeight )
			{
				object->m_alignment = Vec2( 0.01f,  curPlacement / m_screenHeight );
				object->Render( m_renderContext, &camera );
				curPlacement +=  object->m_textHeight;
			}
			else
			{
				break;
			}
		}
	}

	m_renderContext->EndCamera();
}

//--------------------------------------------------------------------------
/**
* EndFrame
*/
void DebugRenderSystem::EndFrame()
{
	Update();
	GarbageCollection();
}

//--------------------------------------------------------------------------
/**
* GarbageCollection
*/
void DebugRenderSystem::GarbageCollection()
{
	for( int typeIndex = 0; typeIndex < NUM_DEBUG_RENDER_MODES; ++typeIndex  )
	{
		for( int objIndex = 0; objIndex < (int) m_renderWorldObjects[typeIndex].size(); ++objIndex )
		{
			DebugRenderObject*& object = m_renderWorldObjects[typeIndex][objIndex];
			if( object && object->m_garbage )
			{
				delete object;
				object = nullptr;
			}
		}
	}

	for( int objIndex = 0; objIndex < (int) m_renderScreenObjects.size(); ++objIndex )
	{
		DebugRenderObject*& object = m_renderScreenObjects[objIndex];
		if( object && object->m_garbage )
		{
			delete object;
			object = nullptr;
		}
	}

	for( int objIndex = 0; objIndex < (int) m_renderScreenDebugMessages.size(); ++objIndex )
	{
		DebugRenderObjectText2D*& object = m_renderScreenDebugMessages[objIndex];
		if( object && object->m_garbage )
		{
			delete object;
			object = nullptr;
		}
	}
}

//--------------------------------------------------------------------------
/**
* GetNumAllObjs
*/
int DebugRenderSystem::GetNumAllObjs() const
{
	int count = 0;
	for( int typeIndex = 0; typeIndex < NUM_DEBUG_RENDER_MODES; ++typeIndex  )
	{
		for( int objIndex = 0; objIndex < (int) m_renderWorldObjects[typeIndex].size(); ++objIndex )
		{
			DebugRenderObject* object = m_renderWorldObjects[typeIndex][objIndex];
			if( object )
			{
				++count;
			}
		}
	}

	for( int objIndex = 0; objIndex < (int) m_renderScreenObjects.size(); ++objIndex )
	{
		DebugRenderObject* object = m_renderScreenObjects[objIndex];
		if( object )
		{
			++count;
		}
	}

	for( int objIndex = 0; objIndex < (int) m_renderScreenDebugMessages.size(); ++objIndex )
	{
		DebugRenderObjectText2D* object = m_renderScreenDebugMessages[objIndex];
		if( object )
		{
			++count;
		}
	}
	return count;
}

//--------------------------------------------------------------------------
/**
* Clear
*/
void DebugRenderSystem::Clear()
{
	for( int typeIndex = 0; typeIndex < NUM_DEBUG_RENDER_MODES; ++typeIndex  )
	{
		for( int objIndex = 0; objIndex < (int) m_renderWorldObjects[typeIndex].size(); ++objIndex )
		{
			DebugRenderObject*& object = m_renderWorldObjects[typeIndex][objIndex];
			if( object )
			{
				delete object;
				object = nullptr;
			}
		}
	}

	for( int objIndex = 0; objIndex < (int) m_renderScreenObjects.size(); ++objIndex )
	{
		DebugRenderObject*& object = m_renderScreenObjects[objIndex];
		if( object )
		{
			delete object;
			object = nullptr;
		}
	}

	for( int objIndex = 0; objIndex < (int) m_renderScreenDebugMessages.size(); ++objIndex )
	{
		DebugRenderObjectText2D*& object = m_renderScreenDebugMessages[objIndex];
		if( object )
		{
			delete object;
			object = nullptr;
		}
	}
	SAFE_DELETE(m_wireframeSphere);
}

//--------------------------------------------------------------------------
/**
* GetRenderContext
*/
RenderContext* DebugRenderSystem::GetRenderContext() const
{
	return m_renderContext; 
}

//--------------------------------------------------------------------------
/**
* GetFont
*/
BitmapFont* DebugRenderSystem::GetFont() const
{
	return m_renderContext->CreateOrGetBitmapFromFile( m_fontName.c_str() );
}

//--------------------------------------------------------------------------
/**
* GetDebugTextHeight
*/
float DebugRenderSystem::GetDebugTextHeight() const
{
	return m_screenHeight * 0.02f;
}

//--------------------------------------------------------------------------
/**
* GetScreenHeight
*/
float DebugRenderSystem::GetScreenHeight() const
{
	return m_screenHeight;
}


//--------------------------------------------------------------------------
/**
* GetScreenWidth
*/
float DebugRenderSystem::GetScreenWidth() const
{
	return m_screenWidth;
}

//--------------------------------------------------------------------------
/**
* GetScreen
*/
AABB2 DebugRenderSystem::GetScreen() const
{
	return AABB2( m_screenWidth, m_screenHeight );
}

//--------------------------------------------------------------------------
/**
* AddWorldObj
*/
void DebugRenderSystem::AddWorldObj( DebugRenderObject* obj )
{
	eDebugRenderMode typeOfObj = obj->m_renderMode;
	std::vector<DebugRenderObject*>& objList = m_renderWorldObjects[typeOfObj];
	for( int objIndex = 0; objIndex < (int) objList.size(); ++objIndex )
	{
		DebugRenderObject*& curObj = objList[objIndex];
		if( !curObj )
		{
			curObj = obj;
			return;
		}
	}
	objList.push_back( obj );
}

//--------------------------------------------------------------------------
/**
* AddScreenObj
*/
void DebugRenderSystem::AddScreenObj( DebugRenderObject* obj )
{
	for( int objIndex = 0; objIndex < (int) m_renderScreenObjects.size(); ++objIndex )
	{
		DebugRenderObject*& curObj = m_renderScreenObjects[objIndex];
		if( !curObj )
		{
			curObj = obj;
			return;
		}
	}
	m_renderScreenObjects.push_back( obj );
}

//--------------------------------------------------------------------------
/**
* AddDebugMessage
*/
void DebugRenderSystem::AddDebugMessage( DebugRenderObjectText2D* obj )
{
	for( int objIndex = 0; objIndex < (int) m_renderScreenDebugMessages.size(); ++objIndex )
	{
		DebugRenderObjectText2D*& curObj = m_renderScreenDebugMessages[objIndex];
		if( !curObj )
		{
			curObj = obj;
			return;
		}
	}
	m_renderScreenDebugMessages.push_back( obj );
}

//--------------------------------------------------------------------------
/**
* CreateUVSphere
*/
MeshCPU* DebugRenderSystem::CreateUVSphere( float radius )
{
	MeshCPU* mesh = new MeshCPU( m_wireframeSphere );
	mesh->UniformScale( radius );
	return mesh;
}

//--------------------------------------------------------------------------
/**
* Command_Clear
*/
bool DebugRenderSystem::Command_Clear( EventArgs& args )
{
	UNUSED(args);
	g_theDebugRenderSystem->Clear();
	return false;
}

//--------------------------------------------------------------------------
/**
* Command_Toggle
*/
bool DebugRenderSystem::Command_Toggle( EventArgs& args )
{
	UNUSED(args);
	g_theDebugRenderSystem->m_open = !g_theDebugRenderSystem->m_open;
	return false;
}

//--------------------------------------------------------------------------
/**
* Command_Open
*/
bool DebugRenderSystem::Command_Open( EventArgs& args )
{
	UNUSED(args);
	g_theDebugRenderSystem->m_open = true;
	return false;
}

//--------------------------------------------------------------------------
/**
* Command_Close
*/
bool DebugRenderSystem::Command_Close( EventArgs& args )
{
	UNUSED(args);
	g_theDebugRenderSystem->m_open = false;
	return false;
}

//--------------------------------------------------------------------------
/**
* DebugRenderPoint
*/
void DebugRenderPoint( float duration, eDebugRenderMode mode, Vec3 pos, Rgba startColor, Rgba endColor, float size /*= 0.1f */ )
{
	AABB2 quad( 1.0f, 1.0f );
	quad.ScaleUniform( size );

	DebugRenderQuad( duration, mode, pos, quad, startColor, endColor );
}

//--------------------------------------------------------------------------
/**
* DebugRenderQuad
*/
void DebugRenderQuad( float duration, eDebugRenderMode mode, Vec3 pos, AABB2 const &quad, Rgba startColor, Rgba endColor, TextureView *view /*= nullptr */ )
{
	MeshCPU* meshCPU = new MeshCPU();
	CPUMeshAddPlain( meshCPU, quad );

	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, mode, pos, view, startColor, endColor, duration );
	g_theDebugRenderSystem->AddWorldObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderBox
*/
void DebugRenderBox( float duration, eDebugRenderMode mode, Vec3 pos, AABB3 const &box, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, TextureView *view /*= nullptr */ )
{
	MeshCPU* meshCPU = new MeshCPU();
	CPUMeshAddCube( meshCPU, box );

	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, mode, pos, view, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddWorldObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderFrustum
*/
void DebugRenderFrustum( float duration, eDebugRenderMode mode, Vec3 pos, Frustum const &frustum, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, TextureView *view /*= nullptr */ )
{
	MeshCPU* meshCPU = new MeshCPU();
	CPUMeshAddFrustum( meshCPU, frustum );

	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, mode, pos, view, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddWorldObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderLine
*/
void DebugRenderLine( float duration, eDebugRenderMode mode, Vec3 p0, Vec3 p1, Rgba startColor, Rgba endColor, float width /*= 0.1f */ )
{
	MeshCPU* meshCPU = new MeshCPU();
	Vec3 disp = p1 - p0;
	CPUMeshAddCylinder( meshCPU, Vec3::ZERO, disp, width, width, 8 );

	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, mode, p0, nullptr, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddWorldObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderArrow
*/
void DebugRenderArrow( float duration, eDebugRenderMode mode, Vec3 p0, Vec3 p1, Rgba startColor, Rgba endColor, float width /*= 0.1f*/, float arrowWidth /*= 0.5f */ )
{
	MeshCPU* meshCPU = new MeshCPU();

	Vec3 disp = p1 - p0;
	Vec3 shaftDisp = disp;
	shaftDisp.SetLength( disp.GetLength() - arrowWidth * 2.0f );

	CPUMeshAddCylinder( meshCPU, Vec3::ZERO, shaftDisp, width, width, 16 );
	CPUMeshAddCylinder( meshCPU, shaftDisp, disp, arrowWidth, 0.0f, 16 );

	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, mode, p0, nullptr, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddWorldObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderBasis
*/
void DebugRenderBasis( float duration, eDebugRenderMode mode, Vec3 pos, Vec3 forward, Vec3 up, Vec3 right, float scale/*= 1.0f*/ )
{
	forward.SetLength( scale );
	up.SetLength( scale );
	right.SetLength( scale );

	DebugRenderArrow( duration, mode, pos, pos + right, Rgba::RED, Rgba::RED );
	DebugRenderArrow( duration, mode, pos, pos + forward, Rgba::BLUE, Rgba::BLUE );
	DebugRenderArrow( duration, mode, pos, pos + up, Rgba::GREEN, Rgba::GREEN );
	DebugRenderSphere( duration, mode, pos, scale * 0.1f );
}

//--------------------------------------------------------------------------
/**
* DebugRenderWireSphere
*/
void DebugRenderWireSphere( float duration, Vec3 pos, float radius, Rgba startColor, Rgba endColor, TextureView *view /*= nullptr */ )
{
	DebugRenderSphere( duration, DEBUG_RENDER_WIREFRAME, pos, radius, startColor, endColor, view );
}



//--------------------------------------------------------------------------
/**
* DebugRenderSphere
*/
void DebugRenderSphere( float duration, eDebugRenderMode mode, Vec3 pos, float radius, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, TextureView *view /*= nullptr */ )
{
	MeshCPU* meshCPU = g_theDebugRenderSystem->CreateUVSphere( radius );
	CPUMeshAddUVSphere( meshCPU, Vec3::ZERO, radius );

	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, mode, pos, view, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddWorldObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderWireCapsule
*/
void DebugRenderWireCapsule( float duration, const Vec3& startPos, const Vec3& endPos, float radius, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, TextureView *view /*= nullptr */ )
{
	DebugRenderCapsule( duration, eDebugRenderMode::DEBUG_RENDER_WIREFRAME, startPos, endPos, radius, startColor, endColor, view );
}

//--------------------------------------------------------------------------
/**
* DebugRenderWireCapsule
*/
void DebugRenderWireCapsule(float duration, const Capsule3& capsule, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, TextureView *view /*= nullptr */)
{
	DebugRenderWireCapsule( duration, capsule.m_start, capsule.m_end, capsule.m_radius, startColor, endColor, view );
}

//--------------------------------------------------------------------------
/**
* DebugRenderCapsule
*/
void DebugRenderCapsule( float duration, eDebugRenderMode mode, const Vec3& startPos, const Vec3& endPos, float radius, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, TextureView *view /*= nullptr */ )
{
	MeshCPU* meshCPU = new MeshCPU();
	CPUMeshAddCapsule( meshCPU, Vec3::ZERO, endPos - startPos, radius, 8, 8 );

	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, mode, startPos, view, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddWorldObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderTextf
*/
void DebugRenderTextf( float duration, eDebugRenderMode mode, Vec3 pos, Vec2 alignment, float height, Rgba startColor, Rgba endColor, char const *format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string stringToAdd( textLiteral );

	
	DebugRenderObject* debugObj = new DebugRenderObjectText3D( stringToAdd, mode, pos, alignment, Vec3::ZERO, height, startColor, endColor, duration );
	g_theDebugRenderSystem->AddWorldObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderGlyphf
*/
void DebugRenderWorldTextf( float duration, eDebugRenderMode mode, Vec3 pos, Vec2 alignment, Vec3 eulerRot, float height, Rgba startColor, Rgba endColor, char const *format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string stringToAdd( textLiteral );


	DebugRenderObject* debugObj = new DebugRenderObjectText3D( stringToAdd, mode, pos, alignment, eulerRot, height, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddWorldObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderScreenPoint
*/
void DebugRenderScreenPoint( float duration, Vec2 pos, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, float size /*= g_theDebugRenderSystem->GetDebugTextHeight() */ )
{
	AABB2 quad( 1.0f, 1.0f );
	quad.ScaleUniform( size );

	DebugRenderScreenQuad( duration, quad, pos, startColor, endColor, nullptr );
}

//--------------------------------------------------------------------------
/**
* DebugRenderScreenQuad
*/
void DebugRenderScreenQuad( float duration, AABB2 const &quad, Vec2 pos, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, TextureView *view /*= nullptr */ )
{
	MeshCPU* meshCPU = new MeshCPU();
	CPUMeshAddPlain( meshCPU, quad );

	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, DEBUG_RENDER_ALWAYS, Vec3( pos.x, pos.y, 0.0f ), view, startColor, endColor, duration );
	g_theDebugRenderSystem->AddScreenObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderDisc
*/
void DebugRenderDisc( float duration, Vec2 pos, float radius, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, TextureView *view /*= nullptr */ )
{
	MeshCPU* meshCPU = g_theDebugRenderSystem->CreateUVSphere( radius );
	meshCPU->Flatten();
	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, DEBUG_RENDER_ALWAYS, Vec3( pos.x, pos.y, 0.0f ), view, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddScreenObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderScreenArrow
*/
void DebugRenderScreenArrow( float duration, Vec3 p0, Vec3 p1, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, float width /*= 0.1f*/, float arrowWidth /*= 0.3f */ )
{
	if( p0 == p1 )
	{
		return;
	}
	MeshCPU* meshCPU = new MeshCPU();
	Vec3 disp = p1 - p0;
	Vec3 shaftDisp = disp;
	shaftDisp.SetLength( disp.GetLength() - arrowWidth * 2.0f );

	CPUMeshAddCylinder( meshCPU, Vec3::ZERO, shaftDisp, width, width, 4 );
	CPUMeshAddCone( meshCPU, shaftDisp, disp, arrowWidth, 5 );
	meshCPU->Flatten();

	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, DEBUG_RENDER_ALWAYS, Vec3( p0.x, p0.y, 0.0f ), nullptr, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddScreenObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderScreenBasis
*/
void DebugRenderScreenBasis( float duration, Vec2 pos, Vec3 forward, Vec3 up, Vec3 right, float scale /*= 1.0f */ )
{
	forward.SetLength( scale );
	up.SetLength( scale );
	right.SetLength( scale );
	Vec3 newPos( pos.x, pos.y, 0.0f );

	DebugRenderScreenArrow( duration, newPos, newPos + right, Rgba::RED, Rgba::RED );
	DebugRenderScreenArrow( duration, newPos, newPos + forward, Rgba::BLUE, Rgba::BLUE );
	DebugRenderScreenArrow( duration, newPos, newPos + up, Rgba::GREEN, Rgba::GREEN );
	DebugRenderDisc( duration,  pos, scale * 0.1f );
}

//--------------------------------------------------------------------------
/**
* DebugRenderScreenLine
*/
void DebugRenderScreenLine( float duration, Vec2 p0, Vec2 p1, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, float width /*= 0.1f */ )
{
	MeshCPU* meshCPU = new MeshCPU();
	Vec2 disp = p1 - p0;
	CPUMeshAddLine2D( meshCPU, Vec2::ZERO, disp, width );	
	
	DebugRenderObjectMesh* debugObj = new DebugRenderObjectMesh( meshCPU, DEBUG_RENDER_ALWAYS, Vec3( p0.x, p0.y, 0.0f ), nullptr, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddScreenObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderScreenTextf
*/
void DebugRenderScreenTextf( float duration, Vec2 pivot, float height, Rgba startColor /*= Rgba::WHITE*/, Rgba endColor /*= Rgba::WHITE*/, char const *format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string stringToAdd( textLiteral );

	DebugRenderObjectText2D* debugObj = new DebugRenderObjectText2D( stringToAdd, DEBUG_RENDER_ALWAYS, Vec3::ZERO, pivot, Vec3::ZERO, height, startColor, endColor, duration, false );
	g_theDebugRenderSystem->AddScreenObj( debugObj );
}

//--------------------------------------------------------------------------
/**
* DebugRenderMessage
*/
void DebugRenderMessage( float duration, Rgba startColor, Rgba endColor, char const *format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string stringToAdd( textLiteral );


	DebugRenderObjectText2D* debugObj = new DebugRenderObjectText2D( stringToAdd, DEBUG_RENDER_ALWAYS, Vec3::ZERO, Vec2::ALIGN_BOTTOM_LEFT,  Vec3::ZERO, g_theDebugRenderSystem->GetDebugTextHeight(), startColor, endColor, duration );
	g_theDebugRenderSystem->AddDebugMessage( debugObj );
}
