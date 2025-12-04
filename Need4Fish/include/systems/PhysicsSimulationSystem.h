#ifndef PHYSICS_SIMULATION_SYSTEM_H
#define PHYSICS_SIMULATION_SYSTEM_H

#include "System.h"
#include "physicsEngine/WaterCollector.h"

class PhysicsSimulationSystem : public System
{
public:
	PhysicsSimulationSystem();

private:
	static constexpr double TARGET_UPDATES_PER_SECOND = 60.0;
	static constexpr double PHYSICS_UPDATE_RATE = 1.0 / TARGET_UPDATES_PER_SECOND;
	static constexpr float WATER_LEVEL_Y = 2225.59f;

	WaterCollector waterCollector;

	// Water parameters
	JPH::RVec3 surfacePoint;
	JPH::AABox waterBox;

	void Update(double, EntityManager& entityManager) override;

	static void UpdateControllables(EntityManager& entityManager);
	static void UpdateTransforms(EntityManager& entityManager);
	static void UpdateNPCs(EntityManager& entityManager);
	
	void UpdatePhysics();

	// Pour ajustement vers le yaw et le pitch visés
	static void RotateTowardsCameraDirection(RigidBody& rigidBody, const Camera& camera, JPH::Vec3 forward, float inputRoll);

	// Pour effet de roulement sur le côté
	static float GetTargetRoll(float yawDiff, float inputRoll);
};

#endif
