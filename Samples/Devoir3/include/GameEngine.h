#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "ecs/EntityManager.h"
#include "rendering/RenderSystem.h"

class GameEngine
{
public:
	explicit GameEngine(RenderSystem&& renderSystem, EntityManager&& entityManager)
		: renderSystem{std::move(renderSystem)},
		  entityManager{std::move(entityManager)}
	{
	}

	void Run();

private:
	static constexpr double PHYSICS_UPDATE_RATE = 1.0f / 60.0f;
	static constexpr double TARGET_FPS = 60.0;
	static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;

	RenderSystem renderSystem;
	EntityManager entityManager;

	static void UpdatePhysics();

	static void WaitBeforeNextFrame(DWORD frameStartTime);
};

#endif
