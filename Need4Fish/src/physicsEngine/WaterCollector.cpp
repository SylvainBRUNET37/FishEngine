#include "pch.h"
#include "physicsEngine/WaterCollector.h"

#include "physicsEngine/JoltSystem.h"

using namespace JPH;

void WaterCollector::AddHit(const BodyID& inBodyID)

{
	const BodyLockWrite lock(JoltSystem::GetPhysicSystem().GetBodyLockInterface(), inBodyID);
	Body& body = lock.GetBody();
	if (body.IsActive())
		body.ApplyBuoyancyImpulse(mSurfacePosition, mSurfaceNormal, buoyancy, linearDrag, angularDrag,
			Vec3::sZero(), JoltSystem::GetPhysicSystem().GetGravity(), mDeltaTime);
}
