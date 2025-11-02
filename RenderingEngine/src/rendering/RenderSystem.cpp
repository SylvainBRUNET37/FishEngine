#include "pch.h"
#include "rendering/RenderSystem.h"

RenderSystem::RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials)
	: renderer(renderContext->GetDevice(), std::move(materials), frameCbRegisterNumber, objectCbRegisterNumber),
	sceneData(CreateSceneData()),
	renderContext(renderContext),
	cursorCoordinates{}
{
	InitializeCamera();
	sceneData.matViewProj = camera->getMatView() * camera->getMatProj();
}

void RenderSystem::InitializeCamera()
{
	camera = std::make_unique<ThirdPersonCamera>(
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		100.0f, //Distance
		30.0f, //Hauteur
		static_cast<float>(renderContext->GetScreenWidth()),
		static_cast<float>(renderContext->GetScreenHeight())
	);
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

void RenderSystem::UpdateCamera(const Transform& cubeTransform)
{
	camera->UpdateFromCube(&cubeTransform);
}

void RenderSystem::UpdateScene(double elapsedTime, const Transform& cubeTransform)
{
	HandleRotation();
	UpdateCamera(cubeTransform);
	sceneData.matViewProj = camera->getMatView() * camera->getMatProj();
	RenderScene();
}

void RenderSystem::Render(const Mesh& mesh, const Transform& transform)
{
	renderer.Render(mesh, renderContext->GetContext(), transform, sceneData);
}

void RenderSystem::RenderScene()
{
	ID3D11DeviceContext* context = renderContext->GetContext();
	ID3D11RenderTargetView* renderTarget = renderContext->GetRenderTargetView();
	ID3D11DepthStencilView* depthStencil = renderContext->GetDepthStencilView();

	constexpr float backgroundColor[4] = { 0.0f, 0.5f, 0.0f, 1.0f };
	context->ClearRenderTargetView(renderTarget, backgroundColor);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* rtvs[] = { renderTarget };
	context->OMSetRenderTargets(1, rtvs, depthStencil);

	XMStoreFloat4(&sceneData.cameraPosition, camera->GetPosition());
}

SceneData RenderSystem::CreateSceneData()
{
	return {
		.matViewProj = {},
		.lightPosition = XMFLOAT4(2, 2, -20, 1),
		.cameraPosition = {},
		.vAEcl = XMFLOAT4(0.2f, 0.2f, 0.2f, 1),
		.vDEcl = XMFLOAT4(1, 1, 1, 1),
		.vSEcl = XMFLOAT4(1, 1, 1, 1)
	};
}