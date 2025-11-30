#include "pch.h"
#include "GameEngine.h"

#include <tuple>
#include <algorithm>
#include "GameState.h"
#include "entities/EntityManagerFactory.h"
#include "Locator.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/texture/TextureLoader.h"
#include "gameplay/mechanics/Eating.h"
#include "systems/PowerSystem.h"

using namespace DirectX;

GameEngine::GameEngine(RenderContext* renderContext)
	: renderContext{renderContext},
	  uiManager{std::make_shared<UIManager>(renderContext->GetDevice())}
{
	CameraSystem::SetMouseCursor();

	auto& sceneResources = Locator::Get<ResourceManager>().LoadScene();

	// Care about the order of construction, it will be the order of update calls
	systems.emplace_back(std::make_unique<PhysicsSimulationSystem>());
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

		if (GameState::currentState == GameState::FINISHED) return;

		const DWORD frameStartTime = GetTickCount();
		const auto isGamePaused = GameState::currentState != GameState::PLAYING;

		const double elapsedTime = isGamePaused ? 0.0 : (frameStartTime - prevTime) / 1000.0;

		if (not isGamePaused) [[likely]]
		{
			prevTime = frameStartTime;
			GameState::playTime += 1.0f / TARGET_FPS;
		}

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
	if (GameState::isGrowing)
		Eating::UpdatePlayerScale(entityManager);

	// Check if things get eaten
	HandleCollions();

	static bool wasEscapePressed = false;
	const bool isEscapePressed = GetAsyncKeyState(VK_ESCAPE) & 0x8000;
	const bool isPausableOrResumable = GameState::currentState == GameState::PLAYING || GameState::currentState ==
		GameState::PAUSED;

	// Handle clicks
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && GameState::currentState != GameState::PLAYING)
		uiManager->HandleClick();

	// Restart the game if has been was pressed
	if (GetAsyncKeyState('R') & 0x8000 && GameState::currentState != GameState::PAUSED)
	{
		uiManager->Clear();
		RestartGame();
	}

	if (isEscapePressed && !wasEscapePressed && isPausableOrResumable)
		ChangeGameStatus();

	wasEscapePressed = isEscapePressed;
}

void GameEngine::HandleCollions()
{
	while (!GameState::detectedCollisions.empty())
	{
		auto& [bodyId1, bodyId2] = GameState::detectedCollisions.front();
		GameState::detectedCollisions.pop();
		Eating::Eat(entityManager, bodyId1, bodyId2);
		if (GameState::currentState != GameState::PLAYING) ChangeGameStatus();
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

	// TODO: revise this
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

	const auto cameraEntity = entityManager.CreateEntity();
	auto& cameraComponent = entityManager.AddComponent<Camera>(cameraEntity, camera);

	GameState::currentCameraEntity = cameraEntity;
	GameState::postProcessSettings = {};

	// Assign the controllable entity to the camera (it's not a pretty way of doing it but it works)
	unsigned short nbControllable = 0;
	for (const auto& [entity, controllable] : entityManager.View<Controllable>())
	{
		++nbControllable;
		vassert(nbControllable == 1, "Currently, it's only possible to have 1 controllable entity.");

		cameraComponent.targetEntity = entity;
	}

	// Create a billboard
	static const auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();
	static Billboard dieBillboard
	(
		ShaderProgram
		{
			renderContext->GetDevice(), shaderBank.Get<VertexShader>("shaders/BillboardVS.hlsl"),
			shaderBank.Get<PixelShader>("shaders/BillboardPS.hlsl")
		},
		TextureLoader::LoadTextureFromFile("assets/textures/de.png", renderContext->GetDevice()),
		renderContext->GetDevice(),
		{0.0f, 700.0f, 0.0f},
		{50, 50},
		true
	);

	const auto dieBillboardEntity = entityManager.CreateEntity();
	entityManager.AddComponent<Billboard>(dieBillboardEntity, dieBillboard);

	mainMenuEntity = entityManager.CreateEntity();

	GameState::playTime = 0.0f;
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
	uiManager->AddSprite({
		.sprite = sprite,
		.onClick = [this] { ChangeGameStatus(); }
	});

	// Restart Button
	sprite = uiManager->LoadSprite("assets/ui/restartButton.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteX(sprite, - 1.1f * sprite.texture.width);
	uiManager->AddSprite({
		.sprite = sprite,
		.onClick = [this] { RestartGame(); }
	});

	// Quit Button
	sprite = uiManager->LoadSprite("assets/ui/quitButton.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteX(sprite, 1.1f * sprite.texture.width);
	uiManager->AddSprite({
		.sprite = sprite,
		.onClick = [this] { GameState::currentState = GameState::FINISHED; }
	});

	// Option Button
	auto oldHeigth = sprite.texture.height;
	sprite = uiManager->LoadSprite("assets/ui/optionsButton.png", 0.f, 0.f, 2.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteY(sprite, sprite.texture.height / 2.f + oldHeigth / 2.f + 15.f);
	uiManager->AddSprite({
		.sprite = sprite,
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
	uiManager->AddSprite({
		.sprite = sprite,
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
	uiManager->AddSprite({
		.sprite = sprite,
		.clickSprite = clickSprite,
		.clickDelay = 0.1f,
		.onClick = [] {
			Camera::invertCamRotation ^= 1;
			std::cout << Camera::invertCamRotation << std::endl;
		}, // Theo's dark magic for boolean inversion
		.isCheckBox = true,
	});
}

void GameEngine::BuildEndMenu()
{
	uiManager->Clear();

	const std::string spriteFile = (GameState::currentState == GameState::DIED) ? "assets/ui/deathTitle.png" : "assets/ui/winTitle.png";
	Sprite2D sprite = uiManager->LoadSprite(spriteFile);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->AddSprite({
			.sprite = sprite,
	});

	// Restart Button
	sprite = uiManager->LoadSprite("assets/ui/restartButton.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteX(sprite, -.55f * sprite.texture.width);
	uiManager->AddSprite({
		.sprite = sprite,
		.onClick = [this] { RestartGame(); }
	});

	// Quit Button
	sprite = uiManager->LoadSprite("assets/ui/quitButton.png", 0.f, 0.f, 1.0f);
	uiManager->AlignSpriteXY(sprite, "center", "center");
	uiManager->TranslateSpriteX(sprite, .55f * sprite.texture.width);
	uiManager->AddSprite({
		.sprite = sprite,
		.onClick = [this] { GameState::currentState = GameState::FINISHED; }
	});
}
