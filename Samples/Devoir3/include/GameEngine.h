#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <cstdlib>

#include "ResourceManager.h"
#include "entityComponentSystem/EntityManager.h"
#include "systems/RenderSystem.h"
#include "systems/System.h"

class GameEngine
{
public:
	explicit GameEngine(EntityManager&& entityManager, ResourceManager&& resourceManager,
	                    std::vector<std::unique_ptr<System>>&& systems)
		: systems{std::move(systems)},
		  resourceManager{std::move(resourceManager)},
		  entityManager{std::move(entityManager)}
	{
		std::srand(std::time(nullptr));
		MoveSensorRandomly();
	}

	void Run();

private:
	static constexpr double TARGET_FPS = 60.0;
	static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;
	static constexpr double PHYSICS_UPDATE_RATE = 1.0f / TARGET_FPS;

	std::vector<std::unique_ptr<System>> systems;

	ResourceManager resourceManager;
	EntityManager entityManager;

	void CheckForWinConditions();

	int currentWinCount = 0;

	// BallShooterSystem
	void ShootBallIfKeyPressed();
	// LifeSpan system
	void DestroyObjectAtEndOfLife(double elapsedTIme);

	static void WaitBeforeNextFrame(DWORD frameStartTime);

	void MoveSensorRandomly();
};

#endif
