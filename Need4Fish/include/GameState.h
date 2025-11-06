#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "systems/CameraSystem.h"

struct GameState
{
    enum State : uint8_t
    {
	    PLAYING,
        PAUSED
    };

    inline static Entity currentCameraEntity = INVALID_ENTITY;
    inline static auto currentState = PLAYING;
};

#endif