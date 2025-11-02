#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "ResourceManager.h"
#include "ecs/EntityManager.h"
#include "rendering/RenderSystem.h"

class GameEngine
{
public:
	explicit GameEngine(RenderSystem&& renderSystem, EntityManager&& entityManager, ResourceManager&& resourceManager)
		: resourceManager{std::move(resourceManager)},
		  renderSystem{std::move(renderSystem)},
		  entityManager{std::move(entityManager)}
	{
	}

	void Run();

private:
	static constexpr double TARGET_FPS = 60.0;
	static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;
	static constexpr double PHYSICS_UPDATE_RATE = 1.0f / TARGET_FPS;

	ResourceManager resourceManager;
	RenderSystem renderSystem;
	EntityManager entityManager;

	static void UpdatePhysics();
	void UpdateTransforms();
	void RenderScene(double elapsedTime);

	void ShootBallIfKeyPressed();

	static void WaitBeforeNextFrame(DWORD frameStartTime);
};

#endif
