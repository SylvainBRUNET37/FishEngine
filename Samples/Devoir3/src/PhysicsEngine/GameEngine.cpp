#include "pch.h"
#include "GameEngine.h"

#include "ecs/Components.h"
#include "ecs/EntityManagerFactory.h"
#include "PhysicsEngine/ShapeFactory.h"
#include "PhysicsEngine/systems/JoltSystem.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Mesh.h"

#include "Globals.h"

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
		return {Sanitize(value.x), Sanitize(value.y), Sanitize(value.z)};
	};

	[[nodiscard]] XMFLOAT4 Sanitize(const XMFLOAT4 value)
	{
		return {Sanitize(value.x), Sanitize(value.y), Sanitize(value.z), value.w};
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

		DestroyObjectAtEndOfLife(elapsedTime);
		ShootBallIfKeyPressed();

		UpdatePhysics();
		UpdateTransforms();

		RenderScene(elapsedTime);
		CheckForWinConditions();

		WaitBeforeNextFrame(frameStartTime);
	}
}

void GameEngine::UpdatePhysics()
{
	// update controllables
	for (const auto& [entity, rigidBody, controllable] : entityManager.View<RigidBody, Controllable>())
	{
		const auto& transform = rigidBody.body->GetWorldTransform();
		JPH::Vec3 right = transform.GetColumn3(0).Normalized();
		JPH::Vec3 up = transform.GetColumn3(1).Normalized();
		JPH::Vec3 forward = transform.GetColumn3(2).Normalized();

		JPH::Vec3 currentSpeed = JoltSystem::GetBodyInterface().GetLinearVelocity(rigidBody.body->GetID());
		JPH::Vec3 newSpeed = currentSpeed;
		bool speedChanged = false;

		if (GetAsyncKeyState('W') & 0x8000) // I
		{
			newSpeed = newSpeed + 1.0f * forward;
			speedChanged = true;
		}
		if (GetAsyncKeyState('S') & 0x8000) // K
		{
			newSpeed = newSpeed - 1.0f * forward;
			speedChanged = true;
		}
		if (GetAsyncKeyState('D') & 0x8000) // L
		{
			newSpeed = newSpeed - 1.0f * right;
			speedChanged = true;
		}
		if (GetAsyncKeyState('A') & 0x8000) // J
		{
			newSpeed = newSpeed + 1.0f * right;
			speedChanged = true;
		}

		if (speedChanged) {
			if (newSpeed.Length() > controllable.maxSpeed) newSpeed = newSpeed.Normalized();
			JoltSystem::GetBodyInterface().SetLinearVelocity(rigidBody.body->GetID(), newSpeed);
		}

		// Rotation
		bool rotatesPositive = GetAsyncKeyState('Q') & 0x8000;
		if (rotatesPositive || GetAsyncKeyState('E') & 0x8000) // U & O
		{
			JPH::Quat delta = JPH::Quat::sRotation(up, .05f * (1-2*!rotatesPositive)); // theta = 10
			JoltSystem::GetBodyInterface().SetRotation(
				rigidBody.body->GetID(),
				(rigidBody.body->GetRotation() * delta).Normalized(),
				JPH::EActivation::Activate);
		}
	}


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
	Transform cubeTransform;
	for (const auto& [entity, name, transform] : entityManager.View<Name, Transform>())
	{
		if (name.name == "Cube") {
			cubeTransform = transform;
		}
	}
	
	renderSystem.UpdateScene(elapsedTime,cubeTransform);

	for (const auto& [entity, transform, mesh] : entityManager.View<Transform, Mesh>())
	{
		renderSystem.Render(mesh, transform);
	}

	renderSystem.Render();
}

void GameEngine::CheckForWinConditions()
{
	if (currentWinCount < Globals::getNGamesWon())
	{
		//A new game has been won
		currentWinCount = Globals::getNGamesWon();
		if (currentWinCount >= Globals::getMaxGamesWon())
		{
			abort(); //end game
		}
		MoveSensorRandomly();
	}
}

void GameEngine::ShootBallIfKeyPressed()
{
	static constexpr double timeBetweenBallShoot = 500; // in ms
	static double prevTimeBallWasFired = 0;

	for (const auto& [entity, entityTransform, entityBallShooter] : entityManager.View<Transform, BallShooter>())
	{
		if (GetAsyncKeyState(entityBallShooter.inputKey) & 0x8000 
			&& GetTickCount() - prevTimeBallWasFired > timeBetweenBallShoot) [[unlikely]]
		{
			prevTimeBallWasFired = GetTickCount();
			const auto ballEntity = entityManager.CreateEntity();

			constexpr auto spawnDistance = 10.f;
			const auto entityRotation = Sanitize(entityTransform.rotation);

			const XMVECTOR entityForwardDirection =

				XMVector3Rotate(XMVectorSet(0, 0, 1, 0),
				                XMLoadFloat4(&entityRotation));

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

			static constexpr double ballLifeTime = 10.0;
			entityManager.AddComponent<LifeSpan>(ballEntity, 0.0, ballLifeTime);

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

void GameEngine::DestroyObjectAtEndOfLife(const double elapsedTIme)
{
	for (const auto& [entity, lifeSpan] : entityManager.View<LifeSpan>())
	{
		lifeSpan.lifeTime += elapsedTIme;
		if (lifeSpan.lifeTime >= lifeSpan.lifeDuration) [[unlikely]]
			entityManager.Kill(entity);
	}
}

void GameEngine::WaitBeforeNextFrame(const DWORD frameStartTime)
{
	const DWORD frameEnd = GetTickCount();
	const DWORD frameDuration = frameEnd - frameStartTime;

	if (frameDuration < FRAME_TIME)
		Sleep(static_cast<DWORD>(FRAME_TIME - frameDuration));
}

void GameEngine::MoveSensorRandomly()
{
	//TODO: Teleport sensor here
	for (const auto& [entity, name, rigidBody] : entityManager.View<Name, RigidBody>())
	//Was there a better way to do this? Probably...
	{
		if (name.name == "Capsule")
		{
			// Get jolt transform data
			const JPH::RMat44& joltTransform = rigidBody.body->GetWorldTransform();
			const JPH::Vec3 joltPos = joltTransform.GetTranslation();
			float newX = rand() % 500 - 250; //Not clean at all, but sufficient for testing, hopefully...
			float newY = 17.565f; //Should this be in global?
			float newZ = rand() % 500 - 250;
			JPH::BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
			bodyInterface.SetPosition(rigidBody.body->GetID(), JPH::RVec3Arg(newX, newY, newZ),
			                          JPH::EActivation::Activate);
		}
	}
}
