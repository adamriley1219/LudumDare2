#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Debug/DevConsole.hpp"
#include "Engine/Core/Strings/NamedStrings.hpp"
#include "Engine/Core/XML/XMLUtils.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/ImGUI/ImGUISystem.hpp"

#include "Game/GameCommon.hpp"
#include "Game/App.hpp"



//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move these constants to GameCommon.hpp or elsewhere
// 
constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area


									  //-----------------------------------------------------------------------------------------------
									  // #SD1ToDo: Move each of these items to its proper place, once that place is established
									  // 
void* g_hWnd = nullptr;							// ...becomes WindowContext::m_windowHandle
HDC g_displayDeviceContext = nullptr;			// ...becomes WindowContext::m_displayContext
HGLRC g_openGLRenderingContext = nullptr;		// ...becomes RenderContext::m_apiRenderingContext
const char* APP_NAME = "LudumDare2";					// ...becomes ???

														//-----------------------------------------------------------------------------------------------
														// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
														// This function is called by Windows whenever we ask it for notifications
														//
														// #SD1ToDo: We will move this function to a more appropriate place later on...
														//
static bool AppWindowProc( void* windowHandle, uint32_t wmMessageCode, uintptr_t wParam, uintptr_t lParam )
{
	if( ImGUI_ProcessInputWindows( windowHandle, wmMessageCode, wParam, lParam ) )
	{
		// ImGUI Consumed it
		return true;
	}

	switch( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:		
	{
		//g_isQuitting = true;
		g_theApp->HandleQuitRequested();
		return true; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		unsigned char asKey = (unsigned char) wParam;
		g_theApp->HandleKeyPressed(asKey);

		if( asKey == VK_ESCAPE )
		{
			if( !g_theConsole->HandleESCPress() )
			{
				g_theEventSystem->FireEvent( "quit" );
			}
			//g_theApp->HandleQuitRequested();
			return true; 
		}

		return true;
		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		unsigned char asKey = (unsigned char) wParam;
		if(g_theApp->HandleKeyReleased(asKey))
		{
			return true; 
		}

		break;
	}
	case WM_CHAR:
	{
		unsigned char asKey = (unsigned char) wParam;
		if(g_theApp->HandleCharPressed(asKey))
		{
			return true;
		}

		break;
	}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return false; 
}

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
void CreateWindowAndRenderContext( float clientAspect )
{
	g_theWindowContext = new WindowContext();
	g_theWindowContext->Create( APP_NAME, clientAspect, .90f, AppWindowProc ); 

	// this is the end of the windows part
	// 	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	// 	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
	// 	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
	// 	pixelFormatDescriptor.nVersion = 1;
	// 	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	// 	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	// 	pixelFormatDescriptor.cColorBits = 24;
	// 	pixelFormatDescriptor.cDepthBits = 24;
	// 	pixelFormatDescriptor.cAccumBits = 0;
	// 	pixelFormatDescriptor.cStencilBits = 8;
	// 
	// 	int pixelFormatCode = ChoosePixelFormat( g_displayDeviceContext, &pixelFormatDescriptor );
	// 	SetPixelFormat( g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
	// 	g_openGLRenderingContext = wglCreateContext( g_displayDeviceContext );
	// 	wglMakeCurrent( g_displayDeviceContext, g_openGLRenderingContext );


}


//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
void RunMessagePump()
{
	g_theWindowContext->BeginFrame(); 
}

//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
//
void RunFrame()
{
	RunMessagePump();

	g_theApp->RunFrame();	

}
//-----------------------------------------------------------------------------------------------
void Startup()
{
	tinyxml2::XMLDocument config;
	config.LoadFile("Data/GameConfig.xml");
	XmlElement* root = config.RootElement();
	g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*root);
	CreateWindowAndRenderContext( CLIENT_ASPECT );
	g_theApp = new App();
	g_theApp->Startup();
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	g_theApp->Shutdown();

	// Destroy the global App instance
	delete g_theApp;		
	g_theApp = nullptr;

	delete g_theWindowContext; 
	g_theWindowContext = nullptr; 

}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( applicationInstanceHandle ); 
	UNUSED( commandLineString );

	Startup();


	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting()) 
	{
		RunFrame();
		//SwapBuffers( g_displayDeviceContext );
		Sleep(0);
	}

	Shutdown();
	return 0;
}
