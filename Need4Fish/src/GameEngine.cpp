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
#include "gameplay/mechanics/Eating.h"
#include "systems/PowerSystem.h"

using namespace DirectX;

GameEngine::GameEngine(RenderContext* renderContext)
	: uiManager(std::make_shared<UIManager>(renderContext->GetDevice()))
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
	if (GameState::isGrowing)
		Eating::UpdatePlayerScale(entityManager);

	// Check if things get eaten
	HandleCollions();

	static bool wasEscapePressed = false;
	const bool isEscapePressed = GetAsyncKeyState(VK_ESCAPE) & 0x8000;
	const bool isPausableOrResumable = GameState::currentState == GameState::PLAYING || GameState::currentState == GameState::PAUSED;

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

void GameEngine::HandleCollions() {
	
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
	Camera::firstPersonOffset = { 0.0f,-1.7f,35.0f };

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

	mainMenuEntity = entityManager.CreateEntity();
}

void GameEngine::BuildPauseMenu()
{
	uiManager->Clear();
	// Pause title
	uiManager->AddSprite({
		.sprite = uiManager->LoadSprite("assets/ui/testPause.png"),
	});

	// Resume Button
	uiManager->AddSprite({
		.sprite = uiManager->LoadSprite("assets/ui/testResume.png", 0.0f, 300.0f, 0.0f),
		.onClick = [this] { ChangeGameStatus(); }
	});

	// Option Button
	uiManager->AddSprite({
		.sprite = uiManager->LoadSprite("assets/ui/testOptions.png", 0.f, 600.0f, 0.0f),
		.onClick = [this] { BuildOptionMenu(); }
	});
}

void GameEngine::BuildOptionMenu()
{
	uiManager->Clear();

	// Option title
	uiManager->AddSprite({
		.sprite = uiManager->LoadSprite("assets/ui/testOptions.png"),
	});

	// Back button
	uiManager->AddSprite({
		.sprite = uiManager->LoadSprite("assets/ui/testBack.png", 800.0f, 500.0f, 0.0f),
		.onClick = [this] { PauseGame(); }
	});

	// Camera inversion
	bool isChecked = Camera::invertCamRotation;
	std::string spriteFile = (isChecked) ? "assets/ui/testChecked.png" : "assets/ui/testUnchecked.png";
	std::string clickFile = (!isChecked) ? "assets/ui/testChecked.png" : "assets/ui/testUnchecked.png";
	uiManager->AddSprite({
		.sprite = uiManager->LoadSprite(spriteFile, 800.0f, 200.0f, 0.0f),
		.clickSprite = uiManager->LoadSprite(clickFile, 800.0f, 200.0f, 0.0f),
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

	const std::string sprite = (GameState::currentState == GameState::DIED) ? "assets/ui/testDeath.png" : "assets/ui/testWin.png";
	uiManager->AddSprite({
			.sprite = uiManager->LoadSprite(sprite),
	});

	float positionX = 400.0f;
	float positionY = 400.0f;
	float positionZ = 0.0f;
	uiManager->AddSprite({
		.sprite = uiManager->LoadSprite("assets/ui/testRestart.png", positionX, positionY, positionZ),
		.onClick = [this] { RestartGame(); }
	});
}