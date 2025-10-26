#ifndef SCENE_H
#define SCENE_H

#include "PhysicsEngine/pch.h"
#include "PhysicsEngine/systems/JoltSystem.h"
#include <Jolt/Physics/Body/BodyID.h>

using namespace JPH;
using namespace JPH::literals;


// TODO: move this
class PhysicsModel {
	//Model model; // replace with transform
	BodyID id;
public:
	BodyID GetId() { return id; }
};


class Scene
{
public:

	~Scene() {
		auto& bodyInterface = JoltSystem::GetBodyInterface();

		for (auto staticObject : staticObjects)
		{
			bodyInterface.RemoveBody(staticObject);
			bodyInterface.DestroyBody(staticObject);
		}

		for (auto& model : models)
		{
			bodyInterface.RemoveBody(model.GetId());
			bodyInterface.DestroyBody(model.GetId());
		}
	}

	void update()
	{
		constexpr int collisionSteps = 1;
		JoltSystem::GetPhysicSystem().Update(PHYSICS_UPDATE_RATE, collisionSteps,
			&JoltSystem::GetTempAllocator(),
			&JoltSystem::GetJobSystem());

		for (auto& task : JoltSystem::GetPostStepCallbacks())
			task();

		// TODO
		// update models transforms

		JoltSystem::GetPostStepCallbacks().clear();
	}

private:
	static constexpr double PHYSICS_UPDATE_RATE = 1.0f / 60.0f;
	static constexpr double TARGET_FPS = 60.0;
	static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;

	std::vector<PhysicsModel> models;
	std::vector<JPH::BodyID> staticObjects;
};

#endif
