#include "Engine/Core/Debug/DevConsole.hpp"

#ifdef PLATFORM_WINDOWS

#include "Engine/Core/Debug/TextLine.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Core/Memory/MemoryManagement.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Shaders/Shader.hpp"


const Rgba DevConsole::CONSOLE_INFO			= Rgba( 1.0f, 1.0f, 1.0f );
const Rgba DevConsole::CONSOLE_ERROR		= Rgba( 1.0f, 0.0f, 0.0f );
const Rgba DevConsole::CONSOLE_ERROR_DESC	= Rgba( 0.7f, 0.0f, 0.0f );
const Rgba DevConsole::CONSOLE_ECHO			= Rgba( 0.3f, 1.0f, 0.3f );
const Rgba DevConsole::CONSOLE_WARNING		= Rgba( 1.0f, 1.0f, 0.0f );

//--------------------------------------------------------------------------
/**
* DevConsole
*/
DevConsole::DevConsole( const std::string& nameOfBitmapFont, const Rgba& backgroundColor /*= Rgba::FADED_BLACK*/, float fadeOutTime /* = 3.0f */, float textStartingFadeAmount /* = 0.8f */ )
	: m_bitmapName(nameOfBitmapFont)
	, m_backgroundColor(backgroundColor)
	, m_fadeOutTime(fadeOutTime)
	, m_textStartingFadeAmount(textStartingFadeAmount)
	, m_inputCommand( "", CONSOLE_INFO, 0.0f, 0 )
{
}

//--------------------------------------------------------------------------
/**
* ~DevConsole
*/
DevConsole::~DevConsole()
{
	for( int lineIndex = 0; lineIndex < (int) m_lines.size(); lineIndex++ )
	{
		TextLine* line = m_lines[lineIndex];
		if( line )
		{
			delete line;
			line = nullptr;
		}
	}
}

//--------------------------------------------------------------------------
/**
* Startup
*/
void DevConsole::Startup()
{
	g_theEventSystem->SubscribeEventCallbackFunction( "test", Command_Test );
	g_theEventSystem->SubscribeEventCallbackFunction( "help", Command_Help );
	g_theEventSystem->SubscribeEventCallbackFunction( "exec", Command_Exec );
	g_theEventSystem->SubscribeEventCallbackFunction( "clear", Command_ClearConsole );
	g_theEventSystem->SubscribeEventCallbackFunction( "clearHist", Command_ClearConsoleHistory );
	g_theEventSystem->SubscribeEventCallbackFunction( "logMem", Command_LogCurrentMemoryAllocs );
	PrintString("*** WELCOME TO THE DEV CONSOLE ***", CONSOLE_INFO );
	PrintString("FOR HELP TYPE: help", CONSOLE_INFO );
}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void DevConsole::Shutdown()
{

}

//--------------------------------------------------------------------------
/**
* BeginFrame
*/
void DevConsole::BeginFrame()
{
	++ m_currentFrame;
}

//--------------------------------------------------------------------------
/**
* Update
*/
void DevConsole::Update()
{
	float time = (float) Clock::Master.GetTotalTime();
	m_timeFromLastMovedCarot += time - m_currentTime; 
	m_currentTime = time;
	for( int lineIndex = 0; lineIndex < (int) m_lines.size(); lineIndex++ )
	{
		TextLine* line = m_lines[lineIndex];
		float curTimeAlive = time - line->m_timeStamp;
		if( curTimeAlive > m_fadeOutTime )
		{
			line->m_color.a = 0.0f;
		}
		else
		{
			float alpha = 1.0f - curTimeAlive / m_fadeOutTime;
			if( alpha > m_textStartingFadeAmount )
			{
				alpha = m_textStartingFadeAmount;
			}
			line->m_color.a = alpha;
		}
	}
}

//--------------------------------------------------------------------------
/**
* PushTransformsToGame
*/
void DevConsole::Render( RenderContext* renderContext, Camera& camera, float textHeight ) const
{
	camera.SetColorTargetView( renderContext->GetColorTargetView() );
	camera.SetDepthTargetView( renderContext->GetDepthTargetView() );
	renderContext->BeginCamera( &camera );


	switch( m_state )
	{
		case UNKNOWN_DEVCONSOLE_STATE:
		{
			ERROR_AND_DIE( "DevConsole::PushTransformsToGame is reporting and unknown state." );
			break;
		}
		case DEVCON_CLOSED:
		{
			// Don't Draw if closed.
			break;
		}
		case DEVCON_FADE_AWAY:
		{
			FadeRender( renderContext, camera, textHeight );
			break;
		}
		case DEVCON_FULLY_OPEN:
		{
			OpenRender( renderContext, camera, textHeight );
			break;
		}
		default:
		{
			ERROR_AND_DIE( "DevConsole::Render is reporting and unknown state." );
			break;
		}
	}
	renderContext->EndCamera();
}

//--------------------------------------------------------------------------
/**
* EndFrame
*/
void DevConsole::EndFrame()
{

}

//--------------------------------------------------------------------------
/**
* PrintString
*/
void DevConsole::PrintString( const std::string& toPrint, const Rgba& color /*= Rgba::WHITE*/ )
{
	m_lines.push_back( new TextLine( toPrint, color, m_currentTime, m_currentFrame ) );
}

//--------------------------------------------------------------------------
/**
* SetState
*/
void DevConsole::SetState( eDevConsoleState newState )
{
	if( newState != m_state )
	{
		ResetInputLine();
		m_state = newState;
		g_theConsole->PrintString( Stringf( "Set DevConsole State To: %s", GetStateToString( newState ).c_str() ), CONSOLE_ECHO );
	}

}

//--------------------------------------------------------------------------
/**
* GetState
*/
eDevConsoleState DevConsole::GetState()
{
	return m_state;
}

//--------------------------------------------------------------------------
/**
* ToggleOpenFull
*/
void DevConsole::ToggleOpenFull()
{
	if( m_state == DEVCON_FULLY_OPEN )
	{
		m_state = DEVCON_CLOSED;
	}
	else
	{
		m_state = DEVCON_FULLY_OPEN;
	}
}

//--------------------------------------------------------------------------
/**
* ToggleVerbosePrinting
*/
void DevConsole::ToggleVerbosePrinting()
{
	m_printVerbose = !m_printVerbose;
}

//--------------------------------------------------------------------------
/**
* IsOpen
*/
bool DevConsole::IsOpen() const
{
	return m_state != DEVCON_CLOSED;
}

//--------------------------------------------------------------------------
/**
* SetTextAspectRatio
*/
void DevConsole::SetTextAspectRatio( float aspectRatio )
{
	m_textAspectRatio = aspectRatio;
}

//--------------------------------------------------------------------------
/**
* HandleKeyPress
*/
bool DevConsole::HandleKeyPress( unsigned char keyCode )
{
	if( m_state == DEVCON_FULLY_OPEN ) 
	{
		switch( keyCode )
		{
		case 13: // 'Enter' pressed
			EnterPressed();
			break;
		case 46: // 'DELETE' pressed
			if( InputSystem::IsCtrlPressed() )
			{
				CtrlDeletePressed();
			}
			else
			{
				DeletePressed();
			}
			break;
		case 35: // 'END' pressed
			EndPressed();
			break;
		case 36: // 'HOME' pressed
			HomePressed();
			break;
		case 37: // 'Left ARROW' pressed
		case 38: // 'UP ARROW' pressed
		case 39: // 'RIGHT ARROW' pressed
		case 40: // 'DOWN ARROW' pressed
			if( InputSystem::IsShiftPressed() )
			{
				ShiftArrowPressed( keyCode );
			}
			else if( InputSystem::IsCtrlPressed() )
			{
				ControlArrowPressed( keyCode );
			}
			else
			{
				ArrowPressed( keyCode );
			}
			break;
		default:
			break;
		}
		m_timeFromLastMovedCarot = 0;
		return true; 
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* HandleCharPress
*/
bool DevConsole::HandleCharPress( unsigned char keyCode )
{
	switch( keyCode )
	{
	case 96: // '`' pressed
		SetState( DEVCON_FULLY_OPEN );
		return true;
		break;
	case 126: // '~' pressed
		SetState( DEVCON_FADE_AWAY );
		return true;
		break;
	case 9: // 'TAB' pressed
		break;
	case 8: // 'Backspace' pressed
		BackspacePressed();
		return true;
		break;
	case 127: // 'Ctrl Backspace' pressed
		CtrlBackspacePressed();
		break;
	default:
		break;
	}
	// Check to see if keyCode is in between 'space' and '~' on ASCII table
	if( keyCode >= 32 && keyCode <= 126 ) 
	{
		if( m_selecting )
		{
			DeleteSelected();
			StopSelecting();
		}
		m_timeFromLastMovedCarot = 0;
		AddCharToLine( keyCode );
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------
/**
* HandleESCPress
*/
bool DevConsole::HandleESCPress()
{
	if( IsOpen() )
	{
		if( (int) m_inputCommand.m_text.size() > 0 )
		{
			ResetInputLine();
		}
		else
		{
			SetState( DEVCON_CLOSED );
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* HandleKeyRelease
*/
bool DevConsole::HandleKeyReleased( unsigned char keyCode )
{
	UNUSED(keyCode);
	if( m_state == DEVCON_FULLY_OPEN ) 
	{
		

		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* AddCharToLine
*/
void DevConsole::AddCharToLine( unsigned char keyCode )
{
	m_inputCommand.m_text.insert( m_inputCommand.m_text.begin() + m_carotIndex, (char) keyCode );
	++m_carotIndex;
}

//--------------------------------------------------------------------------
/**
* DeletePressed
*/
void DevConsole::DeletePressed()
{
	m_timeFromLastMovedCarot = 0;
	if( m_selecting )
	{
		DeleteSelected();
		return;
	}
	if( (int) m_inputCommand.m_text.size() > 0 )
	{
		m_inputCommand.m_text.erase( m_inputCommand.m_text.begin() + m_carotIndex );
	}
}

//--------------------------------------------------------------------------
/**
* CtrlDeletePressed
*/
void DevConsole::CtrlDeletePressed()
{
	if( !m_selecting )
	{
		BeginSelecting();
	}
	JumpCarotRight();
	DeleteSelected();
	StopSelecting();
}

//--------------------------------------------------------------------------
/**
* BackspacePressed
*/
void DevConsole::BackspacePressed()
{
	m_timeFromLastMovedCarot = 0;
	if( m_selecting )
	{
		DeleteSelected();
		return;
	}

	if( (int) m_inputCommand.m_text.size() > 0 )
	{
		if( m_carotIndex - 1 >= 0 )
		{
			m_inputCommand.m_text.erase( m_inputCommand.m_text.begin() + --m_carotIndex );
		}
	}
}

//--------------------------------------------------------------------------
/**
* CtrlBackspacePressed
*/
void DevConsole::CtrlBackspacePressed()
{
	if( !m_selecting )
	{
		BeginSelecting();
	}
	JumpCarotLeft();
	DeleteSelected();
	StopSelecting();
}

//--------------------------------------------------------------------------
/**
* HomePressed
*/
void DevConsole::HomePressed()
{
	m_timeFromLastMovedCarot = 0;
	m_carotIndex = 0;
}

//--------------------------------------------------------------------------
/**
* EndPressed
*/
void DevConsole::EndPressed()
{
	m_timeFromLastMovedCarot = 0;
	m_carotIndex = (int) m_inputCommand.m_text.size();
}

//--------------------------------------------------------------------------
/**
* EnterPressed
*/
void DevConsole::EnterPressed()
{
	if( !m_inputCommand.m_text.empty() )
	{
		PrintString( Stringf("FROM CONSOLE: %s",  m_inputCommand.m_text.c_str() ), CONSOLE_INFO );
		ExecuteCommandLine( m_inputCommand.m_text );
		CopyStringIntoHistory( m_inputCommand.m_text );
		ResetInputLine();
	}
}

//--------------------------------------------------------------------------
/**
* ControlArrowPressed
*/
void DevConsole::ControlArrowPressed( unsigned char keyCode )
{
	if( m_selecting )
	{
		StopSelecting();
		return;
	}
	switch( keyCode )
	{
	case 37: // 'Left ARROW' pressed
		JumpCarotLeft();
		break;
	case 39: // 'RIGHT ARROW' pressed
		JumpCarotRight();
		break;

	default:
		break;
	}
}

//--------------------------------------------------------------------------
/**
* ArrowPressed
*/
void DevConsole::ArrowPressed( unsigned char keyCode )
{
	if( m_selecting )
	{
		StopSelecting();
		return;
	}
	switch( keyCode )
	{
	case 37: // 'Left ARROW' pressed
		MoveCarotLeft();
		break;
	case 38: // 'UP ARROW' pressed
		++m_historyIndex;
		if( m_historyIndex > (int) m_inputHistory.size() - 1 )
		{
			m_historyIndex = (int) m_inputHistory.size() - 1;
		}
		SetInputLineByHistoryIndex();
		break;
	case 39: // 'RIGHT ARROW' pressed
		MoveCarotRight();
		break;
	case 40: // 'DOWN ARROW' pressed
		--m_historyIndex;
		if( m_historyIndex < -1 )
		{
			m_historyIndex = -1;
		}
		SetInputLineByHistoryIndex();
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------------------
/**
* ShiftArrowPressed
*/
void DevConsole::ShiftArrowPressed( unsigned char keyCode )
{
	switch( keyCode )
	{
	case 37: // 'Left ARROW' pressed
		if( !m_selecting )
		{
			BeginSelecting();
		}
		if( InputSystem::IsCtrlPressed() )
		{
			JumpCarotLeft();
		}
		else
		{
			MoveCarotLeft();
		}
		break;
	case 39: // 'RIGHT ARROW' pressed
		if( !m_selecting )
		{
			BeginSelecting();
		}
		if( InputSystem::IsCtrlPressed() )
		{
			JumpCarotRight();
		}
		else
		{
			MoveCarotRight();
		}
		break;
	case 38: // 'UP ARROW' pressed
	case 40: // 'DOWN ARROW' pressed
		StopSelecting();
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------------------
/**
* ResetInputLine
*/
void DevConsole::ResetInputLine()
{
	m_inputCommand.m_text = "";
	m_carotIndex = 0;
	m_historyIndex = -1;
	StopSelecting();
}

//--------------------------------------------------------------------------
/**
* SetInputLineByHistoryIndex
*/
void DevConsole::SetInputLineByHistoryIndex()
{
	if( m_historyIndex < 0 )
	{
		ResetInputLine();
	}
	else
	{
		m_inputCommand.m_text = m_inputHistory[m_historyIndex];
		m_carotIndex = (int) m_inputCommand.m_text.size();
	}
}

//--------------------------------------------------------------------------
/**
* CopyStringIntoHistory
*/
void DevConsole::CopyStringIntoHistory( std::string line )
{
	for( int i = 0; i < (int) m_inputHistory.size(); ++i )
	{
		std::string& hLine = m_inputHistory[i];
		if( hLine == line )
		{
			m_inputHistory.erase( m_inputHistory.begin() + i );
			break;
		}
	}
	m_inputHistory.insert( m_inputHistory.begin(), line );
}

//--------------------------------------------------------------------------
/**
* Clear
*/
void DevConsole::Clear()
{
	ResetInputLine();
	m_lines.clear();
}

//--------------------------------------------------------------------------
/**
* BeginSelecting
*/
void DevConsole::BeginSelecting()
{
	m_selecting = true;
	m_beingSelectingIndex = m_carotIndex;
}


//--------------------------------------------------------------------------
/**
* StopSelecting
*/
void DevConsole::StopSelecting()
{
	m_selecting = false;
	m_beingSelectingIndex = -1;
}

//--------------------------------------------------------------------------
/**
* DeleteSelected
*/
void DevConsole::DeleteSelected()
{
	if( m_beingSelectingIndex == m_carotIndex || m_beingSelectingIndex < 0 )
	{
		return;
	}
	if( m_beingSelectingIndex < m_carotIndex )
	{
		int tempVar = m_beingSelectingIndex;
		m_beingSelectingIndex = m_carotIndex;
		m_carotIndex = tempVar;
	}
	
	m_inputCommand.m_text.erase
	( 
		m_inputCommand.m_text.begin() + m_carotIndex
		, m_inputCommand.m_text.begin() + m_beingSelectingIndex 
	);
	StopSelecting();
}

//--------------------------------------------------------------------------
/**
* MoveCarotRight
*/
void DevConsole::MoveCarotRight()
{
	++m_carotIndex;
	m_timeFromLastMovedCarot = 0;
	if( m_carotIndex > (int) m_inputCommand.m_text.size() )
	{
		m_carotIndex = (int) m_inputCommand.m_text.size();
		if( m_carotIndex < 0 )
		{
			m_carotIndex = 0;
		}
	}
}

//--------------------------------------------------------------------------
/**
* MoveCarotLeft
*/
void DevConsole::MoveCarotLeft()
{
	--m_carotIndex;
	m_timeFromLastMovedCarot = 0;
	if( m_carotIndex < 0 )
	{
		m_carotIndex = 0;
	}
}

//--------------------------------------------------------------------------
/**
* JumpCarotRight
*/
void DevConsole::JumpCarotRight()
{
	while( m_carotIndex < m_inputCommand.m_text.size() )
	{
		++m_carotIndex;
		if( m_inputCommand.m_text[m_carotIndex] == ' ' || m_inputCommand.m_text[m_carotIndex] == '	' )
		{
			while( m_inputCommand.m_text[m_carotIndex] == ' ' || m_inputCommand.m_text[m_carotIndex] == '	' )
			{
				++m_carotIndex;
			}
			break;
		}
	}
	if( m_carotIndex > m_inputCommand.m_text.size() )
	{
		m_carotIndex = (int) m_inputCommand.m_text.size();
	}
}

//--------------------------------------------------------------------------
/**
* JumpCarotLeft
*/
void DevConsole::JumpCarotLeft()
{
	if( m_carotIndex == 0 )
	{
		return;
	}
	--m_carotIndex;
	while( m_inputCommand.m_text[m_carotIndex] == ' ' || m_inputCommand.m_text[m_carotIndex] == '	' )
	{
		--m_carotIndex;
	}
	while( m_carotIndex > 0 )
	{
		--m_carotIndex;
		if( m_inputCommand.m_text[m_carotIndex] == ' ' || m_inputCommand.m_text[m_carotIndex] == '	' )
		{
			++m_carotIndex;
			break;
		}
	}
	if( m_carotIndex < 0 )
	{
		m_carotIndex = 0 ;
	}
}

//--------------------------------------------------------------------------
/**
* Command_Test
*/
bool DevConsole::Command_Test( EventArgs& args )
{
	args.SetValue("test1", "Test1 was Set");
	args.SetValue("test2", "Test2 was Set");
	g_theConsole->AddLineAtEnd( "DevConsole::Command_Test Ran", CONSOLE_ECHO );
	return false;
}

//--------------------------------------------------------------------------
/**
* Command_Help
*/
bool DevConsole::Command_Help( EventArgs& args )
{
	UNUSED(args);
	g_theConsole->AddLineAtEnd( "Registered Events:", CONSOLE_INFO );
	Strings listOfCommands;
	g_theEventSystem->GetSubscribedEventsList( listOfCommands );

	for( int commandIndex = 0; commandIndex < (int) listOfCommands.size(); commandIndex++ )
	{
		const std::string& commandName = listOfCommands[commandIndex];
		g_theConsole->AddLineAtEnd( Stringf( "			%s (args)", commandName.c_str() ), CONSOLE_INFO );
	}
	g_theConsole->AddLineAtEnd( "Example Execution: exec toExec=help ", CONSOLE_INFO );

	return false;
}

//--------------------------------------------------------------------------
/**
* Command_Exec
*/
bool DevConsole::Command_Exec( EventArgs& args )
{
	g_theConsole->AddLineAtEnd( "DevConsole::Command_Exec Ran", CONSOLE_ECHO );
	g_theConsole->ExecuteCommandLine( args.GetValue( "toExec", "UNKNOWN_LINE_TO_EXECUTE" ) );
	return true;
}

//--------------------------------------------------------------------------
/**
* Command_ClearConsole
*/
bool DevConsole::Command_ClearConsole( EventArgs& args )
{
	UNUSED(args);
	g_theConsole->Clear();
	return true;
}

//--------------------------------------------------------------------------
/**
* Command_ClearConsoleHistory
*/
bool DevConsole::Command_ClearConsoleHistory( EventArgs& args )
{
	UNUSED(args);
	g_theConsole->m_inputHistory.clear();
	return true;
}

//--------------------------------------------------------------------------
/**
* Command_LogCurrentMemoryAllocs
*/
bool DevConsole::Command_LogCurrentMemoryAllocs(EventArgs& args)
{
	UNUSED(args);
	MemTrackLogLiveAllocations(10);
	return true;
}

//--------------------------------------------------------------------------
/**
* OpenRender
*/
void DevConsole::OpenRender( RenderContext* renderContext, const Camera& camera, float textHeight ) const
{	
	renderContext->BindSampler( SAMPLE_MODE_LINEAR );
	Shader* shader = renderContext->CreateOrGetShaderFromXML( "Data/Shaders/devShader.xml" );
	
	BitmapFont* font = renderContext->CreateOrGetBitmapFromFile( m_bitmapName.c_str() );
	renderContext->BindShader( shader );
	
	RenderBackground( renderContext, camera );
	std::vector<Vertex_PCU> verts;
	

	float topRightY = camera.GetOrthoTopRight().y;
	float topRightX = camera.GetOrthoTopRight().x;
	float botLeftY = camera.GetOrthoBottomLeft().y;
	float botLeftX = camera.GetOrthoBottomLeft().x;

	int maxLinesToDraw = (int) ( ( topRightY - camera.GetOrthoBottomLeft().y ) / textHeight );

	// Draw InputLine
	AABB2 inputLinePos
	(	
		botLeftX, botLeftY
		, topRightX, botLeftY + textHeight
	);

	
	font->AddVertsFor2DTextAlignedInBox( 
		verts
		, textHeight
		, Stringf( "> %s", m_inputCommand.m_text.c_str() ).c_str()
		, inputLinePos
		, Vec2::ALIGN_CENTER_LEFT
		, BITMAP_MODE_SHRINK_TO_FIT
		, m_textAspectRatio
		, m_inputCommand.m_color
	);

	botLeftY  += textHeight;

	// Draw History
	int count = 0;
	for (int lineIndex = (int) m_lines.size() - 1; lineIndex >= 0 && count <= maxLinesToDraw; lineIndex--)
	{
		TextLine* line = m_lines[lineIndex];

		AABB2 linePosition
		(	
			botLeftX	, botLeftY + (float) count * textHeight
			,	topRightX		, botLeftY	+ (float) (count + 1) * textHeight
		);
		AABB2 lineBlackPosition
		(	
			botLeftX - 0.2f * textHeight, botLeftY + (float) count * textHeight  - 0.2f * textHeight
			,	topRightX + 0.2f * textHeight, botLeftY	+ (float) (count + 1) * textHeight + 0.2f * textHeight
		);

		font->AddVertsFor2DTextAlignedInBox( 
			verts
			, textHeight + 0.2f * textHeight
			, line->m_text.c_str()
			, linePosition
			, Vec2::ALIGN_CENTER_LEFT
			, BITMAP_MODE_SHRINK_TO_FIT
			, m_textAspectRatio
			, Rgba::BLACK
		);

		Rgba color = line->m_color;
		font->AddVertsFor2DTextAlignedInBox( 
			verts
			, textHeight
			, line->m_text.c_str()
			, linePosition
			, Vec2::ALIGN_CENTER_LEFT
			, BITMAP_MODE_SHRINK_TO_FIT
			, m_textAspectRatio
			, Rgba( color.r, color.g, color.b, 1.0f)
		);	

		count++;
	}

	// Draw Cursor
	std::vector<Vertex_PCU> cursorVerts;
	int cursorLocationIndexLoc =  12 + m_carotIndex * 6 - 5;
	Vec3 startCurPos = verts[cursorLocationIndexLoc].position;
	Vec2 start = Vec2( startCurPos.x, startCurPos.y - textHeight * .01f );
	Vec2 end = Vec2( startCurPos.x, startCurPos.y + textHeight + textHeight * .01f );
	AddVertsForLine2D( cursorVerts, start, end, 0.18f, CONSOLE_INFO );

	if( m_selecting )
	{
		std::vector<Vertex_PCU> selectVerts;

		// Draw Selection Box
		int beginLocationIndexLoc =  12 + m_beingSelectingIndex * 6 - 5;
		
		if( m_beingSelectingIndex < m_carotIndex )
		{
			int tempVar = beginLocationIndexLoc;
			beginLocationIndexLoc = cursorLocationIndexLoc;
			cursorLocationIndexLoc = tempVar;
		}

		startCurPos = verts[cursorLocationIndexLoc].position;
		Vec3 initSelectPos = verts[beginLocationIndexLoc].position;

		Vec2 botLeftSelect( startCurPos.x, startCurPos.y );
		Vec2 topRightSelect( initSelectPos.x, initSelectPos.y );

		AABB2 selectingBox
		(	
			botLeftSelect.x - 0.2f * textHeight , botLeftSelect.y - 0.2f * textHeight
			, topRightSelect.x + 0.2f * textHeight, topRightSelect.y + textHeight + 0.2f * textHeight
		);

		AddVertsForAABB2D( selectVerts, selectingBox, Rgba( 0.3f, 1.0f, 3.0f, 0.4f ) );
		
		if( selectVerts.size() > 0.0f )
		{
			renderContext->BindTextureView( TEXTURE_SLOT_ALBEDO, nullptr );
			renderContext->DrawVertexArray( selectVerts );
		}
	}


	if( verts.size() > 0.0f )
	{
		renderContext->BindTextureView( TEXTURE_SLOT_ALBEDO, font->GetTextureView() );
		renderContext->DrawVertexArray( verts );
	}

	if( ( CosDegrees( m_timeFromLastMovedCarot * 360.0f ) + 1.0f ) * 0.5f > 0.5f )
	{
		renderContext->BindTextureView( TEXTURE_SLOT_ALBEDO, nullptr );
		renderContext->DrawVertexArray( cursorVerts );
	}
}

//--------------------------------------------------------------------------
/**
* FadeRender
*/
void DevConsole::FadeRender( RenderContext* renderContext, const Camera& camera, float textHeight ) const
{
	Shader* shader = renderContext->CreateOrGetShaderFromXML( "Data/Shaders/devShader.xml" );

	renderContext->BindShader( shader );
	BitmapFont* font = renderContext->CreateOrGetBitmapFromFile( m_bitmapName.c_str() );
	
	std::vector<Vertex_PCU> verts;
	float topRightY = camera.GetOrthoTopRight().y;
	float topRightX = camera.GetOrthoTopRight().x;
	float botLeftY = camera.GetOrthoBottomLeft().y;
	float botLeftX = camera.GetOrthoBottomLeft().x;

	int maxLinesToDraw = (int) ( ( topRightY - botLeftY ) / textHeight );

	AABB2 inputLinePos
	(	
		botLeftX, botLeftY
		, topRightX, botLeftY + textHeight
	);

	font->AddVertsFor2DTextAlignedInBox( 
		verts
		, textHeight
		, "> *** FADE MODE *** NO INPUT TO CONCOLE"
		, inputLinePos
		, Vec2::ALIGN_CENTER_LEFT
		, BITMAP_MODE_SHRINK_TO_FIT
		, m_textAspectRatio
		, CONSOLE_INFO
	);

	botLeftY  += textHeight;

	int count = 0;
	for (int lineIndex = (int) m_lines.size() - 1; lineIndex >= 0 && count <= maxLinesToDraw; lineIndex--)
	{
		TextLine* line = m_lines[lineIndex];
		if( line->m_color.a <= 0.0f )
		{
			break;
		}

		AABB2 linePosition
		(	
			botLeftX, botLeftY + (float) count * textHeight
			, topRightX, botLeftY + (float) (count + 1) * textHeight
		);

		font->AddVertsFor2DTextAlignedInBox( 
			verts
			, textHeight
			, line->m_text.c_str()
			, linePosition
			, Vec2::ALIGN_CENTER_LEFT
			, BITMAP_MODE_SHRINK_TO_FIT
			, m_textAspectRatio
			, line->m_color
		);	

		count++;
	}
	if( verts.size() > 0.0f )
	{
		renderContext->BindSampler( SAMPLE_MODE_LINEAR );
		renderContext->BindTextureView( TEXTURE_SLOT_ALBEDO, font->GetTextureView() );
		renderContext->DrawVertexArray( verts );
	}
}

//--------------------------------------------------------------------------
/**
* RenderBackground
*/
void DevConsole::RenderBackground( RenderContext* renderContext, const Camera& camera ) const
{
	std::vector<Vertex_PCU> verts;
	AddVertsForAABB2D( verts, AABB2( camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight() ), m_backgroundColor );
	renderContext->BindTextureView( TEXTURE_SLOT_ALBEDO, nullptr );
	renderContext->BindSampler( SAMPLE_MODE_LINEAR );
	renderContext->DrawVertexArray(verts);
}

//--------------------------------------------------------------------------
/**
* ExecuteCommandLine
*/
void DevConsole::ExecuteCommandLine( const std::string& commandLine )
{
	Strings eachLine = SplitStringOnDelitmiter( commandLine.c_str(), "\n" );
	for( int commandIndex = 0; commandIndex < (int) eachLine.size(); commandIndex++ )
	{
		Strings splitLine = SplitStringOnDelitmiter( eachLine[commandIndex].c_str(), " " );
		std::string command = splitLine[0];
		if( g_theEventSystem->GetNumSubscribersForCommand( command ) == 0 )
		{
			g_theConsole->PrintString( Stringf( "No subscribers to: %s", command.c_str() ), CONSOLE_WARNING );
			continue;
		}

		EventArgs argsToCommand;
		for( int argIndex = 1; argIndex < (int) splitLine.size(); argIndex++ )
		{ 
			Strings splitArgument = SplitStringOnDelitmiter( splitLine[argIndex].c_str(), "=" );
			if( ( (int) splitArgument.size() ) != 2 )
			{
				g_theConsole->PrintString( Stringf( "Invalid argument pair %s given to command: %s", splitLine[argIndex].c_str(), command.c_str() ), CONSOLE_ERROR );
				g_theConsole->PrintString( "All arguments must be in form ArgKey=ArgValue", CONSOLE_ERROR_DESC );
				g_theConsole->PrintString( Stringf( "Line given: %s", eachLine[commandIndex].c_str() ), CONSOLE_ERROR_DESC );
				return;
			}
			argsToCommand.SetValue( splitArgument[0], splitArgument[1] );
		}

		g_theEventSystem->FireEvent( command, argsToCommand );
	}
}

//--------------------------------------------------------------------------
/**
* AddLineAtEnd
*/
void DevConsole::AddLineAtEnd( const std::string& toPrint, const Rgba& color )
{
	PrintString( toPrint, color );
}

//--------------------------------------------------------------------------
/**
* GetStateToString
*/
std::string DevConsole::GetStateToString( eDevConsoleState state )
{
	switch( state )
	{
	case UNKNOWN_DEVCONSOLE_STATE:
		return "UNKNOWN_DEVCONSOLE_STATE";
		break;
	case DEVCON_CLOSED:
		return "DEVCON_CLOSED";
		break;
	case DEVCON_FADE_AWAY:
		return "DEVCON_FADE_AWAY";
		break;
	case DEVCON_FULLY_OPEN:
		return "DEVCON_FULLY_OPEN";
		break;
	default:
		return "STATE NOT FOUND";
		break;
	}
}


#endif