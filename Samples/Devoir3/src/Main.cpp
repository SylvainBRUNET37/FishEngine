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

#include "Components.h"
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

#include "EntityManagerFactory.h"
#include "Scene.h"

using namespace JPH;
using namespace JPH::literals;
using namespace std;
using namespace DirectX;

namespace
{
	XMMATRIX ToXMMATRIX(const RMat44& transform)
	{
		// Extract Jolt basis vectors and translation
		const Vec3 x = transform.GetAxisX();
		const Vec3 y = transform.GetAxisY();
		const Vec3 z = transform.GetAxisZ();
		const RVec3 p = transform.GetTranslation();

		// Construct a DirectX right-handed matrix
		return XMMatrixSet(
			x.GetX(), x.GetY(), x.GetZ(), 0.0f,
			y.GetX(), y.GetY(), y.GetZ(), 0.0f,
			z.GetX(), z.GetY(), z.GetZ(), 0.0f,
			p.GetX(),
			p.GetY(),
			p.GetZ(),
			1.0f
		);
	}

	void WaitBeforeNextFrame(const DWORD frameStartTime)
	{
		static constexpr double TARGET_FPS = 60.0;
		static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;

		const DWORD frameEnd = GetTickCount();
		const DWORD frameDuration = frameEnd - frameStartTime;

		if (frameDuration < FRAME_TIME)
			Sleep(static_cast<DWORD>(FRAME_TIME - frameDuration));
	}
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

	EntityManagerFactory<Transform, Mesh, Hierarchy, RigidBody> entityManagerFactory;
	auto entityManager = entityManagerFactory.Create(sceneResources);

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

	// CREATE CUBE

	BodyCreationSettings boxSettings(
		new BoxShape(Vec3(1.0f, 1.0f, 1.0f)),
		RVec3(0, 0, 0),
		Quat::sIdentity(),
		EMotionType::Dynamic,
		Layers::MOVING
	);

	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
	Body* body = bodyInterface.CreateBody(boxSettings);
	bodyInterface.AddBody(body->GetID(), EActivation::Activate);

	// TODO: delete
	Entity cubeEntity = { .index = 1, .generation = 1 };
	entityManager.AddComponent<RigidBody>(cubeEntity, body);

	//

	Scene scene;

	DWORD prevTime = GetTickCount();

	while (true)
	{
		const DWORD frameStartTime = GetTickCount();

		double elapsedTime = (frameStartTime - prevTime) / 1000.0f;
		prevTime = frameStartTime;

		if (not WindowsApplication::ProcessWindowMessages())
			break;

		scene.update(); // update physics

		renderSystem.UpdateScene(elapsedTime);

		for (const auto& [transform, mesh] : entityManager.View<Transform, Mesh>())
		{
			renderSystem.Render(mesh, transform);
		}

		for (const auto& [transform, rigidBody] : entityManager.View<Transform, RigidBody>())
		{
			transform.world = ToXMMATRIX(rigidBody.body->GetWorldTransform());
		}

		renderSystem.Render();

		WaitBeforeNextFrame(frameStartTime);
	}

	return EXIT_SUCCESS;
}
