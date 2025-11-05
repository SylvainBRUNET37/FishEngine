#include "pch.h"
#include "systems/PhysicsSimulationSystem.h"

#include "PhysicsEngine/JoltSystem.h"
#include <GameState.h>

using namespace DirectX;

void PhysicsSimulationSystem::Update(double, EntityManager& entityManager)
{
	UpdateControllables(entityManager);
	UpdatePhysics();
	UpdateTransforms(entityManager);
}

void PhysicsSimulationSystem::UpdateControllables(EntityManager& entityManager)
{
	// Toggle pause avec ESC
	static bool escWasPressed = false;
	bool escIsPressed = GetAsyncKeyState(VK_ESCAPE) & 0x8000;
	if (escIsPressed && !escWasPressed)
	{
		ChangePauseStatus();
	}
	escWasPressed = escIsPressed;

	//Récupérer la caméra pour le targetYaw
	Camera* activeCamera = nullptr;
	for (const auto& [entity, camera] : entityManager.View<Camera>())
	{
		activeCamera = &camera;
		break;
	}

	if (!GameState::isPaused) {
		for (const auto& [entity, rigidBody, controllable] : entityManager.View<RigidBody, Controllable>())
		{
			const auto& transform = rigidBody.body->GetWorldTransform();
			JPH::Vec3 right = transform.GetColumn3(0).Normalized();
			JPH::Vec3 up = transform.GetColumn3(1).Normalized();
			JPH::Vec3 forward = transform.GetColumn3(2).Normalized();

			// Rotation progressive vers la direction de la caméra
			if (activeCamera)
			{
				RotateTowardsCameraDirection(rigidBody, *activeCamera, forward, up);
			}

			JPH::Vec3 currentSpeed = JoltSystem::GetBodyInterface().GetLinearVelocity(rigidBody.body->GetID());
			JPH::Vec3 newSpeed = currentSpeed;
			bool speedChanged = false;



			if (GetAsyncKeyState('W') & 0x8000)
			{
				newSpeed = newSpeed + 1.0f * forward;
				speedChanged = true;
			}
			if (GetAsyncKeyState('S') & 0x8000)
			{
				newSpeed = newSpeed - 1.0f * forward;
				speedChanged = true;
			}
			if (GetAsyncKeyState('D') & 0x8000)
			{
				newSpeed = newSpeed - 1.0f * right;
				speedChanged = true;
			}
			if (GetAsyncKeyState('A') & 0x8000)
			{
				newSpeed = newSpeed + 1.0f * right;
				speedChanged = true;
			}

			if (speedChanged)
			{
				if (newSpeed.Length() > controllable.maxSpeed) newSpeed = newSpeed.Normalized();
				JoltSystem::GetBodyInterface().SetLinearVelocity(rigidBody.body->GetID(), newSpeed);
			}

			// Rotation manuelle -> on pourrait retirer
			bool rotatesPositive = GetAsyncKeyState('Q') & 0x8000;
			if (rotatesPositive || GetAsyncKeyState('E') & 0x8000)
			{
				JPH::Quat delta = JPH::Quat::sRotation(up, .05f * (1 - 2 * !rotatesPositive)); // theta = 10
				JoltSystem::GetBodyInterface().SetRotation(
					rigidBody.body->GetID(),
					(rigidBody.body->GetRotation() * delta).Normalized(),
					JPH::EActivation::Activate);
			}
		}
	}
	
}

void PhysicsSimulationSystem::UpdatePhysics()
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

void PhysicsSimulationSystem::RotateTowardsCameraDirection(RigidBody& rigidBody, const Camera& camera,
	const JPH::Vec3& forward, const JPH::Vec3& up)
{
	// Yaw actuel du poisson
	const float currentYaw = atan2f(forward.GetX(), forward.GetZ());
	const float targetYaw = camera.targetYaw;

	//Différence d'angle
	float angleDiff = targetYaw - currentYaw;

	// Normaliser
	while (angleDiff > JPH::JPH_PI) angleDiff -= 2.0f * JPH::JPH_PI;
	while (angleDiff < -JPH::JPH_PI) angleDiff += 2.0f * JPH::JPH_PI;

	// Rotation progressive
	constexpr float rotationSpeed = 0.02f;
	constexpr float rotationThreshold = 0.05f; //"Deadzone"

	const float rotationStep = std::clamp(angleDiff, -rotationSpeed, rotationSpeed);

	if (std::abs(angleDiff) > rotationThreshold)
	{
		const JPH::Quat delta = JPH::Quat::sRotation(up, rotationStep);
		JoltSystem::GetBodyInterface().SetRotation(
			rigidBody.body->GetID(),
			(rigidBody.body->GetRotation() * delta).Normalized(),
			JPH::EActivation::Activate);
	}
}

void PhysicsSimulationSystem::UpdateTransforms(EntityManager& entityManager)
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
