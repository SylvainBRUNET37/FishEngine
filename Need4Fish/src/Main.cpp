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
#include "systems/RenderSystem.h"
#include "ResourceManager.h"

#include "entities/EntityManagerFactory.h"
#include "GameEngine.h"
#include <systems/PhysicsSimulationSystem.h>

#include "PhysicsEngine/utils/JoltUtils.h"
#include "systems/CameraSystem.h"

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
	physicsSystem.SetGravity(Vec3(0, -500, 0));

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

	// Care about the order of construction, it will be the order of update calls
	std::vector<std::unique_ptr<System>> systems;

	auto physicsSimulationSystem = std::make_unique<PhysicsSimulationSystem>();
	physicsSimulationSystem->Init();
	systems.emplace_back(std::move(physicsSimulationSystem));
	systems.emplace_back(std::make_unique<CameraSystem>());
	systems.emplace_back(std::make_unique<RenderSystem>(&renderContext, std::move(sceneResources.materials)));

	GameEngine gameEngine
	{
		std::move(entityManager),
		std::move(resourceManager),
		UIManager{renderContext.GetDevice()},
		std::move(systems)
	};

	CameraSystem::SetMouseCursor();
	gameEngine.Run();

	UnregisterTypes();

	return EXIT_SUCCESS;
}
