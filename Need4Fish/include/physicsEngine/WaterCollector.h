#ifndef WATER_COLLECTOR_H
#define WATER_COLLECTOR_H

#include "pch.h"
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseQuery.h>

#include "JoltSystem.h"

// Broadphase results, will apply buoyancy to any body that intersects with the water volume
class WaterCollector : public JPH::CollideShapeBodyCollector
{
public:
	WaterCollector() = default;

	WaterCollector(JPH::RVec3Arg inSurfacePosition, JPH::Vec3Arg inSurfaceNormal, const float inDeltaTime) :
		mSurfacePosition(inSurfacePosition), mSurfaceNormal(inSurfaceNormal), mDeltaTime(inDeltaTime)
	{
	}

	void AddHit(const JPH::BodyID& inBodyID) override;

private:
	JPH::RVec3 mSurfacePosition = JPH::RVec3(0, 0, 0); // default value
	JPH::Vec3 mSurfaceNormal = JPH::Vec3(0, 1, 0); // default value
	float mDeltaTime = 0.01f; // default value

	float fishBuoyancy = 1.0f; // 1 to cancel weight, < 1 sinks, 1 < floats
	float decorBuoyancy = 0.01f;
	float linearDrag = 0.0f;
	float angularDrag = 0.0f;
};

#endif
