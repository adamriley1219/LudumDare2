#pragma once

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif // _WIN32

#ifdef PLATFORM_WINDOWS
#include "Engine/Core/Strings/StringUtils.hpp"

class RenderContext;

//--------------------------------------------------------------------------
class ImGUISystem
{
public:
	ImGUISystem( RenderContext* context );
	~ImGUISystem();

	void Startup();
	void Shutdown();

	void BeginFrame();
	void Render();
	void EndFrame();

	RenderContext* GetRenderContext() { return m_renderContext; };

private:
	RenderContext* m_renderContext = nullptr;

};


//**********************************************************************************************************
// MUST USE THESE TO BEGIN AN END WINDOWS
//**********************************************************************************************************

// Use to begin a window to add into
bool ImGUI_BeginWindow( const char* name, bool* p_open = (bool*)false, int flags = 0 );

// Use to end the window you are currently drawing in
void ImGUI_EndWindow();

//**********************************************************************************************************
// Draw into the window
//**********************************************************************************************************
bool ImGUI_Checkbox( const char* label, bool* set );
bool ImGUI_SliderFloat( const char* lable, float* init_setting, float min, float max, const char* format = "%.3f", float power = 1.0f );
bool ImGUI_ColorEdit3( const char* lable, float col[3], int flags = 0 );
void ImGUI_Text( const std::string& text );
//**********************************************************************************************************


//**********************************************************************************************************
//Process Input
//**********************************************************************************************************
bool ImGUI_ProcessInputWindows( void* windowHandle, uint32_t wmMessageCode, uintptr_t wParam, uintptr_t lParam );
//**********************************************************************************************************

#endif // PLATFORM_WINDOWS
