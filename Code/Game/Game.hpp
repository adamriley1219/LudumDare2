#pragma once
#include "Game/GameCommon.hpp"

#include "Engine/Input/KeyButtonState.hpp"

#include <queue>

class Shader;
class StopWatch;

class Game
{
	friend App;
public:
	Game();
	~Game();

	void Startup();
	void Shutdown();

	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );

	void GameRender() const;
	void UpdateGame( float deltaSeconds );

	const std::string& GetBadResponse(); 
	const std::string& GetGoodResponse(); 
	const std::string& GetRecoveryResponse(); 
	const std::string& GetRandomText(); 

	void PushTextToPlayer( const std::string& text );
	const std::string& SeeTextToPlayer() const;
	bool PopTextToPlayer();

	void UpdateTextToPlayer( float deltaSeconds );

private:
	void ImGUIWidget();

	void UpdateCamera( float deltaSeconds );

private:
	void ResetGame();

	// Helper Methods
	void ConstructGame();
	void DeconstructGame();
private:
	bool m_isQuitting = false;

	Shader* m_shader;

	std::vector<std::string> player_bad_response;
	std::vector<std::string> player_good_response;
	std::vector<std::string> player_recovery_response;
	std::vector<std::string> player_random_diolog;

	std::queue<std::string> player_text_queue;

	KeyButtonState yes;
	KeyButtonState no;
	bool begun = false;

	StopWatch* responseTimer;
	StopWatch* randomTextTimer;

	mutable Camera m_CurentCamera;
	mutable Camera m_DevColsoleCamera;

};