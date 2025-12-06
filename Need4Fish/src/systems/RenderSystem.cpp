#include "pch.h"

#include "GameState.h"
#include "Locator.h"
#include "rendering/culling/FrustumCuller.h"
#include "rendering/texture/TextureLoader.h"
#include "resources/ResourceManager.h"
#include "systems/RenderSystem.h"

#include <format>
#include <Jolt/Physics/Body/Body.h>

using namespace DirectX;
using namespace std;

RenderSystem::RenderSystem(RenderContext* renderContext, const std::shared_ptr<UIManager>& uiManager,
                           std::vector<Material>&& materials)
	: uiManager(uiManager),
	  renderer(renderContext, std::move(materials)),
	  frameBuffer(CreateDirectionnalLight()),
	  renderContext(renderContext),
	  particleData(BillboardRenderer::MAX_BILLBOARDS)
{
}

void RenderSystem::RenderPostProcesses(const double deltaTime)
{
	GameState::postProcessSettings.enableVignette =
		(Camera::mode == Camera::CameraMode::FIRST_PERSON || Camera::isTemporaryFirstPerson) ? 1 : 0;

	GameState::postProcessSettings.deltaTime = static_cast<float>(deltaTime);

	static const auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();
	static const auto vertexShader = shaderBank.Get<VertexShader>("shaders/PostProcessVS.hlsl").shader;
	static const auto pixelShader = shaderBank.Get<PixelShader>("shaders/PostProcessPS.hlsl").shader;

	renderer.RenderPostProcess
	(
		vertexShader,
		pixelShader,
		GameState::postProcessSettings
	);
}

void RenderSystem::ComputeDistortionZones(EntityManager& entityManager)
{
	// Apply distortion effect
	renderer.PrepareSceneForDistortion();
	for (const auto& [entity, transform, distortionMeshInstance] : entityManager.View<
		     Transform, DistortionMeshInstance>())
	{
		// Check if the mesh should be rendered or not
		auto& mesh = Locator::Get<ResourceManager>().GetMesh(distortionMeshInstance.meshIndex);
		if (FrustumCuller::IsMeshCulled(mesh, transform))
			continue;

		renderer.Render(mesh, renderContext->GetContext(), transform);
	}
}

void RenderSystem::RenderBillboards(EntityManager& entityManager, const Camera& currentCamera)
{
	// Render billboards
	renderer.PrepareSceneForBillboard();
	for (const auto& [entity, billboard] : entityManager.View<Billboard>())
	{
		const auto worldTransform =
			billboard.type == Billboard::Type::CameraFacing
				? billboard.ComputeCameraFacingBillboardWorldMatrix()
				: billboard.ComputeCylindricBillboardWorldMatrix();
		/* Culling has a higher cost than drawing a billboard, so it's disabled
		if (FrustumCuller::IsBillboardCulled(billboard, worldTransform))
			continue;
		*/
		renderer.Render(billboard, worldTransform, currentCamera);
	}
}

// Not very well coded, will be improved if needed
void RenderSystem::RenderParticles(EntityManager& entityManager, const Camera& currentCamera)
{
	renderer.PrepareSceneForBillboard();

	Billboard* billboard{};
	size_t i = 0;
	for (const auto& [entity, particle] : entityManager.View<Particle>())
	{
		if (i == 0) [[unlikely]] // store the billboard object only once
			billboard = &particle.billboard;

		// Assume that: scale X = scale Y
		particleData[i] = { particle.billboard.position, particle.billboard.scale.x };
		++i;
	}

	vassert(billboard, "There should be particles in the world since RenderParticles is called");

	renderer.RenderWithInstancing(*billboard, particleData, currentCamera);
}

void RenderSystem::RenderMeshes(EntityManager& entityManager)
{
	// Render Meshes
	for (const auto& [entity, transform, meshInstance] : entityManager.View<Transform, MeshInstance>())
	{
		// Check if the mesh should be rendered or not
		auto& mesh = Locator::Get<ResourceManager>().GetMesh(meshInstance.meshIndex);
		if (FrustumCuller::IsMeshCulled(mesh, transform))
			continue;

		renderer.Render(mesh, renderContext->GetContext(), transform);
	}
}

void RenderSystem::UpdatePointLights(EntityManager& entityManager)
{
	frameBuffer.pointLightCount = 0;
	for (const auto& [entity, pointLight, transform] : entityManager.View<PointLight, Transform>())
	{
		// Extract position
		const XMVECTOR lightPos = transform.world.r[3];

		// Store it as float3
		XMFLOAT3 lightPosFloat3;
		XMStoreFloat3(&lightPosFloat3, lightPos);

		// Update position
		pointLight.position = lightPosFloat3;

		if (frameBuffer.pointLightCount >= FrameBuffer::MAX_POINT_LIGHTS)
			throw runtime_error(
				std::format(
					"Cannot exceed {} point light, change the number in the C++ and in the shader if you want more lights",
					FrameBuffer::MAX_POINT_LIGHTS));

		frameBuffer.pointLights[frameBuffer.pointLightCount++] = pointLight;
	}
}

void RenderSystem::UpdateFrameBuffer(const double deltaTime, EntityManager& entityManager, const Camera& currentCamera)
{
	// Add point lights to the frame buffer and update their position
	UpdatePointLights(entityManager);

	// Update frame buffer
	XMStoreFloat4x4(&frameBuffer.matViewProj, XMMatrixTranspose(currentCamera.matView * currentCamera.matProj));
	XMStoreFloat4(&frameBuffer.vCamera, Camera::position);

	static float elapsedTime = 0;
	elapsedTime += static_cast<float>(deltaTime);
	frameBuffer.elapsedTime = elapsedTime;

	renderer.UpdateFrameBuffer(frameBuffer);
}

void RenderSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	const auto currentCamera = entityManager.Get<Camera>(GameState::currentCameraEntity);
	FrustumCuller::Init(static_cast<BaseCameraData>(currentCamera)); // Prepare the frustum culler

	renderer.UpdateScene();

	UpdateFrameBuffer(deltaTime, entityManager, currentCamera);

	RenderMeshes(entityManager);
	RenderBillboards(entityManager, currentCamera);
	RenderParticles(entityManager, currentCamera);

	ComputeDistortionZones(entityManager);
	RenderPostProcesses(deltaTime);

	RenderUI(entityManager);

	Present();

	renderer.ClearPixelShaderResources();
}

void RenderSystem::RenderUI(EntityManager& entityManager)
{
	// Text Addition (create sprite)
	const auto watchables = entityManager.View<Controllable, Eatable, RigidBody>();
	int playerMass;
	float playerSpeed;
	for (const auto& [_, __, eatable, rigiBody] : watchables)
	{
		playerMass = eatable.mass;
		playerSpeed = rigiBody.body->GetLinearVelocity().Length();
		break;
	}
	const auto text = std::format(L"Player mass : {}\nPlayer speed : {:.2f}\nPlaytime : {:.2f}", playerMass,
	                              playerSpeed, GameState::playTime);
	uiManager->RenderText(text);

	// Render sprites
	renderer.PrepareSceneForSprite();
	for (auto& sprite : uiManager->GetSprites())
		renderer.Render(sprite, renderContext->GetContext());
}

FrameBuffer RenderSystem::CreateDirectionnalLight()
{
	return
	{
		.dirLight =
		{
			.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f),
			.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),

			.direction = XMFLOAT3(-0.5f, -1.0f, 0.5f),
			.pad = 0.0f
		},
	};
}
