#include "pch.h"

#include "systems/RenderSystem.h"
#include "rendering/culling/FrustumCuller.h"

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
    XMStoreFloat4x4(&frameBuffer.matViewProj, XMMatrixTranspose(currentCamera.matView * currentCamera.matProj));
	XMStoreFloat4(&frameBuffer.vCamera, currentCamera.position);
	renderer.UpdateFrameBuffer(frameBuffer);

	for (const auto& [entity, transform, mesh] : entityManager.View<Transform, Mesh>())
	{
		// check if the mesh should be rendered or not
		if (FurstumCuller::IsMeshCulled(mesh, transform)) continue;
		Render(mesh, transform);
	}

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
        .matViewProj = {},
        .vCamera = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),

        .dirLight =
        {
            .ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
            .diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f),
			.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),

            .direction = XMFLOAT3(-0.5f, 1.0f, -0.5f),
            .pad = 0.0f
        },

        .pointLights =
        {
            PointLight
            {
                .ambient = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f),
                .diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
                .specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),

                .position = XMFLOAT3(2.0f, 20.0f, -20.0f),
                .range = 50.0f,

                .attenuation = XMFLOAT3(1.0f, 0.09f, 0.032f),
                .pad = 0.0f
            }
        }
    };
}

