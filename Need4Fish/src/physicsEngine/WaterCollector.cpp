#include "pch.h"
#include "physicsEngine/WaterCollector.h"

#include "physicsEngine/JoltSystem.h"

using namespace JPH;

void WaterCollector::AddHit(const BodyID& inBodyID)

{
	const BodyLockWrite lock(JoltSystem::GetPhysicSystem().GetBodyLockInterface(), inBodyID);
	Body& body = lock.GetBody();
	if (body.IsActive())
	{
		// Apply gravity to decor objets (rocks, ...)
		float buoyancyValue = fishBuoyancy;
		if (body.GetObjectLayer() == Layers::SINKS || body.GetObjectLayer() == Layers::FLOATS)
			buoyancyValue = (body.GetObjectLayer() == Layers::SINKS) ? sinkingBuoyancy : floatingBuoyancy;

		body.ApplyBuoyancyImpulse(mSurfacePosition, mSurfaceNormal, buoyancyValue, linearDrag, angularDrag,
			                        Vec3::sZero(), JoltSystem::GetPhysicSystem().GetGravity(), mDeltaTime);
	}
}
