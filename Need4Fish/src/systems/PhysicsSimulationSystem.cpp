#include "pch.h"
#include "systems/PhysicsSimulationSystem.h"

#include <ranges>

#include "PhysicsEngine/JoltSystem.h"
#include "GameState.h"
#include "rendering/application/WindowsApplication.h"
#include "PhysicsEngine/layers/Layers.h"
#include "PhysicsEngine/layers/BroadPhaseLayers.h"

using namespace DirectX;
using namespace JPH;
using namespace std;

PhysicsSimulationSystem::PhysicsSimulationSystem()
{
	// Water init
	surfacePoint = RVec3(0, WATER_LEVEL_Y, 0);
	waterBox = AABox(
		Vec3(-6000.0f, -250.0f, -6000),
		Vec3(6000.0f, WATER_LEVEL_Y, 6000.0f)
	);
	// End Water init

	waterCollector = WaterCollector(surfacePoint, Vec3::sAxisY(), PHYSICS_UPDATE_RATE);
}

void PhysicsSimulationSystem::Update(double, EntityManager& entityManager)
{
	if (GameState::currentState == GameState::PLAYING)
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
		Vec3 right = transform.GetColumn3(0).Normalized();
		Vec3 up = transform.GetColumn3(1).Normalized();
		Vec3 forward = transform.GetColumn3(2).Normalized();

		// Pour le roll sur A et D
		static float inputRoll = 0.0f;

		// Rotation progressive vers la direction de la caméra
		if (activeCamera)
		{
			RotateTowardsCameraDirection(rigidBody, *activeCamera, forward, inputRoll);

			if (WindowsApplication::mouseWheelDelta != 0)
			{
				const float oldDistance = activeCamera->distance;

				activeCamera->distance -= WindowsApplication::mouseWheelDelta * 0.01f * activeCamera->zoomSpeed;
				activeCamera->distance = std::clamp(activeCamera->distance, activeCamera->minDistance,
				                                    activeCamera->maxDistance);

				if (oldDistance > activeCamera->minDistance && activeCamera->distance <= activeCamera->minDistance)
				{
					activeCamera->mode = Camera::CameraMode::FIRST_PERSON;
				}
				else if (oldDistance <= activeCamera->minDistance && activeCamera->distance > activeCamera->minDistance)
				{
					activeCamera->mode = Camera::CameraMode::THIRD_PERSON;
				}
				WindowsApplication::mouseWheelDelta = 0; // reset
			}
		}

		Vec3 currentSpeed = JoltSystem::GetBodyInterface().GetLinearVelocity(rigidBody.body->GetID());
		Vec3 newSpeed = Vec3(0.0f, 0.0f, 0.0f);
		bool speedChanged = false;

		// Reset du roll
		inputRoll = 0.0f;

		if (GetAsyncKeyState('W') & 0x8000)
		{
			newSpeed = 10.0f * forward;
			speedChanged = true;
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			newSpeed = - 10.0f * forward;
			speedChanged = true;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			newSpeed = - 5.0f * right;
			speedChanged = true;
			inputRoll = 0.1f;
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			newSpeed = 5.0f * right;
			speedChanged = true;
			inputRoll = -0.1f;
		}

		if (JoltSystem::GetBodyInterface().GetPosition(rigidBody.body->GetID()).GetY() > WATER_LEVEL_Y && newSpeed.GetY() > 0.0f)
			newSpeed.SetY(0.0f);

		const auto theoreticalSpeed = currentSpeed + newSpeed;
		if (speedChanged && (theoreticalSpeed).Length() < controllable.maxSpeed)
			JoltSystem::GetBodyInterface().SetLinearVelocity(rigidBody.body->GetID(), currentSpeed + newSpeed);
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
	constexpr int collisionSteps = 2;
	JoltSystem::GetPhysicSystem().Update(PHYSICS_UPDATE_RATE, collisionSteps,
	                                     &JoltSystem::GetTempAllocator(),
	                                     &JoltSystem::GetJobSystem());
}

void PhysicsSimulationSystem::RotateTowardsCameraDirection(
	RigidBody& rigidBody,
	const Camera& camera,
	Vec3 forward,
	float inputRoll)
{
	const Vec3 worldUp(0.0f, 1.0f, 0.0f);

	// Stockage du pitch et roll entre les frames
	static float currentPitch = 0.0f;

	// Calcul des angles actuels
	const float currentYaw = atan2f(forward.GetX(), forward.GetZ());

	// Pour calculer yawDiff pour le roll
	auto shortestAngle = [](float from, float to)
	{
		float d = to - from;
		while (d > JPH_PI) d -= 2.0f * JPH_PI;
		while (d < -JPH_PI) d += 2.0f * JPH_PI;
		return d;
	};

	float newYaw, newPitch;
	float yawDiff = 0.0f; //Pour les roll

	// En première personne : rotation instantanée
	if (camera.mode == Camera::CameraMode::FIRST_PERSON)
	{
		newYaw = currentYaw + camera.yawOffset;
		newPitch = camera.pitchAngle;
		currentPitch = newPitch; // Pour éviter un saut lors du retour en 3e personne

		yawDiff = camera.yawOffset;
	}
	else // En troisième personne : rotation progressive
	{
		yawDiff = shortestAngle(currentYaw, camera.targetYaw);

		float pitchDiff = shortestAngle(currentPitch, camera.targetPitch);

		// Limiter vitesse rotation
		constexpr float maxYawStep = 0.02f;
		constexpr float maxPitchStep = 0.02f;
		float yawStep = std::clamp(yawDiff, -maxYawStep, maxYawStep);
		float pitchStep = std::clamp(pitchDiff, -maxPitchStep, maxPitchStep);

		newYaw = currentYaw + yawStep;
		currentPitch += pitchStep;
		newPitch = currentPitch;
	}

	// Quats de rotation
	Quat yawQuat = Quat::sRotation(worldUp, newYaw);
	Vec3 right = yawQuat * Vec3::sAxisX();
	Quat pitchQuat = Quat::sRotation(right, currentPitch);

	// Rotation combinée yaw + pitch
	Quat targetRotation = (pitchQuat * yawQuat).Normalized();

	// Appliquer le roll ensuite (souris + A et D)
	Vec3 forwardAxis = targetRotation * Vec3::sAxisZ();
	Quat rollQuat = Quat::sRotation(forwardAxis, GetTargetRoll(yawDiff, inputRoll));

	targetRotation = (rollQuat * targetRotation).Normalized();
	const Quat currentRotation = rigidBody.body->GetRotation();

	// Différence entre la rotation actuel et la roation vers laquel on veut aller
	const Quat delta = (targetRotation * currentRotation.Conjugated()).Normalized();

	// Converti en axe & angle
	Vec3 axis;
	float angle;
	delta.GetAxisAngle(axis, angle);

	// Vérifie l'angle pour éviter les problemes
	if (angle > 0.0001f)
	{
		constexpr float ROTATION_SPEED = 80.0f;
		const Vec3 angularVelocity = axis * angle * ROTATION_SPEED;

		rigidBody.body->SetAngularVelocity(angularVelocity);
	}
}

void PhysicsSimulationSystem::UpdateTransforms(EntityManager& entityManager)
{
	// Update transforms from physics
	for (const auto& [entity, transform, rigidBody] : entityManager.View<Transform, RigidBody>())
	{
		// Get jolt transform data
		const RMat44& joltTransform = rigidBody.body->GetWorldTransform();
		const Vec3 joltPos = joltTransform.GetTranslation();
		const Quat joltRot = joltTransform.GetQuaternion();

		transform.position = {joltPos.GetX(), joltPos.GetY(), joltPos.GetZ()};
		transform.rotation = {joltRot.GetX(), joltRot.GetY(), joltRot.GetZ(), joltRot.GetW()};

		// Uses Jolt position and rotation and keep the orginal scale of the transform
		const auto scaleMatrix = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
		const auto rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&transform.rotation));
		const auto translationMatrix = XMMatrixTranslation(transform.position.x, transform.position.y,
		                                                   transform.position.z);

		transform.world = scaleMatrix * rotationMatrix * translationMatrix;
	}

	// Update transform from hierarchy
	for (const auto& [entity, transform, hierarchy] : entityManager.View<Transform, Hierarchy>())
	{
		if (hierarchy.parent == INVALID_ENTITY)
			continue;

		const auto& parentTransform = entityManager.Get<Transform>(hierarchy.parent);

		// Compute local matrix elements
		const XMMATRIX S = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
		const XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&transform.rotation));
		const XMMATRIX T = XMMatrixTranslation(transform.position.x, transform.position.y, transform.position.z);

		transform.world = S * R * T;
		transform.world *= parentTransform.world;
	}
}

float PhysicsSimulationSystem::GetTargetRoll(float yawDiff, float inputRoll)
{
	constexpr float maxRollAngle = 0.2f;
	static float currentRoll = 0.0f;

	// Roll cible basé sur le yawDiff
	float targetRoll = std::clamp(-yawDiff * 0.5f + inputRoll, -maxRollAngle, maxRollAngle);

	// Interpolation progressive
	constexpr float rollInterpSpeed = 0.05f;
	float rollDelta = targetRoll - currentRoll;
	rollDelta = std::clamp(rollDelta, -rollInterpSpeed, rollInterpSpeed);
	currentRoll += rollDelta;

	return currentRoll;
}
