#include "pch.h"
#include "PhysicsEngine/JoltSystem.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

#include "PhysicsEngine/utils/JoltUtils.h"

using namespace JPH;

JoltSystem::JoltSystem()
{
	RegisterDefaultAllocator();

	Trace = JoltUtils::TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = JoltUtils::AssertFailedImpl;)

	Factory::sInstance = new Factory();

	RegisterTypes();

	physicsSystem = std::make_unique<PhysicsSystem>();
	physicsSystem->SetGravity(Vec3(0, -1'300, 0));

#ifndef NDEBUG
	AssertFailed = JoltUtils::AssertFailedImpl;
#endif

	physicsSystem->Init(1024, 0, 1024, 1024,
		broadPhaseLayerInterface, objectVsBroadPhaseLayerFilter, objectLayerPairFilter);

	PhysicsSettings settings = physicsSystem->GetPhysicsSettings();
	settings.mMinVelocityForRestitution = 0.01f;
	physicsSystem->SetPhysicsSettings(settings);

	physicsSystem->SetBodyActivationListener(&bodyActivationListener);
	physicsSystem->SetContactListener(&contactListener);
}

JoltSystem::~JoltSystem()
{
	UnregisterTypes();
	delete Factory::sInstance;
	Factory::sInstance = nullptr;
}
