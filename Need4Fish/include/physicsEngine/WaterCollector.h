#ifndef WATER_COLLECTOR_H
#define WATER_COLLECTOR_H

#include "pch.h"
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseQuery.h>

using namespace JPH;

// Broadphase results, will apply buoyancy to any body that intersects with the water volume
class WaterCollector : public CollideShapeBodyCollector
{
public:
	
	WaterCollector() = default;
	WaterCollector(PhysicsSystem* inSystem, RVec3Arg inSurfacePosition, Vec3Arg inSurfaceNormal, float inDeltaTime) : mSystem(inSystem), mSurfacePosition(inSurfacePosition), mSurfaceNormal(inSurfaceNormal), mDeltaTime(inDeltaTime) {}

	virtual void AddHit(const BodyID& inBodyID) override
	{
		BodyLockWrite lock(mSystem->GetBodyLockInterface(), inBodyID);
		Body& body = lock.GetBody();
		if (body.IsActive())
			body.ApplyBuoyancyImpulse(mSurfacePosition, mSurfaceNormal, buoyancy, linearDrag, angularDrag, Vec3::sZero(), mSystem->GetGravity(), mDeltaTime);
	}

private:
	PhysicsSystem* mSystem = &JoltSystem::GetPhysicSystem();
	RVec3 mSurfacePosition = RVec3(0, 0, 0); // default value
	Vec3 mSurfaceNormal = Vec3(0, 1, 0);	 // default value
	float mDeltaTime = 0.01f;				 // default value

	float buoyancy = 1.0f;					 // 1 to cancel weight, < 1 sinks, 1 < floats
	float linearDrag = 0.0f;
	float angularDrag = 0.0f;
};

#endif
