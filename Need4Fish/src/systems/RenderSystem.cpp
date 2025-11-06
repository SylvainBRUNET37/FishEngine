#include "pch.h"

#include "systems/RenderSystem.h"
#include "rendering/culling/FrustumCuller.h"

#include "GameEngine.h"

using namespace DirectX;
using namespace std;

RenderSystem::RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials)
	: renderer(renderContext->GetDevice(), std::move(materials)),
	  frameBuffer(AddDirectionLightToFrameBuffer()),
	  renderContext(renderContext)
{
}

void RenderSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	const auto currentCamera = entityManager.Get<Camera>(GameEngine::currentCameraEntity);

	RenderScene();

	// Add point lights to the frame buffer
	int lightCount = 0;
	for (const auto& [entity, pointLight] : entityManager.View<PointLight>())
	{
		if (lightCount >= FrameBuffer::MAX_POINT_LIGHTS)
			throw runtime_error(format("Cannot exceed {} point light", FrameBuffer::MAX_POINT_LIGHTS));

		frameBuffer.pointLights[lightCount++] = pointLight;
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
	for (const auto& [entity, sprite] : entityManager.View<Sprite>())
		renderer.Render(sprite, renderContext->GetContext());

	Present();
}

void RenderSystem::RenderScene() const
{
	ID3D11DeviceContext* context = renderContext->GetContext();
	ID3D11RenderTargetView* renderTarget = renderContext->GetRenderTargetView();
	ID3D11DepthStencilView* depthStencil = renderContext->GetDepthStencilView();

	constexpr float backgroundColor[4] = {0.0f, 0.0f, 0.5f, 1.0f};
	context->ClearRenderTargetView(renderTarget, backgroundColor);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* rtvs[] = {renderTarget};
	context->OMSetRenderTargets(1, rtvs, depthStencil);
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

            .direction = XMFLOAT3(-0.5f, 1.0f, -0.5f),
            .pad = 0.0f
        },
    };
}

