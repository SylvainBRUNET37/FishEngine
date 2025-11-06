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

void PhysicsSimulationSystem::RotateTowardsCameraDirection(
	RigidBody& rigidBody,
	const Camera& camera,
	JPH::Vec3 forward,
	JPH::Vec3 /*up*/)
{
	const JPH::Vec3 worldUp(0.0f, 1.0f, 0.0f);

	// Stockage du pitch et roll entre les frames
	static float currentPitch = 0.0f;
	static float currentRoll = 0.0f;

	// Calcul des angles actuels
	const float currentYaw = atan2f(forward.GetX(), forward.GetZ());

	// Différence entre les angles
	auto shortestAngle = [](float from, float to) {
		float d = to - from;
		while (d > JPH::JPH_PI) d -= 2.0f * JPH::JPH_PI;
		while (d < -JPH::JPH_PI) d += 2.0f * JPH::JPH_PI;
		return d;
		};

	float yawDiff = shortestAngle(currentYaw, camera.targetYaw);
	float pitchDiff = shortestAngle(currentPitch, camera.targetPitch);

	// Limiter vitesse rotation
	constexpr float maxYawStep = 0.02f;
	constexpr float maxPitchStep = 0.02f;
	float yawStep = std::clamp(yawDiff, -maxYawStep, maxYawStep);
	float pitchStep = std::clamp(pitchDiff, -maxPitchStep, maxPitchStep);

	float newYaw = currentYaw + yawStep;
	currentPitch += pitchStep; // interpolation progressive du pitch

	// Roll limité
	constexpr float maxRollAngle = 0.2f;
	float targetRoll = std::clamp(-yawDiff * 0.5f, -maxRollAngle, maxRollAngle);

	// Progressivement vers le roll cible
	constexpr float rollInterpSpeed = 0.05f;
	float rollDelta = targetRoll - currentRoll;
	rollDelta = std::clamp(rollDelta, -rollInterpSpeed, rollInterpSpeed);
	currentRoll += rollDelta;

	// Quats de rotation
	JPH::Quat yawQuat = JPH::Quat::sRotation(worldUp, newYaw);
	JPH::Vec3 right = yawQuat * JPH::Vec3::sAxisX();
	JPH::Quat pitchQuat = JPH::Quat::sRotation(right, currentPitch);

	// roll autour du forward après yaw/pitch
	JPH::Vec3 forwardAfterYawPitch = (pitchQuat * yawQuat) * JPH::Vec3::sAxisZ();
	JPH::Quat rollQuat = JPH::Quat::sRotation(forwardAfterYawPitch, currentRoll);

	// Appliquer rotation finale
	JPH::Quat finalRotation = (rollQuat * pitchQuat * yawQuat).Normalized();
	JoltSystem::GetBodyInterface().SetRotation(
		rigidBody.body->GetID(),
		finalRotation,
		JPH::EActivation::Activate
	);
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
