#ifndef JOLT_INITIALISER_H
#define JOLT_INITIALISER_H

#include <cassert>

#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "PhysicsEngine/layers/BroadPhaseLayerInterfaceImpl.h"
#include "PhysicsEngine/layers/ObjectLayerPairFilterImpl.h"
#include "PhysicsEngine/layers/ObjectVsBroadPhaseLayerFilterImpl.h"
#include "PhysicsEngine/listeners/ContactListenerImpl.h"
#include "PhysicsEngine/listeners/BodyActivationListenerImpl.h"

class JoltSystem
{
public:
	using PostStepCallback = std::function<void()>;
	using PostStepCallbacks = std::vector<PostStepCallback>;

	JoltSystem();
	~JoltSystem();

	JoltSystem(const JoltSystem&) = delete;
	JoltSystem& operator=(const JoltSystem&) = delete;
	JoltSystem(JoltSystem&&) = delete;
	JoltSystem& operator=(JoltSystem&&) = delete;

	static JPH::TempAllocatorImpl& GetTempAllocator()
	{
		static JPH::TempAllocatorImpl tempAllocator{TEMP_ALLOCATOR_SIZE};

		return tempAllocator;
	}

	static JPH::JobSystemThreadPool& GetJobSystem()
	{
		static JPH::JobSystemThreadPool jobSystem
		{
			JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, static_cast<int>(std::thread::hardware_concurrency() - 1)
		};

		return jobSystem;
	}

	static JPH::PhysicsSystem& GetPhysicSystem()
	{
		assert(physicsSystem != nullptr && "PhysicsSystem cannot be null");

		return *physicsSystem;
	}

	static JPH::BodyInterface& GetBodyInterface()
	{
		assert(physicsSystem != nullptr && "PhysicsSystem cannot be null");

		return physicsSystem->GetBodyInterface();
	}

	// Should be cleared after calling callbacks
	static PostStepCallbacks& GetPostStepCallbacks()
	{
		return postStepCallbacks;
	}

	static void AddPostStepCallback(const PostStepCallback& callback)
	{
		postStepCallbacks.emplace_back(callback);
	}

private:
	static constexpr size_t TEMP_ALLOCATOR_SIZE_MB = 10;
	static constexpr size_t TEMP_ALLOCATOR_SIZE = TEMP_ALLOCATOR_SIZE_MB * 1024 * 1024;

	inline static std::unique_ptr<JPH::PhysicsSystem> physicsSystem = nullptr;

	// Action to do after Jolt physics step because bodies are locked during it
	inline static PostStepCallbacks postStepCallbacks{};

	BroadPhaseLayerInterfaceImpl broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl objectLayerPairFilter;
	ContactListenerImpl contactListener;
	BodyActivationListenerImpl bodyActivationListener;
};

#endif
