#ifndef PHYSICS_SIMULATION_SYSTEM_H
#define PHYSICS_SIMULATION_SYSTEM_H

#include "System.h"

class PhysicsSimulationSystem : public System
{
public:
	PhysicsSimulationSystem() = default;

private:
	void Update(double, EntityManager& entityManager) override;

	static void UpdateTransforms(EntityManager& entityManager);
	static void UpdatePhysics();
};

#endif
