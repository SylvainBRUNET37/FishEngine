#include "pch.h"
#include "GameEngine.h"

#include <tuple>
#include <optional>
#include <algorithm>
#include "GameState.h"
#include "entities/EntityManagerFactory.h"
#include "Locator.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/texture/TextureLoader.h"
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>

#include <iostream>
#include <fstream>

using namespace DirectX;

GameEngine::GameEngine(RenderContext* renderContext)
	: uiManager{ renderContext->GetDevice() }
{
	CameraSystem::SetMouseCursor();

	auto& sceneResources = Locator::Get<ResourceManager>().LoadScene();

	// Care about the order of construction, it will be the order of update calls
	systems.emplace_back(std::make_unique<PhysicsSimulationSystem>());
	systems.emplace_back(std::make_unique<CameraSystem>());
	systems.emplace_back(std::make_unique<RenderSystem>(renderContext, std::move(sceneResources.materials)));

	InitGame();
}

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
		if (GameState::currentState != GameState::PLAYING) ResumeGame();
		InitGame();
	}
	

	//if (isEscapePressed && !wasEscapePressed)
	if (isEscapePressed && !wasEscapePressed && isPausableOrResumable)
		ChangeGameStatus();

	wasEscapePressed = isEscapePressed;
}

void GameEngine::HandleCollions() {
	auto isBodyEatable = [&](JPH::BodyID bodyId) -> std::optional<std::tuple<const Entity, Eatable&, RigidBody&>> {
		auto eatables = entityManager.View<Eatable, RigidBody>();
		auto it = std::find_if(eatables.begin(), eatables.end(),
			[&](auto&& tuple)
			{
				auto& [entity, eatable, rigidBody] = tuple;
				return rigidBody.body->GetID() == bodyId;
			});
		if (it != eatables.end())
		{
			return std::tuple<Entity, Eatable&, RigidBody&>(*it);
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
			auto& [firstEntity, firstEatable, firstBody] = firstObject.value();
			auto& [secondEntity, secondEatable, secondBody] = secondObject.value();

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

					// Shape scaling test
					auto currentShape = secondBody.body->GetShape();
					auto& bodyInterface = JoltSystem::GetPhysicSystem().GetBodyInterface();
					JPH::Vec3 scale(2.0f, 2.0f, 2.0f);   // test scale // TODO changer ça + fonction
					if (currentShape->GetSubType() == JPH::EShapeSubType::Box)
					{
						const JPH::BoxShape* box = static_cast<const JPH::BoxShape*>(currentShape);

						JPH::Vec3 newHalfExtents = box->GetHalfExtent() * scale;

						JPH::RefConst<JPH::Shape> newShape = new JPH::BoxShape(newHalfExtents);

						bodyInterface.SetShape(bodyId2, newShape, true, JPH::EActivation::Activate);
					}
					// Scale mesh
					auto& trans = entityManager.Get<Transform>(secondEntity);
					trans.scale.x *= 2;
					trans.scale.y *= 2;
					trans.scale.z *= 2;
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

					// Shape scaling test
					auto currentShape = firstBody.body->GetShape();
					auto& bodyInterface = JoltSystem::GetPhysicSystem().GetBodyInterface();
					JPH::Vec3 scale(2.0f, 2.0f, 2.0f);   // test scale // TODO: changer ça + fonction
					if (currentShape->GetSubType() == JPH::EShapeSubType::Box)
					{
						const JPH::BoxShape* box = static_cast<const JPH::BoxShape*>(currentShape);

						JPH::Vec3 newHalfExtents = box->GetHalfExtent() * scale;

						JPH::RefConst<JPH::Shape> newShape = new JPH::BoxShape(newHalfExtents);

						bodyInterface.SetShape(bodyId1, newShape, true, JPH::EActivation::Activate);
					}
					// Scale mesh
					auto& mesh = entityManager.Get<Mesh>(firstEntity);
					auto& trans = entityManager.Get<Transform>(firstEntity);
					trans.scale.x *= 2;
					trans.scale.y *= 2;
					trans.scale.z *= 2;
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
		ResumeGame();
		break;
	case GameState::PLAYING:
		PauseGame();
		break;
	case GameState::WON:
	case GameState::DIED:
		EndGame();
		break;
	}
}

void GameEngine::ResumeGame()
{
	CameraSystem::SetMouseCursor();
	GameState::currentState = GameState::PLAYING;
	entityManager.RemoveComponent<Sprite2D>(mainMenuEntity);
}

void GameEngine::PauseGame()
{
	ShowCursor(TRUE);
	Camera::isMouseCaptured = false;

	ClipCursor(nullptr);
	ReleaseCapture();

	GameState::currentState = GameState::PAUSED;

	entityManager.AddComponent<Sprite2D>
	(
		mainMenuEntity,
		uiManager.LoadSprite("assets/ui/pauseTitle.png")
	);
}

void GameEngine::EndGame()
{
	ShowCursor(TRUE);
	Camera::isMouseCaptured = false;

	ClipCursor(nullptr);
	ReleaseCapture();

	const auto sprite = (GameState::currentState == GameState::DIED) ?  "assets/ui/deathTitle.png" : "assets/ui/winTitle.png";

	entityManager.AddComponent<Sprite2D>
		(
			mainMenuEntity,
			uiManager.LoadSprite(sprite)
		);
}

// TODO: Init it properly
void GameEngine::InitGame()
{
	entityManager = EntityManagerFactory::Create(Locator::Get<ResourceManager>().GetSceneResource());

	// TODO: revise this
	const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

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

	// Assign the controllable entity to the camera (it's not a pretty way of doing it but it works)
	unsigned short nbControllable = 0;
	for (const auto& [entity, controllable] : entityManager.View<Controllable>())
	{
		++nbControllable;
		vassert(nbControllable == 1, "Currently, it's only possible to have 1 controllable entity.");

		cameraComponent.targetEntity = entity;
	}

	mainMenuEntity = entityManager.CreateEntity();
}