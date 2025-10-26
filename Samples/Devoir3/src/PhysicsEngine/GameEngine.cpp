#include "GameEngine.h"

#include "ecs/Components.h"
#include "PhysicsEngine/systems/JoltSystem.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Mesh.h"

XMMATRIX ToXMMATRIX(const JPH::RMat44& transform)
{
	// Extract Jolt basis vectors and translation
	const JPH::Vec3 x = transform.GetAxisX();
	const JPH::Vec3 y = transform.GetAxisY();
	const JPH::Vec3 z = transform.GetAxisZ();
	const JPH::RVec3 p = transform.GetTranslation();

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

void GameEngine::Run()
{
	bool shouldContinue = true;
	DWORD prevTime = GetTickCount();

	while (shouldContinue)
	{
		const DWORD frameStartTime = GetTickCount();

		const double elapsedTime = (frameStartTime - prevTime) / 1000.0;
		prevTime = frameStartTime;

		shouldContinue = WindowsApplication::ProcessWindowMessages();

		UpdatePhysics();

		renderSystem.UpdateScene(elapsedTime);

		//for (const auto& [transform, mesh] : entityManager.View<Transform, Mesh>())
		//{
		//	renderSystem.Render(mesh, transform);
		//}

		//for (const auto& [transform, rigidBody] : entityManager.View<Transform, RigidBody>())
		//{
		//	transform.world = ToXMMATRIX(rigidBody.body->GetWorldTransform());
		//}

		renderSystem.Render();

		WaitBeforeNextFrame(frameStartTime);
	}
}

void GameEngine::UpdatePhysics()
{
	// Update physics
	constexpr int collisionSteps = 1;
	JoltSystem::GetPhysicSystem().Update(PHYSICS_UPDATE_RATE, collisionSteps,
		&JoltSystem::GetTempAllocator(),
		&JoltSystem::GetJobSystem());

	// Apply logic related to the physics simulation results
	for (auto& task : JoltSystem::GetPostStepCallbacks())
		task();

	JoltSystem::GetPostStepCallbacks().clear();
}

void GameEngine::WaitBeforeNextFrame(const DWORD frameStartTime)

{
	static constexpr double TARGET_FPS = 60.0;
	static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;

	const DWORD frameEnd = GetTickCount();
	const DWORD frameDuration = frameEnd - frameStartTime;

	if (frameDuration < FRAME_TIME)
		Sleep(static_cast<DWORD>(FRAME_TIME - frameDuration));
}