#include "pch.h"
#include "systems/PhysicsSimulationSystem.h"

#include "PhysicsEngine/JoltSystem.h"
#include "GameState.h"
#include "../../../RenderingEngine/src/rendering/application/WindowsApplication.cpp"
#include "PhysicsEngine/layers/Layers.h"
#include "PhysicsEngine/layers/BroadPhaseLayers.h"

using namespace DirectX;

void PhysicsSimulationSystem::Init() {

	// Water init
	surfacePoint = RVec3(0, 55, 0);
	waterBox = AABox(
		-Vec3(300.0f, 300.0f, 300.0f),
		Vec3(300.0f, 55.0f, 300.0f)
	);
	waterBox.Translate(Vec3(surfacePoint));
	// End Water init

	waterCollector = WaterCollector(&JoltSystem::GetPhysicSystem(), surfacePoint, Vec3::sAxisY(), PHYSICS_UPDATE_RATE);
}

void PhysicsSimulationSystem::Update(double, EntityManager& entityManager)
{
	if (GameState::currentState != GameState::PAUSED)
	{
		UpdateControllables(entityManager);
		UpdatePhysics();
		UpdateTransforms(entityManager);
	}
}

void PhysicsSimulationSystem::UpdateControllables(EntityManager& entityManager)
{
	//R�cup�rer la cam�ra pour le targetYaw
	Camera* activeCamera = nullptr;
	for (const auto& [entity, camera] : entityManager.View<Camera>())
	{
		activeCamera = &camera;
		break;
	}	

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

			if (mouseWheelDelta != 0) {
				activeCamera->distance -= mouseWheelDelta * 0.01f * activeCamera->zoomSpeed;
				activeCamera->distance = std::clamp(activeCamera->distance, activeCamera->minDistance, activeCamera->maxDistance);
				mouseWheelDelta = 0; // reset
			}
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
			float inputRoll = 0.2f;
			UpdateRoll(rigidBody, 0.0f, inputRoll);
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			newSpeed = newSpeed + 1.0f * right;
			speedChanged = true;
			float inputRoll = -0.2f;
			UpdateRoll(rigidBody, 0.0f, inputRoll);
		}

		if (speedChanged)
		{
			if (newSpeed.Length() > controllable.maxSpeed) newSpeed = newSpeed.Normalized();
			JoltSystem::GetBodyInterface().SetLinearVelocity(rigidBody.body->GetID(), newSpeed);
		}
		/*// Rotation manuelle retirée
		bool rotatesPositive = GetAsyncKeyState('Q') & 0x8000;
		if (rotatesPositive || GetAsyncKeyState('E') & 0x8000)
		{
			JPH::Quat delta = JPH::Quat::sRotation(up, .05f * (1 - 2 * !rotatesPositive)); // theta = 10
			JoltSystem::GetBodyInterface().SetRotation(
				rigidBody.body->GetID(),
				(rigidBody.body->GetRotation() * delta).Normalized(),
				JPH::EActivation::Activate);
		}*/
	}
}

void PhysicsSimulationSystem::UpdatePhysics()
{
	JoltSystem::GetPhysicSystem().GetBroadPhaseQuery().CollideAABox(
		waterBox,
		waterCollector,
		SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::MOVING),
		SpecifiedObjectLayerFilter(Layers::MOVING)
	);

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

	// Quats de rotation
	JPH::Quat yawQuat = JPH::Quat::sRotation(worldUp, newYaw);
	JPH::Vec3 right = yawQuat * JPH::Vec3::sAxisX();
	JPH::Quat pitchQuat = JPH::Quat::sRotation(right, currentPitch);

	// Appliquer rotation finale
	JPH::Quat finalRotation = (pitchQuat * yawQuat).Normalized();
	JoltSystem::GetBodyInterface().SetRotation(
		rigidBody.body->GetID(),
		finalRotation,
		JPH::EActivation::Activate
	);

	// Roll avec la souris
	UpdateRoll(rigidBody, yawDiff, 0.0f);
}


void PhysicsSimulationSystem::UpdateTransforms(EntityManager& entityManager)
{
	for (const auto& [entity, transform, rigidBody] : entityManager.View<Transform, RigidBody>())
	{
		// Get jolt transform data
		const JPH::RMat44& joltTransform = rigidBody.body->GetWorldTransform();
		const JPH::Vec3 joltPos = joltTransform.GetTranslation();
		const JPH::Quat joltRot = joltTransform.GetQuaternion();

		transform.position = {joltPos.GetX(), joltPos.GetY(), joltPos.GetZ()};
		transform.rotation = {joltRot.GetX(), joltRot.GetY(), joltRot.GetZ(), joltRot.GetW()};

		// Uses Jolt position and rotation and keep the orginal scale of the transform
		const auto scaleMatrix = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
		const auto rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&transform.rotation));
		const auto translationMatrix = XMMatrixTranslation(transform.position.x, transform.position.y,
		                                                   transform.position.z);

		transform.world = scaleMatrix * rotationMatrix * translationMatrix;
	}
}

void PhysicsSimulationSystem::UpdateRoll(RigidBody& rigidBody, float yawDiff, float inputRoll = 0.0f)
{
	constexpr float maxRollAngle = 0.2f;
	static float currentRoll = 0.0f;

	// Roll cible
	float targetRoll = std::clamp(-yawDiff * 0.5f + inputRoll, -maxRollAngle, maxRollAngle);

	// Progressif
	constexpr float rollInterpSpeed = 0.05f;
	float rollDelta = targetRoll - currentRoll;
	rollDelta = std::clamp(rollDelta, -rollInterpSpeed, rollInterpSpeed);
	currentRoll += rollDelta;

	// Pour appliquer roll après yaw/pitch
	const JPH::Quat currentRotation = rigidBody.body->GetRotation();
	// forward local après yaw/pitch
	JPH::Vec3 forward = currentRotation * JPH::Vec3::sAxisZ();
	JPH::Quat rollQuat = JPH::Quat::sRotation(forward, currentRoll);

	// Appliquer rotation finale
	JPH::Quat finalRotation = (rollQuat * currentRotation).Normalized();
	JoltSystem::GetBodyInterface().SetRotation(
		rigidBody.body->GetID(),
		finalRotation,
		JPH::EActivation::Activate
	);
}
