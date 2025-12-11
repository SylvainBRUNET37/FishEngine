#ifndef MATHS_UTILS_H
#define MATHS_UTILS_H

#include <DirectXMath.h>

class EntityManager;
struct Entity;

namespace MathsUtils
{
	[[nodiscard]] float RandomBetween(float min, float max);
    [[nodiscard]] DirectX::XMFLOAT3 RandomPosInSquare(DirectX::XMFLOAT3 squareCenterPosition, const DirectX::XMFLOAT3& halfExtends);
    [[nodiscard]] bool IsInsideAABB(
        const DirectX::XMFLOAT3& point,
        const DirectX::XMFLOAT3& center,
        const DirectX::XMFLOAT3& halfExtends);

    void TurnFishAround(EntityManager& entityManager, const Entity& entity);

    [[nodiscard]] float GetDistanceBetweenBodies(const JPH::Body* bodyA, const JPH::Body* bodyB);
}

#endif