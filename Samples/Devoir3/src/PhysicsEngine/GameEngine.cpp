#include "pch.h"
#include "GameEngine.h"

#include "ecs/Components.h"
#include "ecs/EntityManagerFactory.h"
#include "PhysicsEngine/ShapeFactory.h"
#include "PhysicsEngine/systems/JoltSystem.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Mesh.h"

namespace
{
	[[nodiscard]] float Sanitize(const float value)
	{
		if (!std::isfinite(value))
			return 0.0f;

		if (std::fabs(value) < 1e-6f || std::fabs(value) > 1e6f)
			return 0.0f;

		return value;
	};

	[[nodiscard]] XMFLOAT3 Sanitize(const XMFLOAT3 value)
	{
		return { Sanitize(value.x), Sanitize(value.y), Sanitize(value.z) };
	};

	[[nodiscard]] XMFLOAT4 Sanitize(const XMFLOAT4 value)
	{
		return { Sanitize(value.x), Sanitize(value.y), Sanitize(value.z), value.w };
	};
}

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
		ShootBallIfKeyPressed();

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

void GameEngine::RenderScene(const double elapsedTime)
{
	renderSystem.UpdateScene(elapsedTime);

	for (const auto& [entity, transform, mesh] : entityManager.View<Transform, Mesh>())
	{
		renderSystem.Render(mesh, transform);
	}

	renderSystem.Render();
}

void GameEngine::ShootBallIfKeyPressed()
{
	for (const auto& [entity, entityTransform, entityBallShooter] : entityManager.View<Transform, BallShooter>())
	{
		if (GetAsyncKeyState(entityBallShooter.inputKey) & 0x8000) [[unlikely]]
		{
			const auto ballEntity = entityManager.CreateEntity();

			constexpr auto spawnDistance = 10.f;
			const auto entityRotation = Sanitize(entityTransform.rotation);

			const XMVECTOR entityForwardDirection = 
			
				XMVector3Rotate(XMVectorSet(0, 0, 1, 0),
				                XMLoadFloat4(&entityRotation))
			;

			const XMVECTOR entityPos = XMLoadFloat3(&entityTransform.position);
			const XMVECTOR ballSpawnPosition = XMVectorAdd(
				entityPos, XMVectorScale(entityForwardDirection, spawnDistance));

			XMFLOAT3 ballPosition;
			XMStoreFloat3(&ballPosition, ballSpawnPosition);

			// Sanitize values to avoid very little/high values
			ballPosition = Sanitize(ballPosition);

			const Transform ballTransform
			{
				.world = XMMatrixIdentity(),
				.position = ballPosition,
				.rotation = {0, 0, 0, 1},
				.scale = {1, 1, 1},
			};

			// Add components on the ball
			entityManager.AddComponent<Transform>(ballEntity, ballTransform);
			entityManager.AddComponent<Mesh>(ballEntity, resourceManager.LoadSphere());

			XMFLOAT3 direction;
			XMStoreFloat3(&direction, entityForwardDirection);
			direction = Sanitize(direction);

			JoltSystem::AddPostStepCallback([this, ballEntity, ballTransform, direction]()
			{
				entityManager.AddComponent<RigidBody>(
					ballEntity, ShapeFactory::CreateSphere(ballTransform, direction));
			});
		}
	}
}

void GameEngine::WaitBeforeNextFrame(const DWORD frameStartTime)
{
	const DWORD frameEnd = GetTickCount();
	const DWORD frameDuration = frameEnd - frameStartTime;

	if (frameDuration < FRAME_TIME)
		Sleep(static_cast<DWORD>(FRAME_TIME - frameDuration));
}
