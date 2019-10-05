#pragma once

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif

#ifdef PLATFORM_WINDOWS

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Debug/TextLine.hpp"

class RenderContext;
class Camera;
struct TextLine;

enum eDevConsoleState
{
	UNKNOWN_DEVCONSOLE_STATE = -1,
	DEVCON_CLOSED,
	DEVCON_FADE_AWAY,
	DEVCON_FULLY_OPEN,
	NUM_DEVCONSOLE_STATES
};

class DevConsole
{
public:
	const static Rgba CONSOLE_INFO;
	const static Rgba CONSOLE_ERROR;
	const static Rgba CONSOLE_ERROR_DESC;
	const static Rgba CONSOLE_ECHO;
	const static Rgba CONSOLE_WARNING;

public:
	explicit DevConsole( const std::string& nameOfBitmapFont, const Rgba& backgroundColor = Rgba::FADED_BLACK, float fadeOutTime = 3.0f, float textStartingFadeAmount = 0.8f );
	~DevConsole();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void Update();
	void Render( RenderContext* renderContext, Camera& camera, float textHeight ) const;
	void EndFrame();

	void PrintString( const std::string& toPrint, const Rgba& color = Rgba::WHITE );

	void SetState( eDevConsoleState newState );
	eDevConsoleState GetState();

	void ToggleOpenFull();
	void ToggleVerbosePrinting();
	bool IsOpen() const;

	void SetTextAspectRatio( float aspectRatio );

public:
	bool HandleKeyPress( unsigned char keyCode );
	bool HandleCharPress( unsigned char keyCode );
	bool HandleESCPress();
	bool HandleKeyReleased( unsigned char keyCode );

private:
	void AddCharToLine( unsigned char keyCode );
	void DeletePressed();
	void CtrlDeletePressed();
	void BackspacePressed();
	void CtrlBackspacePressed();
	void HomePressed();
	void EndPressed();
	void EnterPressed();
	void ControlArrowPressed( unsigned char keyCode );
	void ShiftArrowPressed( unsigned char keyCode );
	void ArrowPressed( unsigned char keyCode );

	void ResetInputLine();
	void SetInputLineByHistoryIndex();
	void CopyStringIntoHistory( std::string line );
	void Clear();

	void BeginSelecting();
	void StopSelecting();
	void DeleteSelected();

	void MoveCarotRight();
	void MoveCarotLeft();
	void JumpCarotRight();
	void JumpCarotLeft();

public:
	static bool Command_Test( EventArgs& args );
	static bool Command_Help( EventArgs& args );
	static bool Command_Exec( EventArgs& args );
	static bool Command_ClearConsole( EventArgs& args );
	static bool Command_ClearConsoleHistory( EventArgs& args );
	static bool Command_LogCurrentMemoryAllocs( EventArgs& args );

private:
	void OpenRender( RenderContext* renderContext, const Camera& camera, float textHeight ) const;
	void FadeRender( RenderContext* renderContext, const Camera& camera, float textHeight ) const;

private:
	void RenderBackground( RenderContext* renderContext, const Camera& camera ) const;
	void ExecuteCommandLine( const std::string& commandLine );
	void AddLineAtEnd( const std::string& toPrint, const Rgba& color );

private:
	std::string GetStateToString( eDevConsoleState state );

private:
	std::vector< TextLine* >	m_lines;
	std::vector< std::string >	m_inputHistory;
	int							m_historyIndex = -1;
	TextLine					m_inputCommand;
	std::string					m_bitmapName;
	Rgba						m_backgroundColor;
	eDevConsoleState			m_state = DEVCON_CLOSED;
	float						m_fadeOutTime;
	float						m_textStartingFadeAmount;
	int							m_currentFrame = 0;
	float						m_currentTime = 0.0f;
	float						m_timeFromLastMovedCarot = 0.0f;
	bool						m_printVerbose = false;
	int							m_carotIndex = 0;
	float						m_textAspectRatio = .7f;

private:
	bool						m_selecting = false;
	int							m_beingSelectingIndex = -1;
};

#endif