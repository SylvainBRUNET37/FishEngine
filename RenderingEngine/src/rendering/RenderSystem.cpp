#include "pch.h"
#include "rendering/RenderSystem.h"

RenderSystem::RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials)
	: renderer(renderContext->GetDevice(), std::move(materials), frameCbRegisterNumber, objectCbRegisterNumber),
	sceneData(CreateSceneData()),
	renderContext(renderContext),
	cursorCoordinates{}
{
	InitializeCamera();
	sceneData.matViewProj = displayedCamera->getMatView() * displayedCamera->getMatProj();
	RenderScene();
}

void RenderSystem::UpdateScene(const double elapsedTime)
{
	AnimeScene(elapsedTime);
	RenderScene();
}

void RenderSystem::Render(const Mesh& mesh, const Transform& transform)
{
	renderer.Render(mesh, renderContext->GetContext(), transform, sceneData);
}

void RenderSystem::InitializeCamera()
{
	const XMVECTOR eyePosition = XMVectorSet(0.0f, 5.0f, -4.0f, 1.0f);
	const XMVECTOR focusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	displayedCamera = std::make_unique<FirstPersonCamera>(
		eyePosition, focusPoint, upDirection,
		static_cast<float>(renderContext->GetScreenWidth()),
		static_cast<float>(renderContext->GetScreenHeight())
	);
}

void RenderSystem::SwitchToThirdPerson()
{
	auto* fpCam = dynamic_cast<FirstPersonCamera*>(displayedCamera.get());
	float currentYaw = fpCam ? fpCam->GetYaw() : 0.0f;
	XMVECTOR targetPos = displayedCamera->GetFocus();

	displayedCamera = std::make_unique<ThirdPersonCamera>(
		targetPos, 10.0f, 2.0f,
		static_cast<float>(renderContext->GetScreenWidth()),
		static_cast<float>(renderContext->GetScreenHeight()),
		currentYaw, 0.0f
	);
}

void RenderSystem::SwitchToFirstPerson()
{
	XMVECTOR newPosition = displayedCamera->GetPosition();
	XMVECTOR newFocus = displayedCamera->GetFocus();

	displayedCamera = std::make_unique<FirstPersonCamera>(
		newPosition, newFocus, XMVectorSet(0, 1, 0, 0),
		static_cast<float>(renderContext->GetScreenWidth()),
		static_cast<float>(renderContext->GetScreenHeight())
	);
}

bool RenderSystem::HandleCameraSwitch()
{
	static bool cKeyWasPressed = false;
	const bool cKeyIsPressed = (GetAsyncKeyState('C') & 0x8000) != 0;

	if (cKeyIsPressed && !cKeyWasPressed) {
		if (isFirstPerson) {
			SwitchToThirdPerson();
		}
		else {
			SwitchToFirstPerson();
		}
		isFirstPerson = !isFirstPerson;
		cKeyWasPressed = cKeyIsPressed;
		return true;
	}

	cKeyWasPressed = cKeyIsPressed;
	return false;
}

bool RenderSystem::HandleMovement(float deplacement, float& deltaX, float& deltaZ)
{
	bool moved = false;

	if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) {
		deltaX += deplacement;
		moved = true;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) {
		deltaX -= deplacement;
		moved = true;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) {
		deltaZ += deplacement;
		moved = true;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
		deltaZ -= deplacement;
		moved = true;
	}

	return moved;
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
		const float yaw = deltaX * mouseSensitivity;
		const float pitch = -deltaY * mouseSensitivity;

		displayedCamera->Rotate(yaw, pitch);
		rotated = true;
	}

	cursorCoordinates = currentCursorCoordinates;
	return rotated;
}

void RenderSystem::AnimeScene(const double elapsedTime)
{
	const float deplacement = cameraSpeed * static_cast<float>(elapsedTime);
	float deltaX = 0.0f;
	float deltaZ = 0.0f;

	bool cameraUpdated = HandleCameraSwitch(); //Contrôles changement caméra
	cameraUpdated |= HandleMovement(deplacement, deltaX, deltaZ); //Contrôles move caméra
	cameraUpdated |= HandleRotation(); //Contrôles rotation caméra

	if (cameraUpdated) {
		displayedCamera->Move(deltaZ, deltaX, 0.0f);
		sceneData.matViewProj = displayedCamera->getMatView() * displayedCamera->getMatProj();
	}
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

	XMStoreFloat4(&sceneData.cameraPosition, displayedCamera->GetPosition());
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