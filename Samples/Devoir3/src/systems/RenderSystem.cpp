#include "pch.h"

#include "systems/RenderSystem.h"

#include "GameEngine.h"

RenderSystem::RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials)
	: renderer(renderContext->GetDevice(), std::move(materials), frameCbRegisterNumber, objectCbRegisterNumber),
	sceneData(InitSceneData()),
	renderContext(renderContext),
	cursorCoordinates{}
{

}

bool RenderSystem::HandleRotation()
{
	POINT currentCursorCoordinates;
	if (!GetCursorPos(&currentCursorCoordinates)) {
		return false;
	}

	bool rotated = false;
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		const auto deltaX = currentCursorCoordinates.x - cursorCoordinates.x;
		const auto deltaY = currentCursorCoordinates.y - cursorCoordinates.y;

		constexpr float mouseSensitivity = 0.002f;
		camera->Rotate(deltaX * mouseSensitivity, -deltaY * mouseSensitivity);
		rotated = true;
	}

	cursorCoordinates = currentCursorCoordinates;
	return rotated;
}

void RenderSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	const auto currentCamera = entityManager.Get<CameraData>(GameEngine::currentCameraEntity);
	sceneData.matViewProj = currentCamera.matView * currentCamera.matProj;
	XMStoreFloat4(&sceneData.cameraPosition, currentCamera.position);

	RenderScene();

	for (const auto& [entity, transform, mesh] : entityManager.View<Transform, Mesh>())
		Render(mesh, transform);

	Present();
}

void RenderSystem::Render(const Mesh& mesh, const Transform& transform)
{
	renderer.Render(mesh, renderContext->GetContext(), transform, sceneData);
}

void RenderSystem::RenderScene() const
{
	ID3D11DeviceContext* context = renderContext->GetContext();
	ID3D11RenderTargetView* renderTarget = renderContext->GetRenderTargetView();
	ID3D11DepthStencilView* depthStencil = renderContext->GetDepthStencilView();

	constexpr float backgroundColor[4] = { 0.0f, 0.5f, 0.0f, 1.0f };
	context->ClearRenderTargetView(renderTarget, backgroundColor);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* rtvs[] = { renderTarget };
	context->OMSetRenderTargets(1, rtvs, depthStencil);
}

SceneData RenderSystem::InitSceneData()
{
	return
	{
		.matViewProj = {},
		.lightPosition = XMFLOAT4(2, 20, -20, 1),
		.cameraPosition = {},
		.vAEcl = XMFLOAT4(0.2f, 0.2f, 0.2f, 1),
		.vDEcl = XMFLOAT4(1, 1, 1, 1),
		.vSEcl = XMFLOAT4(1, 1, 1, 1)
	};
}