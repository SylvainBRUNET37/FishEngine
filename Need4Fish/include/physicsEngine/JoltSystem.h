#ifndef JOLT_INITIALISER_H
#define JOLT_INITIALISER_H

#include <functional>
#include <cassert>

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

class JoltSystem
{
public:
	using PostStepCallback = std::function<void()>;
	using PostStepCallbacks = std::vector<PostStepCallback>;

	JoltSystem(const JoltSystem&) = delete;
	JoltSystem& operator=(const JoltSystem&) = delete;
	JoltSystem(JoltSystem&&) = delete;
	JoltSystem& operator=(JoltSystem&&) = delete;

	static void Init();

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
	inline static PostStepCallbacks postStepCallbacks{};

	~JoltSystem();
};

#endif
