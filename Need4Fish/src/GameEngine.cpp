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
		const auto isGamePaused = GameState::currentState != GameState::PLAYING;

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
	const bool isPausableOrResumable = GameState::currentState == GameState::PLAYING || GameState::currentState == GameState::PAUSED;

	// Restart the game if has been was pressed
	if (GetAsyncKeyState('R') & 0x8000 && GameState::currentState != GameState::PAUSED)
	{
		ResumeGame(mainMenuEntity);
		InitGame();
	}
	

	//if (isEscapePressed && !wasEscapePressed)
	if (isEscapePressed && !wasEscapePressed && isPausableOrResumable)
		ChangeGameStatus();

	wasEscapePressed = isEscapePressed;
}

void GameEngine::HandleCollions() {
	auto isBodyEatable = [&](JPH::BodyID bodyId) -> std::optional<std::pair<const Entity, Eatable&>> {
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
			return std::pair<Entity, Eatable&>(entity, eatable);

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
			auto& [firstEntity, firstEatable] = firstObject.value();
			auto& [secondEntity, secondEatable] = secondObject.value();

			// Kill things if necessary
			if (firstEatable.CanBeEatenBy(secondEatable)) {
				if (isEntityAPlayer(firstEntity))
				{
					//InitGame();
					GameState::currentState = GameState::DIED;
				}
				else
				{
					if (firstEatable.isApex) GameState::currentState = GameState::WON;
					entityManager.Kill(firstEntity);
					secondEatable.mass += firstEatable.mass;
				}
			}
			else if (secondEatable.CanBeEatenBy(firstEatable)) {
				if (isEntityAPlayer(secondEntity))
				{
					//InitGame();
					GameState::currentState = GameState::DIED;
				}
				else
				{
					if (secondEatable.isApex) GameState::currentState = GameState::WON;
					entityManager.Kill(secondEntity);
					firstEatable.mass += secondEatable.mass;
				}
			}
			if (GameState::currentState != GameState::PLAYING) ChangeGameStatus();
		}
	}
}

void GameEngine::ChangeGameStatus()
{
	switch (GameState::currentState)
	{
	case GameState::PAUSED:
		ResumeGame(mainMenuEntity);
		break;
	case GameState::PLAYING:
		PauseGame(mainMenuEntity);
		break;
	case GameState::WON:
	case GameState::DIED:
		EndGame(mainMenuEntity);
		break;
	}
}

void GameEngine::ResumeGame(const Entity mainMenuEntity)
{
	CameraSystem::SetMouseCursor();
	GameState::currentState = GameState::PLAYING;
	entityManager.RemoveComponent<Sprite2D>(mainMenuEntity);
}

void GameEngine::PauseGame(const Entity mainMenuEntity)
{
	ShowCursor(TRUE);
	Camera::isMouseCaptured = false;

	ClipCursor(nullptr);
	ReleaseCapture();

	GameState::currentState = GameState::PAUSED;

	entityManager.AddComponent<Sprite2D>
	(
		mainMenuEntity,
		uiManager.LoadSprite("assets/pauseTitle.png", resourceManager)
	);
}

void GameEngine::EndGame(const Entity mainMenuEntity)
{
	ShowCursor(TRUE);
	Camera::isMouseCaptured = false;

	ClipCursor(nullptr);
	ReleaseCapture();

	auto sprite = (GameState::currentState == GameState::DIED) ?  "assets/deathTitle.png" : "assets/winTitle.png";

	entityManager.AddComponent<Sprite2D>
		(
			mainMenuEntity,
			uiManager.LoadSprite(sprite, resourceManager)
		);
}

// TODO: Init it properly
void GameEngine::InitGame()
{
	entityManager = EntityManagerFactory::Create(resourceManager.LoadScene());

	// TODO: revise this
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	Camera camera;
	camera.position = XMVectorSet(0, 5, -10, 1);
	camera.focus = XMVectorSet(0, 0, 0, 1);
	camera.up = XMVectorSet(0, 1, 0, 0);
	camera.aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	camera.distance = 80.f;
	camera.heightOffset = 30.f;

	const auto cameraEntity = entityManager.CreateEntity();
	auto& cameraComponent = entityManager.AddComponent<Camera>(cameraEntity, camera);

	GameState::currentCameraEntity = cameraEntity;

	// Initialize the scene (it's a temporary way of doing it)
	for (const auto& [entity, name] : entityManager.View<Name>())
	{
		if (name.name == "Cube" || name.name.find("Mosasaure") != std::string::npos)
		{
			const auto transform = entityManager.Get<Transform>(entity);
			const auto mesh = entityManager.Get<Mesh>(entity);
			auto& en = entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCube(transform, mesh));
			entityManager.AddComponent<Eatable>(entity, 100.0f);
			entityManager.AddComponent<Controllable>(entity, 200.0f);

			// Link camera to the mosasaur
			cameraComponent.targetEntity = entity;

			std::cout << "created rigid body " << name.name << " with ID: " << en.body->GetID().GetIndexAndSequenceNumber() << std::endl;
		}
		else if (name.name == "Aquarium" || name.name.find("Monticule") != std::string::npos || name.name == "Plane")
		{
			const auto transform = entityManager.Get<Transform>(entity);
			const auto mesh = entityManager.Get<Mesh>(entity);
			auto& en = entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateMeshShape(transform, mesh));
			std::cout << "created rigid body " << name.name << " with ID: " << en.body->GetID().GetIndexAndSequenceNumber() << std::endl;
		}
		// Eatable fish part
		else if (name.name.find("BasicFish") != std::string::npos)
		{
			const auto transform = entityManager.Get<Transform>(entity);
			const auto mesh = entityManager.Get<Mesh>(entity);
			auto& en = entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCube(transform, mesh));
			entityManager.AddComponent<Eatable>(entity, 75.f);

			std::cout << "created rigid body " << name.name << " with ID: " << en.body->GetID().GetIndexAndSequenceNumber() << std::endl;
		}
		else if (name.name.find("CoralFish") != std::string::npos)
		{
			const auto transform = entityManager.Get<Transform>(entity);
			const auto mesh = entityManager.Get<Mesh>(entity);
			auto& en = entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCube(transform, mesh));
			entityManager.AddComponent<Eatable>(entity, 100.f);

			std::cout << "created rigid body " << name.name << " with ID: " << en.body->GetID().GetIndexAndSequenceNumber() << std::endl;
		}
		else if (name.name.find("Momsasaure") != std::string::npos) // Mom
		{
			const auto transform = entityManager.Get<Transform>(entity);
			const auto mesh = entityManager.Get<Mesh>(entity);
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCube(transform, mesh));
			entityManager.AddComponent<Eatable>(entity, 200.f, true);
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