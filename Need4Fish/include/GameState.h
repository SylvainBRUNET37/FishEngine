#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "pch.h"
#include "systems/CameraSystem.h"
#include <queue>

struct GameState
{
    enum State : uint8_t
    {
	    PLAYING,
        PAUSED,
        DIED,
        WON
    };

    inline static std::queue<std::pair<JPH::BodyID, JPH::BodyID>> detectedCollisions;

    inline static Entity currentCameraEntity = INVALID_ENTITY;
    inline static auto currentState = PLAYING;
};

#endif