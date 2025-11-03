#include "pch.h"
#include "GameEngine.h"

#include "components/Components.h"
#include "entities/EntityManagerFactory.h"
#include "PhysicsEngine/ShapeFactory.h"
#include "PhysicsEngine/systems/JoltSystem.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Mesh.h"

#include "Globals.h"

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

		for (const auto& system : systems)
			system->Update(elapsedTime, entityManager);

		CheckForWinConditions();

		WaitBeforeNextFrame(frameStartTime);
	}
}

void GameEngine::CheckForWinConditions()
{
	if (currentWinCount < Globals::getNGamesWon())
	{
		// A new game has been won !
		currentWinCount = Globals::getNGamesWon();
		if (currentWinCount >= Globals::getMaxGamesWon())
		{
			entityManager = {}; // Reset the scene
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

			const XMVECTOR entityForwardDirection =
				XMVector3Rotate(XMVectorSet(0, 0, 1, 0),
				                XMLoadFloat4(&entityTransform.rotation));

			const XMVECTOR entityPos = XMLoadFloat3(&entityTransform.position);
			const XMVECTOR ballSpawnPosition = XMVectorAdd(
				entityPos, XMVectorScale(entityForwardDirection, spawnDistance));

			XMFLOAT3 ballPosition;
			XMStoreFloat3(&ballPosition, ballSpawnPosition);

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
	for (const auto& [entity, name, rigidBody] : entityManager.View<Name, RigidBody>())
	{
		if (name.name == "Capsule")
		{
			// Get jolt transform data
			const float newX = rand() % 500 - 250; // Not clean at all, but sufficient for testing, hopefully...
			constexpr float newY = 17.565f; // Should this be in global?
			const float newZ = rand() % 500 - 250;
			JPH::BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
			bodyInterface.SetPosition(rigidBody.body->GetID(), JPH::RVec3Arg(newX, newY, newZ),
			                          JPH::EActivation::Activate);
		}
	}
}
