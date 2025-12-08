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

    inline static PostProcessSettings postProcessSettings{};

    static void AddCollision(const std::pair<JPH::BodyID, JPH::BodyID>& collision);
    static std::queue<std::pair<JPH::BodyID, JPH::BodyID>>& GetCollisions();

    inline static Entity currentCameraEntity = INVALID_ENTITY;
    inline static auto currentState = PLAYING;

    inline static float playTime = 0.0f;
    //inline static std::chrono::system_clock::time_point startTime;

    inline static bool isGrowing = false;

    inline static double rDeltaTime = 0.0;
    inline static DirectX::XMFLOAT3 colorTint = {0, 0, 0}; // color added as post process
    inline static DirectionalLight dirLight
    {
        .ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f),
        .specular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),

        .direction = DirectX::XMFLOAT3(-0.5f, -1.0f, 0.5f),
        .pad = 0.0f
    };

    inline static UINT meteoriteMeshIndice = UINT32_MAX;

private:
    inline static std::queue<std::pair<JPH::BodyID, JPH::BodyID>> detectedCollisions;
    inline static std::mutex pendingMutex;
};

#endif