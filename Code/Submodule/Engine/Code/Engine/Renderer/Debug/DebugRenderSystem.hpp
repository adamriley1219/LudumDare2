#pragma once
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"
#include <vector>


//--------------------------------------------------------------------------
class DebugRenderObject;
class DebugRenderObjectText2D;
class RenderContext;
class TextureView;
class Shader;
class MeshCPU;
class BitmapFont;
class Camera;
struct Capsule3;
//--------------------------------------------------------------------------


enum eDebugRenderMode 
{
	DEBUG_RENDER_USE_DEPTH, 
	DEBUG_RENDER_ALWAYS, 
	DEBUG_RENDER_XRAY, 
	DEBUG_RENDER_WIREFRAME,
	NUM_DEBUG_RENDER_MODES
};



class DebugRenderSystem
{
public:
	explicit DebugRenderSystem( RenderContext* ctx, float screenHeight, float screenWidth, const std::string& nameOfBitmapFont );
	~DebugRenderSystem();

public:
	void Startup();
	void Shutdown();

	void BeginFrame();
	void Update() const;
	void RenderToCamera( Camera* camera ) const;
	void RenderToScreen() const;
	void EndFrame();

	void GarbageCollection();
	int GetNumAllObjs() const;
public:
	void Clear();

public:
	RenderContext* GetRenderContext() const;
	BitmapFont* GetFont() const;
	float GetDebugTextHeight() const;
	float GetScreenHeight() const;
	float GetScreenWidth() const;
	AABB2 GetScreen() const;

public:
	void AddWorldObj( DebugRenderObject* obj );
	void AddScreenObj( DebugRenderObject* obj );
	void AddDebugMessage( DebugRenderObjectText2D* obj );
	MeshCPU* CreateUVSphere( float radius );

public:
	static bool Command_Clear( EventArgs& args );
	static bool Command_Toggle( EventArgs& args );
	static bool Command_Open( EventArgs& args );
	static bool Command_Close( EventArgs& args );

private:
	bool m_open = false;

private:
	std::vector<DebugRenderObject*>			m_renderWorldObjects[NUM_DEBUG_RENDER_MODES];
	std::vector<DebugRenderObject*>			m_renderScreenObjects;
	std::vector<DebugRenderObjectText2D*>	m_renderScreenDebugMessages;
	RenderContext*							m_renderContext;
	float									m_screenHeight;
	float									m_screenWidth;
	MeshCPU*								m_wireframeSphere;

private:
	Shader*								m_alwaysShader;
	Shader*								m_greaterShader;
	Shader*								m_lequalShader;
	Shader*								m_wireShader;
	std::string							m_fontName;
};


//--------------------------------------------------------------------------
// 3D
//--------------------------------------------------------------------------
void DebugRenderPoint( float duration, eDebugRenderMode mode, 
	Vec3 pos, 
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	float size = 0.1f ); 

void DebugRenderQuad( float duration, eDebugRenderMode mode, 
	Vec3 pos, 
	AABB2 const &quad, 
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	TextureView *view = nullptr ); 

void DebugRenderBox(float duration, eDebugRenderMode mode, 
	Vec3 pos, 
	AABB3 const &box, 
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	TextureView *view = nullptr );

void DebugRenderFrustum(float duration, eDebugRenderMode mode, 
	Vec3 pos, 
	Frustum const &frustum,
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	TextureView *view = nullptr );

void DebugRenderLine( float duration, eDebugRenderMode mode, 
	Vec3 p0, Vec3 p1, 
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE, 
	float width = 0.1f );

void DebugRenderArrow( float duration, eDebugRenderMode mode, 
	Vec3 p0, Vec3 p1, 
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE, 
	float width = 0.1f, float arrowWidth = 0.3f );

void DebugRenderBasis(float duration, eDebugRenderMode mode, 
	Vec3 pos, 
	Vec3 forward,
	Vec3 up,
	Vec3 right,
	float scale = 1.0f );

void DebugRenderWireSphere( float duration,
	Vec3 pos, 
	float radius, 
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	TextureView *view = nullptr ); 

void DebugRenderSphere( float duration, eDebugRenderMode mode, 
	Vec3 pos, 
	float radius, 
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	TextureView *view = nullptr ); 

void DebugRenderWireCapsule( float duration, 
	const Vec3& startPos, const Vec3& endPos, float radius,
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	TextureView *view = nullptr );

void DebugRenderWireCapsule( float duration, const Capsule3& capsule,
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	TextureView *view = nullptr );

void DebugRenderCapsule( float duration, eDebugRenderMode mode, 
	const Vec3& startPos, const Vec3& endPos, float radius,
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	TextureView *view = nullptr );

void DebugRenderTextf( float duration, eDebugRenderMode mode, 
	Vec3 pos,
	Vec2 alignment,
	float height,
	Rgba startColor, Rgba endColor, 
	char const *format, ... ); 

void DebugRenderWorldTextf( float duration, eDebugRenderMode mode, 
	Vec3 pos,
	Vec2 alignment,
	Vec3 eulerRot, 
	float height,
	Rgba startColor, Rgba endColor, 
	char const *format, ... ); 

//--------------------------------------------------------------------------
// 2D
//--------------------------------------------------------------------------

void DebugRenderScreenPoint( float duration, 
	Vec2 pos, 
	Rgba start_color = Rgba::WHITE, Rgba end_color = Rgba::WHITE,
	float size = g_theDebugRenderSystem->GetDebugTextHeight() ); 

void DebugRenderScreenQuad( float duration, 
	AABB2 const &quad, 
	Vec2 pos,
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	TextureView *view = nullptr ); 

void DebugRenderScreenLine( float duration,
	Vec2 p0, Vec2 p1, 
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE, 
	float width = 0.1f ); 

void DebugRenderScreenTextf( float duration,
	Vec2 pivot, 
	float height,
	Rgba startColor, Rgba endColor, 
	char const *format, ... ); 

void DebugRenderDisc( float duration, 
	Vec2 pos, 
	float radius, 
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE,
	TextureView *view = nullptr ); 

void DebugRenderScreenArrow( float duration,
	Vec3 p0, Vec3 p1, 
	Rgba startColor = Rgba::WHITE, Rgba endColor = Rgba::WHITE, 
	float width = 0.1f, float arrowWidth = 0.3f );

void DebugRenderScreenBasis(float duration,
	Vec2 pos, 
	Vec3 forward,
	Vec3 up,
	Vec3 right,
	float scale = 1.0f );
//--------------------------------------------------------------------------
// Message System (renders with screen items)
//--------------------------------------------------------------------------
void DebugRenderMessage( float duration,
	Rgba startColor, Rgba endColor, 
	char const *format, ... ); 