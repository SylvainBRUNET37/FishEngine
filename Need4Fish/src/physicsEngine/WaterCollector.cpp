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
		if (body.GetObjectLayer() == Layers::MOVING_DECOR)
		{
			body.ApplyBuoyancyImpulse(mSurfacePosition, mSurfaceNormal, decorBuoyancy, linearDrag, angularDrag,
				Vec3::sZero(), JoltSystem::GetPhysicSystem().GetGravity(), mDeltaTime);
		}
		else
		{
			body.ApplyBuoyancyImpulse(mSurfacePosition, mSurfaceNormal, fishBuoyancy, linearDrag, angularDrag,
			                          Vec3::sZero(), JoltSystem::GetPhysicSystem().GetGravity(), mDeltaTime);
		}
	}
}
