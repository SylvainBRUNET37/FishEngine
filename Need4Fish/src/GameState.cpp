#include "pch.h"
#include "GameState.h"

void GameState::AddCollision(const std::pair<JPH::BodyID, JPH::BodyID>& collision)
{
    std::lock_guard lock(pendingMutex);
    detectedCollisions.emplace(collision);
}

std::queue<std::pair<JPH::BodyID, JPH::BodyID>>& GameState::GetCollisions()
{
    return detectedCollisions;
}
