#include "pch.h"
#include "utils/MathsUtils.h"

#include <numbers>
#include <random>
#include <Jolt/Geometry/ConvexSupport.h>
#include <Jolt/Geometry/GJKClosestPoint.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>

#include "physicsEngine/JoltSystem.h"

using namespace std;
using namespace DirectX;
using namespace JPH;

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
	const Vec3 axis = rigidBody.body->GetWorldTransform().GetColumn3(1).Normalized(); // up

	static constexpr float angle = std::numbers::pi_v<float>;

	// Vérifie l'angle pour éviter les problemes
	if (angle > 0.0001f)
	{
		bodyInterface.SetPositionAndRotation(
			rigidBody.body->GetID(),
			rigidBody.body->GetPosition(),
			(Quat::sRotation(axis, angle) * rigidBody.body->GetRotation()).Normalized(),
			EActivation::Activate
		);
	}
}

static float ComputeBoundingRadius(const Shape* shape)
{
	const AABox bounds = shape->GetLocalBounds();
	const Vec3 halfExtents = bounds.GetExtent();

	return halfExtents.Length();
}

float MathsUtils::GetDistanceBetweenBodies(const Body* bodyA, const Body* bodyB)
{
	const Shape* shapeA = bodyA->GetShape();
	const Shape* shapeB = bodyB->GetShape();

	if (!shapeA || !shapeB)
		return FLT_MAX;

	const auto bodyPosA = bodyA->GetPosition();
	const auto bodyPosB = bodyB->GetPosition();

	const float dx = bodyPosA.GetX() - bodyPosB.GetX();
	const float dy = bodyPosA.GetY() - bodyPosB.GetY();
	const float dz = bodyPosA.GetZ() - bodyPosB.GetZ();

	const float distance = dx * dx + dy * dy + dz * dz;

	const float rA = ComputeBoundingRadius(shapeA);
	const float rB = ComputeBoundingRadius(shapeB);

	return distance - rA * rA - rB * rB;
}

