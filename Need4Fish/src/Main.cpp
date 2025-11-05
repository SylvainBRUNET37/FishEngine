#include "pch.h"

#include <Jolt/RegisterTypes.h>

#include "components/Components.h"
#include "PhysicsEngine/layers/BroadPhaseLayerInterfaceImpl.h"
#include "PhysicsEngine/layers/BroadPhaseLayers.h"
#include "PhysicsEngine/layers/ObjectLayerPairFilterImpl.h"
#include "PhysicsEngine/layers/ObjectVsBroadPhaseLayerFilterImpl.h"
#include "PhysicsEngine/listeners/BodyActivationListenerImpl.h"
#include "PhysicsEngine/listeners/ContactListenerImpl.h"
#include "PhysicsEngine/JoltSystem.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/device/DeviceBuilder.h"
#include "rendering/device/RenderContext.h"
#include "rendering/shaders/ShaderProgramDesc.h"
#include "systems/RenderSystem.h"
#include "ResourceManager.h"

#include "entities/EntityManagerFactory.h"
#include "GameEngine.h"
#include "PhysicsEngine/ShapeFactory.h"
#include <systems/PhysicsSimulationSystem.h>

#include "PhysicsEngine/utils/JoltUtils.h"
#include "systems/CameraSystem.h"
#include <GameState.h>

using namespace JPH;
using namespace JPH::literals;
using namespace std;
using namespace DirectX;

int APIENTRY _tWinMain(const HINSTANCE hInstance,
                       HINSTANCE,
                       LPTSTR,
                       int)
{
	// Source: https://stackoverflow.com/questions/16703835/how-can-i-see-cout-output-in-a-non-console-application
	AllocConsole(); // D�commenter si la console est voulu
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	WindowsApplication application{hInstance, L"RenderingEngineSample", L"RenderingEngineSampleClass"};

	if (!application.Init())
		return EXIT_FAILURE;

	const auto windowData = application.GetWindowData();

	auto renderContext = DeviceBuilder::CreateRenderContext(application.GetMainWindow(), windowData);

	ResourceManager resourceManager{renderContext.GetDevice()};
	auto sceneResources = resourceManager.LoadScene();

	auto entityManager = EntityManagerFactory::Create(sceneResources);


	/////	Physics System	 /////
	JoltSystem::Init();
	auto& physicsSystem = JoltSystem::GetPhysicSystem();

#ifndef NDEBUG
	AssertFailed = JoltUtils::AssertFailedImpl;
#endif

	const BroadPhaseLayerInterfaceImpl broadPhaseLayerInterface;
	const ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
	const ObjectLayerPairFilterImpl objectLayerPairFilter;
	physicsSystem.Init(1024, 0, 1024, 1024,
	                   broadPhaseLayerInterface, objectVsBroadPhaseLayerFilter, objectLayerPairFilter);

	PhysicsSettings settings = physicsSystem.GetPhysicsSettings();
	settings.mMinVelocityForRestitution = 0.01f;
	physicsSystem.SetPhysicsSettings(settings);

	BodyActivationListenerImpl bodyActivationListener;
	physicsSystem.SetBodyActivationListener(&bodyActivationListener);

	ContactListenerImpl contactListener;
	physicsSystem.SetContactListener(&contactListener);
	//////////////////////////////

	Camera camera
	{
		.position = XMVectorSet(0, 5, -10, 1),
		.focus = XMVectorSet(0, 0, 0, 1),
		.up = XMVectorSet(0, 1, 0, 0),
		.aspectRatio = static_cast<float>(windowData.screenWidth) / static_cast<float>(windowData.screenHeight),
		.distance = 100.f,
		.heightOffset = 30.f,
	};
	const auto cameraEntity = entityManager.CreateEntity();
	auto& cameraComponent = entityManager.AddComponent<Camera>(cameraEntity, camera);

	// Initialize the scene (it's a temporary way of doing it)
	for (const auto& [entity, name] : entityManager.View<Name>())
	{
		if (name.name == "Cube")
		{
			const auto transform = entityManager.Get<Transform>(entity);
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCube(transform));
			entityManager.AddComponent<Controllable>(entity, 100.0f);

			// Link camera to the cube
			cameraComponent.targetEntity = entity;
		}
		else if (name.name == "Cargo")
		{
			const auto transform = entityManager.Get<Transform>(entity);
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCube(transform));
		}
		else if (name.name == "Plane")
		{
			const auto transform = entityManager.Get<Transform>(entity);
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreatePlane(transform));
		}
	}

	// Care about the order of construction, it will be the order of update calls
	std::vector<std::unique_ptr<System>> systems;
	systems.emplace_back(std::make_unique<PhysicsSimulationSystem>());
	systems.emplace_back(std::make_unique<CameraSystem>());
	systems.emplace_back(std::make_unique<RenderSystem>(&renderContext, std::move(sceneResources.materials)));

	GameEngine gameEngine
	{
		std::move(entityManager),
		std::move(resourceManager),
		std::move(systems)
	};

	GameEngine::currentCameraEntity = cameraEntity;

	CameraSystem::SetMouseCursor();
	gameEngine.Run();

	UnregisterTypes();

	return EXIT_SUCCESS;
}
