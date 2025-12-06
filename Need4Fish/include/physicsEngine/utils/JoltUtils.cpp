#include "pch.h"
#include "JoltUtils.h"
#include "physicsEngine/JoltSystem.h"
#include <numbers>

using namespace JPH;

void JoltUtils::TurnFishAround(EntityManager& entityManager, const Entity& entity)
{
	auto& bodyInterface = JoltSystem::GetBodyInterface();
	auto& rigidBody = entityManager.Get<RigidBody>(entity);
	Vec3 axis = rigidBody.body->GetWorldTransform().GetColumn3(1).Normalized(); // up

	float angle = std::numbers::pi_v<float>;

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
