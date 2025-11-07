#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "ResourceManager.h"
#include "UIManager.h"
#include "entities/EntityManager.h"
#include "systems/System.h"

class GameEngine
{
public:
	explicit GameEngine(EntityManager&& entityManager, ResourceManager&& resourceManager, UIManager&& uiManager,
	                    std::vector<std::unique_ptr<System>>&& systems)
		: systems{std::move(systems)},
		  uiManager{std::move(uiManager)},
		  resourceManager{std::move(resourceManager)},
		  entityManager{std::move(entityManager)}
	{
		InitGame();
	}

	void Run();

private:
	static constexpr double TARGET_FPS = 60.0;
	static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;

	std::vector<std::unique_ptr<System>> systems;

	Entity mainMenuEntity;

	UIManager uiManager;
	ResourceManager resourceManager;
	EntityManager entityManager;

	static void WaitBeforeNextFrame(DWORD frameStartTime);

	void HandleGameState();
	void HandleCollions();

	void ChangeGameStatus();
	void PauseGame(Entity mainMenuEntity);
	void ResumeGame(Entity mainMenuEntity);

	void InitGame();
};

#endif
