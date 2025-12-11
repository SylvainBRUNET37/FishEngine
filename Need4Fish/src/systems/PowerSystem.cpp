#include "pch.h"
#include "systems/PowerSystem.h"

#include "GameState.h"
#include "physicsEngine/JoltSystem.h"

using namespace std;

void PowerSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	// Update timers and apply global powers
	for (auto& globalPower : activeGlobalPowers)
	{
		ApplyPower(globalPower, entityManager);
		globalPower.timeWithEffect += deltaTime;
	}

	// Remove expired powers and disable their effect
	erase_if(activeGlobalPowers, [&](const PowerSource& powerSource)
	{
		if (powerSource.timeWithEffect >= powerSource.effectDuration)
		{
			DisablePower(powerSource, entityManager);
			return true;
		}

		return false;
	});
}

void PowerSystem::ApplyPower(const PowerSource& powerSource, EntityManager& entityManager)
{
	switch (powerSource.power)
	{
	case Power::Drug:
		GameState::postProcessSettings.enableChromaticAberration = 1;
		break;
	case Power::Floats:
		ApplyLayer(powerSource.target, Layers::FLOATS, entityManager);
		break;
	case Power::Sink:
		ApplyLayer(powerSource.target, Layers::SINKS, entityManager);
		break;
	}
}

void PowerSystem::DisablePower(const PowerSource& powerSource, EntityManager& entityManager)
{
	switch (powerSource.power)
	{
	case Power::Drug:
		GameState::postProcessSettings.enableChromaticAberration = 0;
		break;
	case Power::Floats:
		ApplyLayer(powerSource.target, Layers::MOVING, entityManager);
		break;
	case Power::Sink:
		ApplyLayer(powerSource.target, Layers::MOVING, entityManager);
		break;
	}
}

void PowerSystem::ApplyLayer(const Entity targetEntity, const JPH::ObjectLayer layer, EntityManager& entityManager)
{
	if (targetEntity != INVALID_ENTITY && entityManager.HasComponent<RigidBody>(targetEntity))
	{
		static auto& bodyInterface = JoltSystem::GetBodyInterface();
		const auto& targetRigidBody = entityManager.Get<RigidBody>(targetEntity);

		bodyInterface.SetObjectLayer(targetRigidBody.body->GetID(), layer);
	}
}

void PowerSystem::ResetPowers(EntityManager& entityManager)
{
	for (const auto& powerSource : activeGlobalPowers) 
		DisablePower(powerSource, entityManager);

	activeGlobalPowers.clear();
}
