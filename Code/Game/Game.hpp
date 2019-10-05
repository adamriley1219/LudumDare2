#pragma once
#include "Game/GameCommon.hpp"

class Shader;

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


private:
	//Render
	void RenderDebug() const;
	void RenderDebugCosmetics() const;
	void RenderDebugPhysics() const;
	void RenderDevConsole() const;

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

	mutable Camera m_CurentCamera;
	mutable Camera m_DevColsoleCamera;

};