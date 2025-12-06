#include "pch.h"
#include "utils/MathsUtils.h"

#include <numbers>
#include <random>
#include <Jolt/Physics/EActivation.h>

#include "physicsEngine/JoltSystem.h"

using namespace std;
using namespace DirectX;

float MathsUtils::RandomBetween(const float min, const float max)
{
	static mt19937 rng{std::random_device{}()};
	uniform_real_distribution dist(min, max);

	return dist(rng);
}

XMFLOAT3 MathsUtils::RandomPosInSquare(XMFLOAT3 squareCenterPosition, const XMFLOAT3& halfExtends)
{
	squareCenterPosition.x += RandomBetween(-halfExtends.x, halfExtends.x);
	squareCenterPosition.y += RandomBetween(-halfExtends.y, halfExtends.y);
	squareCenterPosition.z += RandomBetween(-halfExtends.z, halfExtends.z);

	return squareCenterPosition;
}

bool MathsUtils::IsInsideAABB(const XMFLOAT3& point, const XMFLOAT3& center,
                              const XMFLOAT3& halfExtends)

{
	return point.x >= center.x - halfExtends.x &&
		point.x <= center.x + halfExtends.x &&
		point.y >= center.y - halfExtends.y &&
		point.y <= center.y + halfExtends.y &&
		point.z >= center.z - halfExtends.z &&
		point.z <= center.z + halfExtends.z;
}

void MathsUtils::TurnFishAround(EntityManager& entityManager, const Entity& entity)
{
	auto& bodyInterface = JoltSystem::GetBodyInterface();
	const auto& rigidBody = entityManager.Get<RigidBody>(entity);
	const JPH::Vec3 axis = rigidBody.body->GetWorldTransform().GetColumn3(1).Normalized(); // up

	static constexpr float angle = std::numbers::pi_v<float>;

	// Vérifie l'angle pour éviter les problemes
	if (angle > 0.0001f)
	{
		bodyInterface.SetPositionAndRotation(
			rigidBody.body->GetID(),
			rigidBody.body->GetPosition(),
			(JPH::Quat::sRotation(axis, angle) * rigidBody.body->GetRotation()).Normalized(),
			JPH::EActivation::Activate
		);
	}
}
