#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "pch.h"

#include <mutex>

#include "systems/CameraSystem.h"
#include <queue>

#include "rendering/postProcessing/PostProcessSettings.h"

struct GameState
{
    enum State : uint8_t
    {
	    PLAYING,
        PAUSED,
        DIED,
        WON,
        FINISHED
    };

    inline static PostProcessSettings postProcessSettings{};

    static void AddCollision(const std::pair<JPH::BodyID, JPH::BodyID>& collision);
    static std::queue<std::pair<JPH::BodyID, JPH::BodyID>>& GetCollisions();

    inline static Entity currentCameraEntity = INVALID_ENTITY;
    inline static auto currentState = PLAYING;

    inline static float playTime = 0.0f;
    //inline static std::chrono::system_clock::time_point startTime;

    inline static bool isGrowing = false;

    inline static double rDeltaTime = 0.0;

private:
    inline static std::queue<std::pair<JPH::BodyID, JPH::BodyID>> detectedCollisions;
    static inline std::mutex pendingMutex;
};

#endif