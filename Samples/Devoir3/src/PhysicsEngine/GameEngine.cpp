#include "pch.h"
#include "GameEngine.h"

#include "ecs/Components.h"
#include "PhysicsEngine/systems/JoltSystem.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Mesh.h"

void GameEngine::Run()
{
	bool shouldContinue = true;
	DWORD prevTime = GetTickCount();

	while (shouldContinue)
	{
		const DWORD frameStartTime = GetTickCount();

		const double elapsedTime = (frameStartTime - prevTime) / 1000.0;
		prevTime = frameStartTime;

		// End the loop if Windows want to terminate the program (+ process messages)
		shouldContinue = WindowsApplication::ProcessWindowsMessages();

		UpdatePhysics();
		UpdateTransforms();
		RenderScene(elapsedTime);

		WaitBeforeNextFrame(frameStartTime);
	}
}

void GameEngine::UpdatePhysics()
{
	// Update physics
	constexpr int collisionSteps = 1;
	JoltSystem::GetPhysicSystem().Update(PHYSICS_UPDATE_RATE, collisionSteps,
		&JoltSystem::GetTempAllocator(),
		&JoltSystem::GetJobSystem());

	// Apply logic related to the physics simulation results
	for (auto& task : JoltSystem::GetPostStepCallbacks())
		task();

	JoltSystem::GetPostStepCallbacks().clear();
}

void GameEngine::UpdateTransforms()
{
	for (const auto& [entity, transform, rigidBody] : entityManager.View<Transform, RigidBody>())
	{
		// Get jolt transform data
		const JPH::RMat44& joltTransform = rigidBody.body->GetWorldTransform();
		const JPH::Vec3 joltPos = joltTransform.GetTranslation();
		const JPH::Quat joltRot = joltTransform.GetQuaternion();

		transform.position = { joltPos.GetX(), joltPos.GetY(), joltPos.GetZ() };
		transform.rotation = { joltRot.GetX(), joltRot.GetY(), joltRot.GetZ(), joltRot.GetW() };

		// Uses Jolt position and rotation and keep the orginal scale of the transform
		const auto scaleMatrix = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
		const auto rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&transform.rotation));
		const auto translationMatrix = XMMatrixTranslation(transform.position.x, transform.position.y,
			transform.position.z);

		transform.world = scaleMatrix * rotationMatrix * translationMatrix;
	}
}

void GameEngine::RenderScene(const double elapsedTime)
{
	renderSystem.UpdateScene(elapsedTime);

	for (const auto& [entity, transform, mesh] : entityManager.View<Transform, Mesh>())
	{
		renderSystem.Render(mesh, transform);
	}

	renderSystem.Render();
}

void GameEngine::WaitBeforeNextFrame(const DWORD frameStartTime)
{
	const DWORD frameEnd = GetTickCount();
	const DWORD frameDuration = frameEnd - frameStartTime;

	if (frameDuration < FRAME_TIME)
		Sleep(static_cast<DWORD>(FRAME_TIME - frameDuration));
}
