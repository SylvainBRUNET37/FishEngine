#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "ResourceManager.h"
#include "entityComponentSystem/EntityManager.h"
#include "rendering/RenderSystem.h"
#include <cstdlib>

class GameEngine
{
public:
	explicit GameEngine(RenderSystem&& renderSystem, EntityManager&& entityManager, ResourceManager&& resourceManager)
		: resourceManager{std::move(resourceManager)},
		  renderSystem{std::move(renderSystem)},
		  entityManager{std::move(entityManager)}
	{
		std::srand(std::time(0));
		MoveSensorRandomly();
	}

	void Run();

private:
	static constexpr double TARGET_FPS = 60.0;
	static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;
	static constexpr double PHYSICS_UPDATE_RATE = 1.0f / TARGET_FPS;

	ResourceManager resourceManager;
	RenderSystem renderSystem;
	EntityManager entityManager;

	void UpdatePhysics();
	void UpdateTransforms();
	void RenderScene(double elapsedTime);
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
