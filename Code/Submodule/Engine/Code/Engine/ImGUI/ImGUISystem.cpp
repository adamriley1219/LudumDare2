#include "Engine/ImGUI/ImGUISystem.hpp"

#ifdef PLATFORM_WINDOWS
#include "Engine/Core/WindowContext.hpp"

// Heavy Header, Only include in cpps
#include "ThirdParty/imGUI/imgui.h"
#include "ThirdParty/imGUI/imgui_impl_dx11.h"
#include "ThirdParty/imGUI/imgui_impl_win32.h"

#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Textures/TextureView.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3d11.h>  
#include <DXGI.h>    

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

//--------------------------------------------------------------------------
/**
* ImGUISystem
*/
ImGUISystem::ImGUISystem(RenderContext* context)
	: m_renderContext(context)
{

}

//--------------------------------------------------------------------------
/**
* ~ImGUISystem
*/
ImGUISystem::~ImGUISystem()
{

}

//--------------------------------------------------------------------------
/**
* Startup
*/
void ImGUISystem::Startup()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(m_renderContext->GetWindowContext()->m_hwnd);
	ImGui_ImplDX11_Init( m_renderContext->GetD3DDevice(), m_renderContext->GetD3DContext() );
	ImGui::StyleColorsDark();
	UNUSED(io);
}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void ImGUISystem::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//--------------------------------------------------------------------------
/**
* BeginFrame
*/
void ImGUISystem::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

//--------------------------------------------------------------------------
/**
* Render
*/
void ImGUISystem::Render()
{
	ImGui::Render();
	ImDrawData* data = ImGui::GetDrawData();

	//Set RTV from imGUI
	ID3D11RenderTargetView* dx_rtv = m_renderContext->GetColorTargetView()->m_rtv;
	ID3D11DeviceContext* context = m_renderContext->GetD3DContext();
	context->OMSetRenderTargets( 1, &dx_rtv, NULL );

	ImGui_ImplDX11_RenderDrawData( data );
}

//--------------------------------------------------------------------------
/**
* EndFrame
*/
void ImGUISystem::EndFrame()
{

}

//--------------------------------------------------------------------------
/**
* ImGUI_BeginWindow
*/
bool ImGUI_BeginWindow( const char* name, bool* p_open, int flags )
{
	return ImGui::Begin( name, p_open, flags );
}

//--------------------------------------------------------------------------
/**
* ImGUI_EndWindow
*/
void ImGUI_EndWindow()
{
	ImGui::End();
}

//--------------------------------------------------------------------------
/**
* ImGUI_Checkbox
*/
bool ImGUI_Checkbox(const char* label, bool* set)
{
	return ImGui::Checkbox( label, set );
}

//--------------------------------------------------------------------------
/**
* SliderFloat
*/
bool ImGUI_SliderFloat(const char* lable, float* init_setting, float min, float max, const char* format /*= "%.3f"*/, float power /*= 1.0f */)
{
	return ImGui::SliderFloat( lable, init_setting, min, max, format, power );
}

//--------------------------------------------------------------------------
/**
* ImGUI_ColorEdit3
*/
bool ImGUI_ColorEdit3(const char* lable, float col[3], int flags /*= 0 */)
{
	return ImGui::ColorEdit3(lable, col, flags);
}

//--------------------------------------------------------------------------
/**
* ImGUI_Text
*/
void ImGUI_Text( const std::string& text )
{
	ImGui::Text( "%s", text.c_str() );
}


//--------------------------------------------------------------------------
/**
* ImGUI_ProcessInputWindows
*/
bool ImGUI_ProcessInputWindows(void* windowHandle, uint32_t wmMessageCode, uintptr_t wParam, uintptr_t lParam)
{
	return ImGui_ImplWin32_WndProcHandler(windowHandle, wmMessageCode, wParam, lParam);
}

#endif


