#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "pch.h"
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

    inline static std::queue<std::pair<JPH::BodyID, JPH::BodyID>> detectedCollisions;

    inline static Entity currentCameraEntity = INVALID_ENTITY;
    inline static auto currentState = PLAYING;

    inline static bool isGrowing = false;
};

#endif