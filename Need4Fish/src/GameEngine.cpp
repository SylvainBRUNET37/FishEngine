#include "pch.h"
#include "GameEngine.h"

#include <tuple>
#include <algorithm>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <numbers> 

#include "GameState.h"
#include "entities/EntityManagerFactory.h"
#include "Locator.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/texture/TextureLoader.h"
#include "gameplay/mechanics/Eating.h"
#include "physicsEngine/utils/MeshUtil.h"
#include "systems/PowerSystem.h"
#include "systems/SensorSystem.h"
#include "utils/MathsUtils.h"
#include "utils/EntityManagerUtils.h"

using namespace DirectX;
using namespace JPH;

GameEngine::GameEngine(RenderContext* renderContext_)
	: renderContext{renderContext_},
	  particleSystem{renderContext->GetDevice()},
	  uiManager{std::make_shared<UIManager>(renderContext->GetDevice())}
{
	CameraSystem::SetMouseCursor();

	auto& sceneResources = Locator::Get<ResourceManager>().LoadScene();

	// Care about the order of construction, it will be the order of update calls
	systems.emplace_back(std::make_unique<PhysicsSimulationSystem>());
	systems.emplace_back(std::make_unique<SensorSystem>());
	systems.emplace_back(std::make_unique<CameraSystem>());
	systems.emplace_back(std::make_unique<PowerSystem>());
	systems.emplace_back(std::make_unique<RenderSystem>(renderContext, uiManager, std::move(sceneResources.materials)));

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

		if (GameState::currentState == GameState::FINISHED)
			return;

		const DWORD frameStartTime = GetTickCount();
		const auto isGamePaused = GameState::currentState != GameState::PLAYING;

		const double elapsedTime = isGamePaused ? 0.0 : (frameStartTime - prevTime) / 1000.0;

		prevTime = frameStartTime;
		GameState::playTime += elapsedTime;

		// End the loop if Windows want to terminate the program (+ process messages)
		shouldContinue = WindowsApplication::ProcessWindowsMessages();

		particleSystem.Update(elapsedTime, *entityManager);

		for (const auto& system : systems)
			system->Update(elapsedTime, *entityManager);

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
	if (GameState::isGrowing)
		Eating::UpdatePlayerScale(*entityManager);

	// Check if things get eaten
	HandleCollions();

	static bool wasEscapePressed = false;
	const bool isEscapePressed = GetAsyncKeyState(VK_ESCAPE) & 0x8000;
	const bool isPausableOrResumable = GameState::currentState == GameState::PLAYING || GameState::currentState ==
		GameState::PAUSED;

	// Handle clicks
	static bool leftButtonPreviouslyDown = false;
	bool leftButtonDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
	if (leftButtonDown && !leftButtonPreviouslyDown && GameState::currentState != GameState::PLAYING)
		uiManager->HandleClick();
	leftButtonPreviouslyDown = leftButtonDown;

	// Restart the game if has been was pressed
	if (GetAsyncKeyState('R') & 0x8000 && GameState::currentState != GameState::PAUSED)
	{
		RestartGame();
	}

	if (isEscapePressed && !wasEscapePressed && isPausableOrResumable)
		ChangeGameStatus();

	wasEscapePressed = isEscapePressed;
}

void GameEngine::HandleCollions()
{
	auto isAiFish = [](EntityManager& entityManager, const Entity& entity) {
		return entityManager.HasComponent<Eatable>(entity) && entityManager.HasComponent<AIController>(entity);
		};

	while (!GameState::detectedCollisions.empty())
	{
		auto& [bodyId1, bodyId2] = GameState::detectedCollisions.front();
		GameState::detectedCollisions.pop();

		auto optEntity1 = EntityManagerUtils::GetEntityFromBody(*entityManager, bodyId1);
		auto optEntity2 = EntityManagerUtils::GetEntityFromBody(*entityManager, bodyId2);

		if (!optEntity1.has_value() || !optEntity2.has_value())
			continue;

		const Entity entity1 = optEntity1.value();
		const Entity entity2 = optEntity2.value();

		if (entityManager->HasComponent<Eatable>(entity1)
			&& entityManager->HasComponent<Eatable>(entity2))
		{
			Eating::Eat(*entityManager, bodyId1, bodyId2);
		}
		else
		{
			// Rotate da fishes around
			if (isAiFish(*entityManager, entity1))
				MathsUtils::TurnFishAround(*entityManager, entity1);
			if (isAiFish(*entityManager, entity2))
				MathsUtils::TurnFishAround(*entityManager, entity2);
		}

		if (GameState::currentState != GameState::PLAYING) ChangeGameStatus();
	}
}

void GameEngine::ChangeGameStatus()
{
	WindowsApplication::mouseWheelDelta = 0;

	switch (GameState::currentState)
	{
	case GameState::PAUSED:
		ResumeGame();
		break;
	case GameState::PLAYING:
		ShowCursor(TRUE);
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
	uiManager->Clear();
}

void GameEngine::PauseGame()
{
	Camera::isMouseCaptured = false;

	ClipCursor(nullptr);
	ReleaseCapture();

	GameState::currentState = GameState::PAUSED;

	BuildPauseMenu();
}

void GameEngine::EndGame()
{
	ShowCursor(TRUE);
	Camera::isMouseCaptured = false;

	ClipCursor(nullptr);
	ReleaseCapture();

	BuildEndMenu();
}

void GameEngine::RestartGame()
{
	uiManager->Clear();
	if (GameState::currentState != GameState::PLAYING) ResumeGame();
	InitGame();
}

// TODO: Init it properly
void GameEngine::InitGame()
{
	entityManager = EntityManagerFactory::Create(Locator::Get<ResourceManager>().GetSceneResource());
	particleSystem.Reset();

	PowerSystem::ResetPowers();

	const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	Camera camera;
	Camera::position = XMVectorSet(0, 5, -10, 1);
	camera.focus = XMVectorSet(0, 0, 0, 1);
	camera.up = XMVectorSet(0, 1, 0, 0);
	camera.aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	Camera::distance = 80.f;
	Camera::heightOffset = 30.f;
	Camera::minDistance = 50.0f;
	Camera::maxDistance = 170.0f;
	Camera::zoomSpeed = 1.0f;
	Camera::firstPersonOffset = {0.0f, -1.7f, 35.0f};
	Camera::currentDistance = Camera::distance;

	const auto cameraEntity = entityManager->CreateEntity();
	auto& cameraComponent = entityManager->AddComponent<Camera>(cameraEntity, camera);

	GameState::currentCameraEntity = cameraEntity;
	GameState::postProcessSettings = {};

	// Assign the controllable entity to the camera (it's not a pretty way of doing it but it works)
	unsigned short nbControllable = 0;
	for (const auto& [entity, controllable] : entityManager->View<Controllable>())
	{
		++nbControllable;
		vassert(nbControllable == 1, "Currently, it's only possible to have 1 controllable entity.");

		cameraComponent.targetEntity = entity;
	}

	for (const auto& [entity, name, _, rigidBody] : entityManager->View<Name, Eatable, RigidBody>())
	{
		const auto& transform = rigidBody.body->GetWorldTransform();
		JPH::Vec3 forward = transform.GetAxisZ();

		std::cout << name.name << " forward : " << forward.GetX() << ", " << forward.GetY() << ", " << forward.GetZ() << ", " << std::endl;

		if (name.name != "Mosasaure")
			entityManager->AddComponent<AIController>(entity, 100.0f, 10.0f);
	}

	CreateParticleZones();

	mainMenuEntity = entityManager->CreateEntity();

	GameState::playTime = 0.0f;
}

void GameEngine::CreateParticleZones()
{
	static auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();

	static const std::shared_ptr<ShaderProgram> bubbleShader = shaderBank.GetOrCreateShaderProgram
	(
		renderContext->GetDevice(),
		"shaders/BubbleInstancingVS.hlsl",
		"shaders/BubbleInstancingPS.hlsl"
	);

	static const Texture bubbleTexture = TextureLoader::LoadTextureFromFile(
		"assets/textures/bble.png", renderContext->GetDevice());

#ifndef NDEBUG
	static constexpr unsigned int NB_WORLD_PARTICLES = 500;
#else
	static constexpr unsigned int NB_WORLD_PARTICLES = 10'000;
#endif

	particleSystem.AddParticleZone(*entityManager,
	                               {
		                               .centerPosition = {0, 1300.0f, 0},
		                               .halfExtends = {7000.0f,900.0f, 7000.0f},
		                               .nbParticle = NB_WORLD_PARTICLES,
		                               .particleDurationMin = 3.0f,
		                               .particleDurationMax = 15.0f,
		                               .particleSpeedMin = 0.5f,
		                               .particleSpeedMax = 1.0f,
		                               .particleDirection = {0, 1, 0},
		                               .billboardTexture = bubbleTexture,
		                               .billboardShader = bubbleShader
	                               }
	);
	// Add particles inside the world (TODO: make it fit the world size)

	// Add particle zone for geysers and currents
	// It's not a pretty way of doing it :/
	for (const auto& [entity, sensor, transform, name] : entityManager->View<Sensor, Transform, Name>())
	{
		auto& bodyInterface = JoltSystem::GetBodyInterface();
		if (name.name == "CurrentSensor")
		{
			const RefConst<Shape> shape = bodyInterface.GetShape(sensor.body->GetID());

			if (shape->GetSubType() == EShapeSubType::Box)
			{
				const auto sensorBoxShape = static_cast<const BoxShape*>(shape.GetPtr());

#ifndef NDEBUG
				static constexpr unsigned int NB_CURRENT_PARTICLES = 75;
#else
				static constexpr unsigned int NB_CURRENT_PARTICLES = 1000;
#endif

				particleSystem.AddParticleZone(*entityManager,
				                               {
					                               .centerPosition = transform.position,
					                               .halfExtends = MeshUtil::ToDirectX(sensorBoxShape->GetHalfExtent()),
					                               .nbParticle = NB_CURRENT_PARTICLES,
					                               .particleDurationMin = 0.5f,
					                               .particleDurationMax = 1.0f,
												   .particleSpeedMin = 8.0f,
				                               	   .particleSpeedMax = 12.0f,
					                               .particleDirection = MeshUtil::ToDirectX(sensor.direction),
					                               .billboardTexture = bubbleTexture,
					                               .billboardShader = bubbleShader
				                               }
				);
			}
		}
		else if (name.name == "GeyserSensor" || name.name == "VolcanoSensor")
		{
			const RefConst<Shape> shape = bodyInterface.GetShape(sensor.body->GetID());

			if (shape->GetSubType() == EShapeSubType::Cylinder)
			{
				const auto sensorCylinderShape = static_cast<const CylinderShape*>(shape.GetPtr());

				const float halfHeight = sensorCylinderShape->GetHalfHeight();
				const float radius = sensorCylinderShape->GetRadius();

#ifndef NDEBUG
				static constexpr unsigned int NB_GEYSER_PARTICLES = 100;
#else
				static constexpr unsigned int NB_GEYSER_PARTICLES = 2500;
#endif

				particleSystem.AddParticleZone(*entityManager,
				                               {
					                               .centerPosition = transform.position,
					                               .halfExtends = {radius, halfHeight, radius},
					                               .nbParticle = NB_GEYSER_PARTICLES,
					                               .particleDurationMin = 0.5f,
					                               .particleDurationMax = 1.0f,
												   .particleSpeedMin = 5.0f,
												   .particleSpeedMax = 12.0f,
					                               .particleDirection = MeshUtil::ToDirectX(sensor.direction),
					                               .billboardTexture = bubbleTexture,
					                               .billboardShader = bubbleShader
				                               }
				);
			}
		}
	}
}

void GameEngine::BuildPauseMenu()
{
	uiManager->Clear();
	// Pause title
	auto sprite = uiManager->LoadSprite("assets/ui/pauseTitle.png");
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->AddSprite({
		.sprite = sprite,
	});

	// Resume Button
	sprite = uiManager->LoadSprite("assets/ui/resumeButton.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	auto hoverSprite = uiManager->LoadSprite("assets/ui/resumeButtonHovered.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(hoverSprite, "center", "center");
	uiManager->AddSprite({
		.sprite = sprite,
		.hoverSprite = hoverSprite,
		.onClick = [this] { ChangeGameStatus(); }
	});

	// Restart Button
	sprite = uiManager->LoadSprite("assets/ui/restartButton.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteX(sprite, -1.1f * sprite.texture.width);
	hoverSprite = uiManager->LoadSprite("assets/ui/restartButtonHovered.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(hoverSprite, "center", "center");
	uiManager->TranslateSpriteX(hoverSprite, -1.1f * hoverSprite.texture.width);
	uiManager->AddSprite({
		.sprite = sprite,
		.hoverSprite = hoverSprite,
		.onClick = [this] { RestartGame(); }
	});

	// Quit Button
	sprite = uiManager->LoadSprite("assets/ui/quitButton.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteX(sprite, 1.1f * sprite.texture.width);
	hoverSprite = uiManager->LoadSprite("assets/ui/quitButtonHovered.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(hoverSprite, "center", "center");
	uiManager->TranslateSpriteX(hoverSprite, 1.1f * hoverSprite.texture.width);
	uiManager->AddSprite({
		.sprite = sprite,
		.hoverSprite = hoverSprite,
		.onClick = [this] { GameState::currentState = GameState::FINISHED; }
	});

	// Option Button
	auto oldHeigth = sprite.texture.height;
	sprite = uiManager->LoadSprite("assets/ui/optionsButton.png", 0.f, 0.f, 2.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteY(sprite, sprite.texture.height / 2.f + oldHeigth / 2.f + 15.f);
	hoverSprite = uiManager->LoadSprite("assets/ui/optionsButtonHovered.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(hoverSprite, "center", "center");
	uiManager->TranslateSpriteY(hoverSprite, hoverSprite.texture.height / 2.f + oldHeigth / 2.f + 15.f);
	uiManager->AddSprite({
		.sprite = sprite,
		.hoverSprite = hoverSprite,
		.onClick = [this] { BuildOptionMenu(); }
	});
}

void GameEngine::BuildOptionMenu()
{
	uiManager->Clear();

	// Option title
	Sprite2D sprite = uiManager->LoadSprite("assets/ui/optionsTitle.png");
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->AddSprite({
		.sprite = sprite,
	});

	// Back button
	sprite = uiManager->LoadSprite("assets/ui/backButton.png", 0.0f, 0.0f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteY(sprite, sprite.texture.height / 2.f + 90.0f);
	auto spriteHover = uiManager->LoadSprite("assets/ui/backButtonHovered.png", 0.0f, 0.0f, 1.0f);
	uiManager->AlignSpriteXY(spriteHover, "center", "center");
	uiManager->TranslateSpriteY(spriteHover, spriteHover.texture.height / 2.f + 90.0f);
	uiManager->AddSprite({
		.sprite = sprite,
		.hoverSprite = spriteHover,
		.onClick = [this] { PauseGame(); }
	});

	// Camera inversion
	bool isChecked = Camera::invertCamRotation;
	std::string spriteFile = (isChecked) ? "assets/ui/checkedBox.png" : "assets/ui/uncheckedBox.png";
	std::string clickFile = (!isChecked) ? "assets/ui/checkedBox.png" : "assets/ui/uncheckedBox.png";
	sprite = uiManager->LoadSprite(spriteFile, 0.0f, 0.0f, 1.0f);
	Sprite2D clickSprite = uiManager->LoadSprite(clickFile, 0.0f, 0.0f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->AlignSpriteXY(clickSprite, "center", "center");
	std::string hoverFile = (isChecked) ? "assets/ui/checkedBoxHovered.png" : "assets/ui/uncheckedBoxHovered.png";
	std::string clickedHoverFile = (!isChecked) ? "assets/ui/checkedBoxHovered.png" : "assets/ui/uncheckedBoxHovered.png";
	spriteHover = uiManager->LoadSprite(hoverFile, 0.0f, 0.0f, 1.0f);
	auto clickSpriteHover = uiManager->LoadSprite(clickedHoverFile, 0.0f, 0.0f, 1.0f);
	uiManager->AlignSpriteXY(spriteHover, "center", "center");
	uiManager->AlignSpriteXY(clickSpriteHover, "center", "center");
	uiManager->AddSprite({
		.sprite = sprite,
		.hoverSprite = spriteHover,
		.clickSprite = clickSprite,
		.clickHoverSprite = clickSpriteHover,
		.clickDelay = 0.1f,
		.onClick = []
		{
			Camera::invertCamRotation ^= 1;
			std::cout << Camera::invertCamRotation << std::endl;
		}, // Theo's dark magic for boolean inversion
		.isCheckBox = true,
	});
}

void GameEngine::BuildEndMenu()
{
	uiManager->Clear();

	const std::string spriteFile = (GameState::currentState == GameState::DIED)
		                               ? "assets/ui/deathTitle.png"
		                               : "assets/ui/winTitle.png";
	Sprite2D sprite = uiManager->LoadSprite(spriteFile);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->AddSprite({
		.sprite = sprite,
	});

	// Restart Button
	sprite = uiManager->LoadSprite("assets/ui/restartButton.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteX(sprite, -.55f * sprite.texture.width);
	auto spriteHover = uiManager->LoadSprite("assets/ui/restartButtonHovered.png", 0.0f, 0.0f, 1.0f);
	uiManager->AlignSpriteXY(spriteHover, "center", "center");
	uiManager->TranslateSpriteX(spriteHover, -.55f * sprite.texture.width);
	uiManager->AddSprite({
		.sprite = sprite,
		.hoverSprite = spriteHover,
		.onClick = [this] { RestartGame(); }
	});

	// Quit Button
	sprite = uiManager->LoadSprite("assets/ui/quitButton.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteX(sprite, .55f * sprite.texture.width);
	spriteHover = uiManager->LoadSprite("assets/ui/quitButtonHovered.png", 0.0f, 0.0f, 1.0f);
	uiManager->AlignSpriteXY(spriteHover, "center", "center");
	uiManager->TranslateSpriteX(spriteHover, .55f * sprite.texture.width);
	uiManager->AddSprite({
		.sprite = sprite,
		.hoverSprite = spriteHover,
		.onClick = [this] { GameState::currentState = GameState::FINISHED; }
	});
}
