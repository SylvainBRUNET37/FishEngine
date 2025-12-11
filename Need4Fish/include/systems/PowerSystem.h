#ifndef POWER_SYSTEM_H
#define POWER_SYSTEM_H

#include "System.h"

class PowerSystem : public System
{
public:
	static void AddEffect(PowerSource powerSource) { activeGlobalPowers.emplace_back(powerSource); }
	static void ResetPowers(EntityManager& entityManager);

private:
	inline static std::vector<PowerSource> activeGlobalPowers;

	void Update(double deltaTime, EntityManager& entityManager) override;

	static void ApplyPower(const PowerSource& powerSource, EntityManager& entityManager);
	static void DisablePower(const PowerSource& powerSource, EntityManager& entityManager);

	static void ApplyLayer(Entity targetEntity, JPH::ObjectLayer layer, EntityManager& entityManager);
};

#endif
