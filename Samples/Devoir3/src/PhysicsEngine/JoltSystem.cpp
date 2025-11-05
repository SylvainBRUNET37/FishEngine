#include "pch.h"
#include "PhysicsEngine/JoltSystem.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

#include "PhysicsEngine/utils/JoltUtils.h"

using namespace JPH;

void JoltSystem::Init()
{
	RegisterDefaultAllocator();

	Trace = JoltUtils::TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = JoltUtils::AssertFailedImpl;)

	Factory::sInstance = new Factory();

	RegisterTypes();

	physicsSystem = std::make_unique<PhysicsSystem>();
}

JoltSystem::~JoltSystem()
{
	UnregisterTypes();
	delete Factory::sInstance;
	Factory::sInstance = nullptr;
}
