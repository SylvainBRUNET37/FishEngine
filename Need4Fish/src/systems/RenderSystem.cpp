#include "pch.h"

#include "systems/RenderSystem.h"

#include <format>

#include "rendering/culling/FrustumCuller.h"

#include "GameState.h"
#include "Locator.h"
#include "resources/ResourceManager.h"

using namespace DirectX;
using namespace std;

RenderSystem::RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials)
	: renderer(renderContext, std::move(materials)),
	  frameBuffer(AddDirectionLightToFrameBuffer()),
	  renderContext(renderContext)
{

}

void RenderSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	const auto currentCamera = entityManager.Get<Camera>(GameState::currentCameraEntity);

	renderer.RenderScene();

	// Add point lights to the frame buffer
	frameBuffer.pointLightCount = 0;
	for (const auto& [entity, pointLight] : entityManager.View<PointLight>())
	{
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
	renderer.UpdateFrameBuffer(frameBuffer);

	for (const auto& [entity, transform, mesh] : entityManager.View<Transform, Mesh>())
	{
		// Check if the mesh should be rendered or not
		if (FrustumCuller::IsMeshCulled(mesh, transform, static_cast<BaseCameraData>(currentCamera)))
			continue;

		renderer.Render(mesh, renderContext->GetContext(), transform);
	}

	// Render sprites
	for (const auto& [entity, sprite] : entityManager.View<Sprite2D>())
		renderer.Render(sprite, renderContext->GetContext());

	const auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();

	renderer.RenderPostProcess
	(
		shaderBank.Get<VertexShader>("shaders/PostProcessVS.hlsl").shader,
		shaderBank.Get<PixelShader>("shaders/PostProcessPS.hlsl").shader
	);

	Present();
}

FrameBuffer RenderSystem::AddDirectionLightToFrameBuffer()
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
