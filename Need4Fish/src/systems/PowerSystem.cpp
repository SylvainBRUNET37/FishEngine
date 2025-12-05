#include "pch.h"
#include "systems/PowerSystem.h"

#include "GameState.h"

using namespace std;

void PowerSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	// Update timers and apply global powers
	for (auto& globalPower : activeGlobalPowers)
	{
		ApplyPower(globalPower.power);
		globalPower.timeWithEffect += deltaTime;
	}

	// Remove expired powers and disable their effect
	erase_if(activeGlobalPowers, [](const PowerSource& powerSource)
	{
		if (powerSource.timeWithEffect >= powerSource.effectDuration)
		{
			DisablePower(powerSource.power);
			return true;
		}

		return false;
	});
}

void PowerSystem::ApplyPower(const Power power)
{
	switch (power)
	{
	case Power::Drug:
		GameState::postProcessSettings.enableChromaticAberration = 1;
		break;
	}
}

void PowerSystem::DisablePower(const Power power)
{
	switch (power)
	{
	case Power::Drug:
		GameState::postProcessSettings.enableChromaticAberration = 0;
		break;
	}
}

void PowerSystem::ResetPowers()
{
	for (const auto& powerSource : activeGlobalPowers) 
		DisablePower(powerSource.power);

	activeGlobalPowers.clear();
}
