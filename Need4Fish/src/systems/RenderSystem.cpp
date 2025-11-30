#include "pch.h"

#include "systems/RenderSystem.h"

#include <format>

#include "rendering/culling/FrustumCuller.h"

#include "GameState.h"
#include "Locator.h"
#include "rendering/texture/TextureLoader.h"
#include "resources/ResourceManager.h"

using namespace DirectX;
using namespace std;

RenderSystem::RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials)
	: renderer(renderContext, std::move(materials)),
	  frameBuffer(CreateDirectionnalLight()),
	  renderContext(renderContext)
{
}

void RenderSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	const auto currentCamera = entityManager.Get<Camera>(GameState::currentCameraEntity);

	renderer.UpdateScene();

	// Add point lights to the frame buffer and update their position
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

	// Update frame buffer
	XMStoreFloat4x4(&frameBuffer.matViewProj, XMMatrixTranspose(currentCamera.matView * currentCamera.matProj));
	XMStoreFloat4(&frameBuffer.vCamera, currentCamera.position);

	static float elapsedTime = 0;
	elapsedTime += static_cast<float>(deltaTime);
	frameBuffer.elapsedTime = elapsedTime;

	renderer.UpdateFrameBuffer(frameBuffer);

	for (const auto& [entity, transform, meshInstance] : entityManager.View<Transform, MeshInstance>())
	{
		// Check if the mesh should be rendered or not
		auto& mesh = Locator::Get<ResourceManager>().GetMesh(meshInstance.meshIndex);
		if (FrustumCuller::IsMeshCulled(mesh, transform, static_cast<BaseCameraData>(currentCamera)))
			continue;

		renderer.Render(mesh, renderContext->GetContext(), transform);
	}

	// Render billboards
	renderer.PrepareSceneForBillboard();
	for (const auto& [entity, billboard] : entityManager.View<Billboard>())
		renderer.Render(billboard, renderContext->GetContext(), currentCamera);

	// Apply distortion effect
	renderer.PrepareSceneForDistortion();
	for (const auto& [entity, transform, distortionMeshInstance] : entityManager.View<Transform, DistortionMeshInstance>())
	{
		// Check if the mesh should be rendered or not
		auto& mesh = Locator::Get<ResourceManager>().GetMesh(distortionMeshInstance.meshIndex);
		if (FrustumCuller::IsMeshCulled(mesh, transform, static_cast<BaseCameraData>(currentCamera)))
			continue;

		renderer.Render(mesh, renderContext->GetContext(), transform);
	}

	GameState::postProcessSettings.enableVignette = Camera::mode == Camera::CameraMode::FIRST_PERSON ? 1 : 0;
	GameState::postProcessSettings.deltaTime = deltaTime;

	const auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();
	renderer.RenderPostProcess
	(
		shaderBank.Get<VertexShader>("shaders/PostProcessVS.hlsl").shader,
		shaderBank.Get<PixelShader>("shaders/PostProcessPS.hlsl").shader,
		GameState::postProcessSettings
	);

	// Render sprites
	for (const auto& [entity, sprite] : entityManager.View<Sprite2D>())
		renderer.Render(sprite, renderContext->GetContext());

	Present();
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
