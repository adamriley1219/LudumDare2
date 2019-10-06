#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shaders/Shader.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"

#include "Engine/Core/Debug/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Time/StopWatch.hpp"

#include "Engine/ImGUI/ImGUISystem.hpp"
#include "Engine/Input/InputSystem.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include <vector>

#include <Math.h>
//--------------------------------------------------------------------------
/**
* Game
*/
Game::Game()
{
	ConstructGame();
}

//--------------------------------------------------------------------------
/**
* ~Game
*/
Game::~Game()
{
	DeconstructGame();
}

//--------------------------------------------------------------------------
/**
* GameStartup
*/
void Game::Startup()
{
	m_shader = g_theRenderer->CreateOrGetShaderFromXML( "Data/Shaders/shader.xml" );
	g_theRenderer->m_shader = m_shader;

	m_DevColsoleCamera.SetOrthographicProjection( Vec2( -100.0f, -50.0f ), Vec2( 100.0f,  50.0f ) );
	m_DevColsoleCamera.SetModelMatrix( Matrix44::IDENTITY );

	EventArgs args;
	g_theDebugRenderSystem->Command_Open(args);


	player_bad_response.push_back("...");
	player_bad_response.push_back("Boooooo...");
	
	player_good_response.push_back("Great!");
	player_good_response.push_back("Good!");
	player_good_response.push_back("Nice, lets move on.");

	player_recovery_response.push_back("Really? It's going to be like that?");
	player_recovery_response.push_back("You probably weren't ready for that.");

	player_random_diolog.push_back( "Do.. do do.. do do...");
	player_random_diolog.push_back( "Well, your still here I guess." );
	player_random_diolog.push_back( "Wonder what's for dinner." );
	player_random_diolog.push_back( "I might set up a party next week..." );
	player_random_diolog.push_back( "I can see your doing your best, I guess." );
	player_random_diolog.push_back( "Don't mind me. I'm just waiting." );
	player_random_diolog.push_back( "Hmmm, tuna, tomato, block.. oh wait, shouldn't give hints" );
	player_random_diolog.push_back( "You can do it! (I wonder how well they can build.)" );

	responseTimer = new StopWatch( g_theApp->GetGameClock() );
	responseTimer->SetAndReset( 0.01f );

	randomTextTimer = new StopWatch( g_theApp->GetGameClock() );

	player_text_queue.push("hello, you ready?");
}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void Game::Shutdown()
{

}

static int g_index = 0;

//--------------------------------------------------------------------------
/**
* HandleKeyPressed
*/
bool Game::HandleKeyPressed( unsigned char keyCode )
{
	if( keyCode == 'O' )
	{
		g_index = ++g_index % ( 8 * 2 );
	}
	if( keyCode == 'W' )
	{
		EventArgs args;
		g_theEventSystem->FireEvent( "test" );
		g_theEventSystem->FireEvent( "test", args );
		g_theConsole->PrintString( args.GetValue( "test1", "NOT FOUND" ), DevConsole::CONSOLE_INFO );
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* HandleKeyReleased
*/
bool Game::HandleKeyReleased( unsigned char keyCode )
{
	UNUSED(keyCode);
	return false;
}


//--------------------------------------------------------------------------
/**
* GameRender
*/
void Game::GameRender() const
{
	g_theDebugRenderSystem->RenderToCamera( &m_DevColsoleCamera );
}

//--------------------------------------------------------------------------
/**
* UpdateGame
*/
void Game::UpdateGame( float deltaSeconds )
{
	UpdateTextToPlayer( deltaSeconds );
	ImGUIWidget();
	UpdateCamera( deltaSeconds );
}



//--------------------------------------------------------------------------
/**
* GetBadResponse
*/
const std::string& Game::GetBadResponse()
{
	ASSERT_RECOVERABLE(player_bad_response.size(), "player_bad_response empty ");

	float roll = GetRandomFloatFromZeroToOne();
	uint idx = (uint)( ( (float)player_bad_response.size() - 1 ) * roll );
	return player_bad_response[idx];
}

//--------------------------------------------------------------------------
/**
* GetGoodResponse
*/
const std::string& Game::GetGoodResponse()
{
	ASSERT_RECOVERABLE( player_good_response.size(), "player_good_response empty " );
	float roll = GetRandomFloatFromZeroToOne();
	uint idx = (uint)(((float)player_good_response.size() - 1) * roll);
	return player_good_response[idx];
}

//--------------------------------------------------------------------------
/**
* GetRecoveryResponse
*/
const std::string& Game::GetRecoveryResponse()
{
	ASSERT_RECOVERABLE(player_recovery_response.size(), "player_recovery_response empty ");
	float roll = GetRandomFloatFromZeroToOne();
	uint idx = (uint)(((float)player_recovery_response.size() - 1) * roll);
	return player_recovery_response[idx];
}

//--------------------------------------------------------------------------
/**
* GetRandomText
*/
const std::string& Game::GetRandomText()
{
	ASSERT_RECOVERABLE(player_random_diolog.size(), "player_random_diolog empty ");
	float roll = GetRandomFloatFromZeroToOne();
	uint idx = (uint)(((float)player_random_diolog.size() - 1) * roll);
	return player_random_diolog[idx];
}

//--------------------------------------------------------------------------
/**
* PushTextToPlayer
*/
void Game::PushTextToPlayer( const std::string& text )
{
	player_text_queue.push( text );
	responseTimer->SetAndReset(0.000001f);
}

//--------------------------------------------------------------------------
/**
* GetNextTextToPlayer
*/
const std::string& Game::SeeTextToPlayer() const
{
	return player_text_queue.front();
}

//--------------------------------------------------------------------------
/**
* PopTextToPlayer
*/
bool Game::PopTextToPlayer()
{
	// Ensure there's always something to show, if you want blank. Post "".
	if( player_text_queue.size() > 1 )
	{
		player_text_queue.pop();
		return true;
	}
	return false;
}


//--------------------------------------------------------------------------
/**
* UpdateTextToPlayer
*/
void Game::UpdateTextToPlayer(float deltaSeconds)
{
	if( responseTimer->HasElapsed() || g_theInputSystem->KeyWasPressed( KEY_SPACEBAR ) )
	{
		if( PopTextToPlayer() )
		{
			randomTextTimer->Reset();
		}
		responseTimer->SetAndReset(3.0f);
	}
	if ( ( randomTextTimer->HasElapsed() || g_theInputSystem->KeyWasPressed( KEY_SPACEBAR ) ) && player_text_queue.size() == 1 )
	{
		PushTextToPlayer( GetRandomText() );
		randomTextTimer->Reset();
	}
}

//--------------------------------------------------------------------------
/**
* ImGUIWidget
*/
void Game::ImGUIWidget()
{
	int flags = ( 1 ) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 13);
	if( begun )
	{

		ImGUI_BeginWindow("top left Widget", 0, flags);

		ImGUI_Text( SeeTextToPlayer() );

		ImGUI_EndWindow();
	}
	else
	{
		ImGUI_BeginWindow("beginning Widget", 0, flags);

		ImGUI_Text( SeeTextToPlayer() );
		ImGUI_Text( "" );
		ImGUI_Text( "" );


		bool yesButton = yes.IsPressed();
		bool noButton = no.IsPressed();
		yes.UpdateStatus( ImGUI_SelectableText( "Yes", yesButton ) );
		no.UpdateStatus( ImGUI_SelectableText( "No", noButton ) );

		if( yes.WasJustPressed() )
		{
			PushTextToPlayer( GetGoodResponse() );
			PushTextToPlayer( "Ok, lets get started");
			PushTextToPlayer( "I'll leave the rest to you");
			begun = true;
			randomTextTimer->SetAndReset( 7.0f );
		}
		if( no.WasJustPressed() )
		{
			PushTextToPlayer( GetBadResponse() );
			PushTextToPlayer( GetRecoveryResponse() );
			PushTextToPlayer("Ok, lets get started anyway...");
			PushTextToPlayer("I'll leave the rest to you");
			begun = true;
			randomTextTimer->SetAndReset( 7.0f );
		}
		ImGUI_EndWindow();
	}


}

//--------------------------------------------------------------------------
/**
* UpdateCamera
*/
void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	m_CurentCamera.SetModelMatrix( Matrix44::IDENTITY );
	m_CurentCamera.SetOrthographicProjection( Vec2(), Vec2(WORLD_WIDTH, WORLD_HEIGHT) );
	m_CurentCamera.SetColorTargetView( g_theRenderer->GetColorTargetView() );
	m_CurentCamera.SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
	g_theRenderer->BeginCamera( &m_CurentCamera );
}

//--------------------------------------------------------------------------
/**
* ResetGame
*/
void Game::ResetGame()
{
	DeconstructGame();
	ConstructGame();
}

//--------------------------------------------------------------------------
/**
* ConstructGame
*/
void Game::ConstructGame()
{

}

//--------------------------------------------------------------------------
/**
* DeconstructGame
*/
void Game::DeconstructGame()
{

}
