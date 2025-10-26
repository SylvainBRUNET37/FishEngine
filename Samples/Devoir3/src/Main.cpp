#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdlib>
#include <minwindef.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Mat44.h>

#include "ecs/Components.h"
#include "PhysicsEngine/layers/BroadPhaseLayerInterfaceImpl.h"
#include "PhysicsEngine/layers/BroadPhaseLayers.h"
#include "PhysicsEngine/layers/ObjectLayerPairFilterImpl.h"
#include "PhysicsEngine/layers/ObjectVsBroadPhaseLayerFilterImpl.h"
#include "PhysicsEngine/listeners/BodyActivationListenerLogger.h"
#include "PhysicsEngine/listeners/ContactListenerImpl.h"
#include "PhysicsEngine/systems/JoltSystem.h"
#include "rendering/SceneLoader.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/device/DeviceBuilder.h"
#include "rendering/device/RenderContext.h"
#include "rendering/shaders/ShaderProgramDesc.h"
#include "rendering/RenderSystem.h"
#include "ResourceManager.h"

#include "ecs/EntityManagerFactory.h"
#include "GameEngine.h"
#include "PhysicsEngine/ShapeFactory.h"

using namespace JPH;
using namespace JPH::literals;
using namespace std;
using namespace DirectX;

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

	// Create the scene

	Body* planeBody = ShapeFactory::CreatePlane();
	Body* cubeBody = ShapeFactory::CreateCube();

	for (const auto& [entity, name] : entityManager.View<Name>())
	{
		if (name.name == "Cube")
			entityManager.AddComponent<RigidBody>(entity, cubeBody);
		else if (name.name == "Plane")
			entityManager.AddComponent<RigidBody>(entity, planeBody);
	}

	GameEngine gameEngine{ std::move(renderSystem), std::move(entityManager) };
	gameEngine.Run();

	return EXIT_SUCCESS;
}
