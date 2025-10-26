#include "pch.h"
#include "rendering/RenderSystem.h"

RenderSystem::RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials) : renderer{
	                                                           renderContext->GetDevice(), std::move(materials), frameCbRegisterNumber,
	                                                           objectCbRegisterNumber
                                                           },
                                                           sceneData{CreateSceneData()},
                                                           renderContext{renderContext}, cursorCoordinates{}

{
	const XMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f, -4.0f, 1.0f);
	const XMVECTOR focusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	firstPersonCamera = std::make_unique<Camera>(
		eyePosition,
		focusPoint,
		upDirection,
		static_cast<float>(renderContext->GetScreenWidth()),
		static_cast<float>(renderContext->GetScreenHeight())
	);

	sceneData.matViewProj = firstPersonCamera->getMatView() * firstPersonCamera->getMatProj();

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

void RenderSystem::AnimeScene(const double elapsedTime)
{
	const float deplacement = cameraSpeed * static_cast<float>(elapsedTime);
	bool cameraUpdated = false;
	float deltaX = 0.0f;
	float deltaZ = 0.0f;

	if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) // Flèche droite
	{
		deltaX += deplacement;
		cameraUpdated = true;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) // Flèche gauche
	{
		deltaX -= deplacement;
		cameraUpdated = true;
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) // Flèche haut
	{
		deltaZ += deplacement;
		cameraUpdated = true;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) // Flèche bas
	{
		deltaZ -= deplacement;
		cameraUpdated = true;
	}

	POINT currentCursorCoordinates; // Structure to store the cursor's coordinates

	// Call GetCursorPos to get the current cursor position
	const bool gotPos = GetCursorPos(&currentCursorCoordinates);
	if (gotPos and GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		// Get difference
		const auto deltaX1 = currentCursorCoordinates.x - cursorCoordinates.x;
		const auto deltaY1 = currentCursorCoordinates.y - cursorCoordinates.y;

		// Rotate camera
		constexpr float mouseSensitivity = 0.002f; // radians per pixel, tweak as needed
		const float yaw = deltaX1 * mouseSensitivity; // horizontal rotation
		const float pitch = -deltaY1 * mouseSensitivity; // vertical rotation (negative = FPS style)
		firstPersonCamera->RotateFirstPerson(yaw, pitch);
		cameraUpdated = true;
	}
	// Update coords
	cursorCoordinates = currentCursorCoordinates;

	// Mettre à jour la caméra active si déplacement
	if (cameraUpdated)
	{
		firstPersonCamera->MoveFirstPerson(deltaZ, deltaX);
		sceneData.matViewProj = firstPersonCamera->getMatView() * firstPersonCamera->getMatProj();
	}
}

void RenderSystem::RenderScene()
{
	ID3D11DeviceContext* pImmediateContext = renderContext->GetContext();
	ID3D11RenderTargetView* pRenderTargetView = renderContext->GetRenderTargetView();
	ID3D11DepthStencilView* pDepthStencilView = renderContext->GetDepthStencilView();

	// Add green background color
	constexpr float backgroundColor[4] = {0.0f, 0.5f, 0.0f, 1.0f};
	pImmediateContext->ClearRenderTargetView(pRenderTargetView, backgroundColor);

	// Clear both depth and stencil
	pImmediateContext->ClearDepthStencilView(pDepthStencilView,
	                                         D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Bind render target AND depth stencil view
	ID3D11RenderTargetView* rtvs[] = {pRenderTargetView};
	pImmediateContext->OMSetRenderTargets(1, rtvs, pDepthStencilView);

	// Update camera position in scene data
	XMStoreFloat4(&sceneData.cameraPosition, firstPersonCamera->GetPosition());
}

SceneData RenderSystem::CreateSceneData()
{
	return
	{
		.matViewProj = {},
		.lightPosition = XMFLOAT4(2, 2, -20, 1),
		.cameraPosition = {},
		.vAEcl = XMFLOAT4(0.2f, 0.2f, 0.2f, 1),
		.vDEcl = XMFLOAT4(1, 1, 1, 1),
		.vSEcl = XMFLOAT4(1, 1, 1, 1)
	};
}
