#ifndef PHYSICS_SIMULATION_SYSTEM_H
#define PHYSICS_SIMULATION_SYSTEM_H

#include "System.h"

class PhysicsSimulationSystem : public System
{
public:
	PhysicsSimulationSystem() = default;

private:
	static constexpr double TARGET_UPDATES_PER_SECOND = 60.0;
	static constexpr double PHYSICS_UPDATE_RATE = 1.0 / TARGET_UPDATES_PER_SECOND;

	void Update(double, EntityManager& entityManager) override;

	static void UpdateControllables(EntityManager& entityManager);
	static void UpdateTransforms(EntityManager& entityManager);
	static void UpdatePhysics();

	//Pour ajustement vers le yaw visé
	static void RotateTowardsCameraDirection(RigidBody& rigidBody, const Camera& camera, JPH::Vec3 forward, JPH::Vec3 up);
	static JPH::Vec3 RotateVectorByQuat(const JPH::Quat& q, const JPH::Vec3& v);
};

#endif
