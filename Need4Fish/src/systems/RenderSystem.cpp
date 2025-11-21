#include "pch.h"

#include "systems/RenderSystem.h"

#include <format>

#include "rendering/culling/FrustumCuller.h"

#include "GameState.h"
#include "Locator.h"
#include "resources/ResourceManager.h"

using namespace DirectX;
using namespace std;

RenderSystem::RenderSystem(RenderContext* renderContext, std::shared_ptr<UIManager> uiManager, std::vector<Material>&& materials)
	: renderer(renderContext, std::move(materials)),
	  uiManager(uiManager),
	  frameBuffer(AddDirectionLightToFrameBuffer()),
	  renderContext(renderContext)
{
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	DXEssayer(renderContext->GetDevice()->CreateSamplerState(&sampDesc, &samplerState));
}

void RenderSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	const auto currentCamera = entityManager.Get<Camera>(GameState::currentCameraEntity);

	renderer.RenderScene();

	// Add point lights to the frame buffer
	int lightCount = 0;
	for (const auto& [entity, pointLight] : entityManager.View<PointLight>())
	{
		if (lightCount >= FrameBuffer::MAX_POINT_LIGHTS)
			throw runtime_error(std::format("Cannot exceed {} point light", FrameBuffer::MAX_POINT_LIGHTS));

		frameBuffer.pointLights[lightCount++] = pointLight;
	}

	// Update frame buffer
	XMStoreFloat4x4(&frameBuffer.matViewProj, XMMatrixTranspose(currentCamera.matView * currentCamera.matProj));
	XMStoreFloat4(&frameBuffer.vCamera, currentCamera.position);
	renderer.UpdateFrameBuffer(frameBuffer);

	renderContext->GetContext()->PSSetSamplers(0, 1, &samplerState);

	for (const auto& [entity, transform, mesh] : entityManager.View<Transform, Mesh>())
	{
		// Check if the mesh should be rendered or not
		if (FrustumCuller::IsMeshCulled(mesh, transform, static_cast<BaseCameraData>(currentCamera)))
			continue;

		renderer.Render(mesh, renderContext->GetContext(), transform);
	}

	// Render sprites
	cout << uiManager->GetSprites().size() << endl;
	for (auto& sprite : uiManager->GetSprites())
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
