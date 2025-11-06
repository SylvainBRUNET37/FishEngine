#include "pch.h"
#include "GameEngine.h"

#include "GameState.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/texture/TextureLoader.h"

void GameEngine::Run()
{
	bool shouldContinue = true;
	DWORD prevTime = GetTickCount();

	while (shouldContinue)
	{
		// Pause/Unpause the game if ESC is pressed for exemple
		HandleGameState();

		const DWORD frameStartTime = GetTickCount();
		const auto isGamePaused = GameState::currentState == GameState::PAUSED;

		const double elapsedTime = isGamePaused ? 0.0 : (frameStartTime - prevTime) / 1000.0;

		if (not isGamePaused) [[likely]]
			prevTime = frameStartTime;

		// End the loop if Windows want to terminate the program (+ process messages)
		shouldContinue = WindowsApplication::ProcessWindowsMessages();

		for (const auto& system : systems)
			system->Update(elapsedTime, entityManager);

		WaitBeforeNextFrame(frameStartTime);
	}
}

void GameEngine::WaitBeforeNextFrame(const DWORD frameStartTime)
{
	const DWORD frameEnd = GetTickCount();
	const DWORD frameDuration = frameEnd - frameStartTime;

	if (frameDuration < FRAME_TIME)
		Sleep(static_cast<DWORD>(FRAME_TIME - frameDuration));
}

void GameEngine::HandleGameState()
{
	static bool wasEscapePressed = false;
	const bool isEscapePressed = GetAsyncKeyState(VK_ESCAPE) & 0x8000;

	if (isEscapePressed && !wasEscapePressed)
		ChangeGameStatus();

	wasEscapePressed = isEscapePressed;
}

void GameEngine::ChangeGameStatus()
{
	static Entity mainMenuEntity = entityManager.CreateEntity();

	GameState::currentState == GameState::PAUSED
		? PauseGame(mainMenuEntity)
		: ResumeGame(mainMenuEntity);
}

void GameEngine::PauseGame(const Entity mainMenuEntity)
{
	CameraSystem::SetMouseCursor();
	GameState::currentState = GameState::PLAYING;
	entityManager.RemoveComponent<Sprite2D>(mainMenuEntity);
}

void GameEngine::ResumeGame(const Entity mainMenuEntity)
{
	ShowCursor(TRUE);
	Camera::isMouseCaptured = false;

	ClipCursor(nullptr);
	ReleaseCapture();

	GameState::currentState = GameState::PAUSED;

	entityManager.AddComponent<Sprite2D>
	(
		mainMenuEntity, 
		uiManager.LoadSprite("assets/pauseMenu.jpg", resourceManager)
	);
}
