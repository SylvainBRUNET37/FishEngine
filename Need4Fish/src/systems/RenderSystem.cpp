#include "pch.h"

#include "systems/RenderSystem.h"

#include "GameEngine.h"

using namespace DirectX;

RenderSystem::RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials)
	: renderer(renderContext->GetDevice(), std::move(materials), frameCbRegisterNumber, objectCbRegisterNumber),
	  frameBuffer(InitFrameBuffer()),
	  renderContext(renderContext)
{
}

void RenderSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	const auto currentCamera = entityManager.Get<Camera>(GameEngine::currentCameraEntity);

	RenderScene();

	// Update frame buffer
	frameBuffer.matViewProj = XMMatrixTranspose(currentCamera.matView * currentCamera.matProj);
	XMStoreFloat4(&frameBuffer.vCamera, currentCamera.position);
	renderer.UpdateFrameBuffer(frameBuffer);

	for (const auto& [entity, transform, mesh] : entityManager.View<Transform, Mesh>())
		Render(mesh, transform);

	Present();
}

void RenderSystem::Render(const Mesh& mesh, const Transform& transform)
{
	renderer.Render(mesh, renderContext->GetContext(), transform);
}

void RenderSystem::RenderScene() const
{
	ID3D11DeviceContext* context = renderContext->GetContext();
	ID3D11RenderTargetView* renderTarget = renderContext->GetRenderTargetView();
	ID3D11DepthStencilView* depthStencil = renderContext->GetDepthStencilView();

	constexpr float backgroundColor[4] = {0.0f, 0.5f, 0.0f, 1.0f};
	context->ClearRenderTargetView(renderTarget, backgroundColor);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* rtvs[] = {renderTarget};
	context->OMSetRenderTargets(1, rtvs, depthStencil);
}

// TODO: exist for testing purpose
FrameBuffer RenderSystem::InitFrameBuffer()
{
	return
	{
		.matViewProj = XMMatrixIdentity(),
		.vCamera = XMFLOAT4(0.0f, 0.0f, -5.0f, 1.0f),

		.dirLight =
		{
			.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),
			.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
			.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),

			.direction = XMFLOAT3(0.0f, -1.0f, 0.0f),
			.pad = 0.0f
		},

		.pointLights =
		{
			PointLight
			{
				.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
				.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),

				.position = XMFLOAT3(2.0f, 20.0f, -20.0f),
				.range = 50.0f,

				.attenuation = XMFLOAT3(1.0f, 0.1f, 0.01f),
				.pad = 0.0f
			}
		}
	};
}
