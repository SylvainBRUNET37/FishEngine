#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "UIManager.h"
#include "entities/EntityManager.h"
#include "systems/PhysicsSimulationSystem.h"
#include "systems/RenderSystem.h"
#include "systems/System.h"

class GameEngine
{
public:
	explicit GameEngine(RenderContext* renderContext_);

	void Run();

private:
	static constexpr double TARGET_FPS = 60.0;
	static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;

	RenderContext* renderContext; // should not be there idealy, but it helps for testing
	std::vector<std::unique_ptr<System>> systems;

	Entity mainMenuEntity;

	std::shared_ptr<UIManager> uiManager;
	EntityManager entityManager;

	static void WaitBeforeNextFrame(DWORD frameStartTime);

	void HandleGameState();
	void HandleCollions();

	void ChangeGameStatus();
	void ResumeGame();
	void PauseGame();
	void EndGame();
	void RestartGame();

	void InitGame();

	void BuildPauseMenu();
	void BuildOptionMenu();
	void BuildEndMenu();
};

#endif
