#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "rendering/RenderSystem.h"

class GameEngine
{
public:
	explicit GameEngine(RenderSystem&& renderSystem) : renderSystem{std::move(renderSystem)} {}

	void Run();

	static void UpdatePhysics();

private:
	static constexpr double PHYSICS_UPDATE_RATE = 1.0f / 60.0f;
	static constexpr double TARGET_FPS = 60.0;
	static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;

	RenderSystem renderSystem;

	static void WaitBeforeNextFrame(DWORD frameStartTime);
};

#endif
