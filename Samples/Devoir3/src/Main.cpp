#include "pch.h"

#include <Jolt/RegisterTypes.h>

#include "entityComponentSystem/Components.h"
#include "PhysicsEngine/layers/BroadPhaseLayerInterfaceImpl.h"
#include "PhysicsEngine/layers/BroadPhaseLayers.h"
#include "PhysicsEngine/layers/ObjectLayerPairFilterImpl.h"
#include "PhysicsEngine/layers/ObjectVsBroadPhaseLayerFilterImpl.h"
#include "PhysicsEngine/listeners/BodyActivationListenerLogger.h"
#include "PhysicsEngine/listeners/ContactListenerImpl.h"
#include "PhysicsEngine/systems/JoltSystem.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/device/DeviceBuilder.h"
#include "rendering/device/RenderContext.h"
#include "rendering/shaders/ShaderProgramDesc.h"
#include "rendering/RenderSystem.h"
#include "ResourceManager.h"

#include "entityComponentSystem/EntityManagerFactory.h"
#include "GameEngine.h"
#include "PhysicsEngine/ShapeFactory.h"

using namespace JPH;
using namespace JPH::literals;
using namespace std;
using namespace DirectX;

static bool MyAssertFailed(const char* inExpression, const char* inMessage, const char* inFile, const uint inLine)
{
	std::cerr << "Jolt Assertion Failed!\n";
	std::cerr << "Expression: " << inExpression << "\n";
	std::cerr << "Message: " << (inMessage ? inMessage : "(none)") << "\n";
	std::cerr << "File: " << inFile << ":" << inLine << "\n";

	assert(false && "Jolt Assertion Failed. See error output for more information.");
	return false;
}

int APIENTRY _tWinMain(const HINSTANCE hInstance,
                       HINSTANCE,
                       LPTSTR,
                       int)
{
	WindowsApplication application{hInstance, L"RenderingEngineSample", L"RenderingEngineSampleClass"};

	if (!application.Init())
		return EXIT_FAILURE;

	const auto windowData = application.GetWindowData();

	auto renderContext = DeviceBuilder::CreateRenderContext(application.GetMainWindow(), windowData);

	ResourceManager resourceManager{renderContext.GetDevice()};
	auto sceneResources = resourceManager.LoadScene();

	RenderSystem renderSystem{&renderContext, std::move(sceneResources.materials)};
	auto entityManager = EntityManagerFactory::Create(sceneResources);

	/////	Physics System	 /////
	JoltSystem::Init();
	auto& physicsSystem = JoltSystem::GetPhysicSystem();

	AssertFailed = MyAssertFailed;

	const BroadPhaseLayerInterfaceImpl broadPhaseLayerInterface;
	const ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
	const ObjectLayerPairFilterImpl objectLayerPairFilter;
	physicsSystem.Init(1024, 0, 1024, 1024,
	                   broadPhaseLayerInterface, objectVsBroadPhaseLayerFilter, objectLayerPairFilter);

	PhysicsSettings settings = physicsSystem.GetPhysicsSettings();
	settings.mMinVelocityForRestitution = 0.01f;
	physicsSystem.SetPhysicsSettings(settings);

	BodyActivationListenerLogger bodyActivationListener;
	physicsSystem.SetBodyActivationListener(&bodyActivationListener);

	ContactListenerImpl contactListener;
	physicsSystem.SetContactListener(&contactListener);
	//////////////////////////////

	// Initialize the scene (it's a temporary way of doing it)
	for (const auto& [entity, name] : entityManager.View<Name>())
	{
		if (name.name == "Cube")
		{
			const auto transform = entityManager.Get<Transform>(entity);
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCubeInVehicleLayer(transform));
			entityManager.AddComponent<BallShooter>(entity);
			entityManager.AddComponent<Controllable>(entity, 100.0f);
		}
		else if (name.name == "Cargo")
		{
			const auto transform = entityManager.Get<Transform>(entity);
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCubeInCargoLayer(transform));
		}
		else if (name.name == "Plane")
		{
			const auto transform = entityManager.Get<Transform>(entity);
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreatePlane(transform));
		}
		else if (name.name == "Capsule")
		{
			const auto transform = entityManager.Get<Transform>(entity);
			entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCapsule(transform));
		}
	}

	GameEngine gameEngine{ std::move(renderSystem), std::move(entityManager), std::move(resourceManager) };

	// Source: https://stackoverflow.com/questions/16703835/how-can-i-see-cout-output-in-a-non-console-application
	AllocConsole();// D�commenter si la console est voulu
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	cout << "Controles\n- W et S pour avancer et reculer\n- A et D pour strafe a gauche et a droite\n- Q et E pour pivoter\n- Barre d'espace pour tirer les balles\n\n";

	gameEngine.Run();

	UnregisterTypes();

	return EXIT_SUCCESS;
}
