#include "pch.h"
#include "GameEngine.h"

#include <iostream>
#include <tuple>
#include <optional>
#include <algorithm>
#include "GameState.h"
#include "entities/EntityManagerFactory.h"
#include "PhysicsEngine/ShapeFactory.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/texture/TextureLoader.h"

using namespace DirectX;

void GameEngine::Run()
{
	bool shouldContinue = true;
	DWORD prevTime = GetTickCount();

	while (shouldContinue)
	{
		// Pause/Unpause the game if ESC is pressed for exemple
		HandleGameState();

		const DWORD frameStartTime = GetTickCount();
		const auto isGamePaused = GameState::currentState == GameState::PAUSED;

		const double elapsedTime = isGamePaused ? 0.0 : (frameStartTime - prevTime) / 1000.0;

		if (not isGamePaused) [[likely]]
			prevTime = frameStartTime;

		// End the loop if Windows want to terminate the program (+ process messages)
		shouldContinue = WindowsApplication::ProcessWindowsMessages();

		for (const auto& system : systems)
			system->Update(elapsedTime, entityManager);

		WaitBeforeNextFrame(frameStartTime);
	}
}

void GameEngine::WaitBeforeNextFrame(const DWORD frameStartTime)
{
	const DWORD frameEnd = GetTickCount();
	const DWORD frameDuration = frameEnd - frameStartTime;

	if (frameDuration < FRAME_TIME)
		Sleep(static_cast<DWORD>(FRAME_TIME - frameDuration));
}

void GameEngine::HandleGameState()
{
	// Check if things get eaten
	HandleCollions();

	static bool wasEscapePressed = false;
	const bool isEscapePressed = GetAsyncKeyState(VK_ESCAPE) & 0x8000;

	// Restart the game if has been was pressed
	if ((GetAsyncKeyState('R') & 0x8000) || GameState::currentState == GameState::DIED)
		InitGame();

	if (isEscapePressed && !wasEscapePressed)
		ChangeGameStatus();

	wasEscapePressed = isEscapePressed;
}

void GameEngine::HandleCollions() {
	
	
	auto isBodyEatable = [&](JPH::BodyID bodyId) -> std::optional<std::pair<Entity, Eatable>> {
		auto eatables = entityManager.View<RigidBody, Eatable>();
		auto it = std::find_if(eatables.begin(), eatables.end(),
			[&](auto&& tuple)
			{
				auto& [entity, rigidBody, eatable] = tuple;
				return rigidBody.body->GetID() == bodyId;
			});
		if (it != eatables.end())
		{
			const auto& [entity, _, eatable] = *it;
			return std::make_pair(entity, eatable);
		}
		return std::nullopt;
	};

	auto isEntityAPlayer = [&](Entity SearchedEntity) -> bool {
		auto watchables = entityManager.View<Controllable>();
		auto it = std::find_if(watchables.begin(), watchables.end(),
			[&](auto&& tuple)
			{
				auto& [entity, _] = tuple;
				return entity.index == SearchedEntity.index;
			});
		return it != watchables.end();
		};
	
	while (!GameState::detectedCollisions.empty())
	{
		auto& [bodyId1, bodyId2] = GameState::detectedCollisions.front();
		GameState::detectedCollisions.pop();

		// Check if the both objects is eatable
		auto firstObject = isBodyEatable(bodyId1);
		auto secondObject = isBodyEatable(bodyId2);
		if (firstObject.has_value() && secondObject.has_value())
		{
			// Kill things if necessary
			if (firstObject.value().second.CanBeEatenBy(secondObject.value().second)) {
				// Kill 1
				auto entity = firstObject.value().first;
				if (isEntityAPlayer(entity))
				{
					// Die
					InitGame();
				}
				else entityManager.Kill(entity);
			}
			else if (secondObject.value().second.CanBeEatenBy(firstObject.value().second)) {
				// Kill 2
				auto entity = secondObject.value().first;
				if (isEntityAPlayer(entity))
				{
					// Die
					InitGame();
				}
				else entityManager.Kill(secondObject.value().first);
			}
		}
	}
}

void GameEngine::ChangeGameStatus()
{
	GameState::currentState == GameState::PAUSED
		? PauseGame(mainMenuEntity)
		: ResumeGame(mainMenuEntity);
}

void GameEngine::PauseGame(const Entity mainMenuEntity)
{
	CameraSystem::SetMouseCursor();
	GameState::currentState = GameState::PLAYING;
	entityManager.RemoveComponent<Sprite2D>(mainMenuEntity);
}

void GameEngine::ResumeGame(const Entity mainMenuEntity)
{
	ShowCursor(TRUE);
	Camera::isMouseCaptured = false;

	ClipCursor(nullptr);
	ReleaseCapture();

	GameState::currentState = GameState::PAUSED;

	entityManager.AddComponent<Sprite2D>
	(
		mainMenuEntity, 
		uiManager.LoadSprite("assets/pauseMenu.jpg", resourceManager)
	);
}

// TODO: Init it properly
void GameEngine::InitGame()
{
	entityManager = EntityManagerFactory::Create(resourceManager.LoadScene());

	// TODO: revise this
	Camera camera;
	camera.position = XMVectorSet(0, 5, -10, 1);
	camera.focus = XMVectorSet(0, 0, 0, 1);
	camera.up = XMVectorSet(0, 1, 0, 0);
	camera.aspectRatio = static_cast<float>(1920) / static_cast<float>(1080);
	camera.distance = 100.f;
	camera.heightOffset = 30.f;

	const auto cameraEntity = entityManager.CreateEntity();
	auto& cameraComponent = entityManager.AddComponent<Camera>(cameraEntity, camera);

	GameState::currentCameraEntity = cameraEntity;

	// Initialize the scene (it's a temporary way of doing it)
	for (const auto& [entity, name] : entityManager.View<Name>())
	{
		if (name.name == "Cube")
		{
			const auto transform = entityManager.Get<Transform>(entity);
			const auto mesh = entityManager.Get<Mesh>(entity);
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCube(transform, mesh));
			entityManager.AddComponent<Controllable>(entity, 100.0f);
			entityManager.AddComponent<Eatable>(entity, 100.0f);

			// Link camera to the mosasaur
			cameraComponent.targetEntity = entity;
		}
		else if (name.name == "Plane")
		{
			const auto transform = entityManager.Get<Transform>(entity);
			//entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreatePlane(transform));
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateFloor());
		}
		else if (name.name == "Aquarium" || name.name == "Sphere" || name.name == "Caverne")
		{
			if (name.name == "Sphere") {
				entityManager.AddComponent<Eatable>(entity, 80.0f); // eatable
			}
			const auto transform = entityManager.Get<Transform>(entity);
			const auto mesh = entityManager.Get<Mesh>(entity);
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateMeshShape(transform, mesh));
		}
	}

	// Add light
	PointLight pointLight =
	{
		.ambient = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f),
		.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),

		.position = XMFLOAT3(2.0f, 20.0f, -20.0f),
		.range = 50.0f,

		.attenuation = XMFLOAT3(1.0f, 0.09f, 0.032f),
		.pad = 0.0f
	};
	const auto pointLightEntity = entityManager.CreateEntity();
	entityManager.AddComponent<PointLight>(pointLightEntity, pointLight);

	mainMenuEntity = entityManager.CreateEntity();
}