#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "pch.h"

#include <mutex>

#include "systems/CameraSystem.h"
#include <queue>

#include "rendering/graphics/lights/DirectionalLight.h"
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

    inline static Entity playerEntity = INVALID_ENTITY;

    inline static PostProcessSettings postProcessSettings{};

    static void AddCollision(const std::pair<JPH::BodyID, JPH::BodyID>& collision);
    static std::queue<std::pair<JPH::BodyID, JPH::BodyID>>& GetCollisions();

    inline static Entity currentCameraEntity = INVALID_ENTITY;
    inline static auto currentState = PLAYING;

    inline static float playTime{};
    inline static double rDeltaTime{};
    inline static bool isGrowing{};
    inline static double apocalipseTime{};
    
    inline static DirectX::XMFLOAT3 colorTint{}; // color added as post process
    inline static DirectionalLight dirLight{};

    inline static UINT meteoriteMeshIndice = UINT32_MAX;

    static void Init();

private:
    inline static std::queue<std::pair<JPH::BodyID, JPH::BodyID>> detectedCollisions;
    inline static std::mutex pendingMutex;
};

#endif