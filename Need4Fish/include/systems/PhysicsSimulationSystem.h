#ifndef PHYSICS_SIMULATION_SYSTEM_H
#define PHYSICS_SIMULATION_SYSTEM_H

#include "System.h"
#include "physicsEngine/WaterCollector.h"

class PhysicsSimulationSystem : public System
{
public:
	PhysicsSimulationSystem() = default;
	void Init();

private:
	static constexpr double TARGET_UPDATES_PER_SECOND = 60.0;
	static constexpr double PHYSICS_UPDATE_RATE = 1.0 / TARGET_UPDATES_PER_SECOND;

	WaterCollector waterCollector;
	// Water parameters
	RVec3 surfacePoint; // = RVec3(0, 55, 0); // TODO: move this blud
	AABox waterBox; // = AABox(
	//	-Vec3(300.0f, 300.0f, 300.0f),
	//	Vec3(300.0f, 55.0f, 300.0f)
	//);

	void Update(double, EntityManager& entityManager) override;

	static void UpdateControllables(EntityManager& entityManager);
	static void UpdateTransforms(EntityManager& entityManager);
	void UpdatePhysics();

	//Pour ajustement vers le yaw et le pitch visés
	static void RotateTowardsCameraDirection(RigidBody& rigidBody, const Camera& camera, JPH::Vec3 forward, JPH::Vec3 up);

	//Pour effet de roulement sur le côté
	static void UpdateRoll(RigidBody& rigidBody, float yawDiff, float inputRoll);
};

#endif
