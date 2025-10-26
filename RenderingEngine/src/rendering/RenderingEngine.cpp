#include "pch.h"
#include "rendering/RenderingEngine.h"

#include <filesystem>

#include "rendering/Renderer.h"
#include "rendering/SceneLoader.h"
#include "rendering/core/SceneData.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Model.h"
#include "rendering/utils/Clock.h"
#include "Windows.h"

using namespace std;
using namespace DirectX;

void RenderingEngine::InitAnimation()
{
	previousTimeCount = Clock::GetTimeCount();
	RenderScene();
}

void RenderingEngine::InitScene()
{
	const XMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f,-4.0f, 1.0f);
	const XMVECTOR focusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	firstPersonCamera = std::make_unique<Camera>(
		eyePosition,
		focusPoint,
		upDirection,
		static_cast<float>(renderContext->GetScreenWidth()),
		static_cast<float>(renderContext->GetScreenHeight())
	);

	matView = firstPersonCamera->getMatView();
	matProj = firstPersonCamera->getMatProj();
	matViewProj = matView * matProj;
}

void RenderingEngine::Run()
{
	auto allCallbacksSucceeded = [&]
	{
		return ranges::all_of(mainLoopCallbacks, [](auto& callback)
		{
			return callback();
		});
	};

	while (allCallbacksSucceeded())
	{
		UpdateScene();
	}
}

void RenderingEngine::UpdateScene()
{
	constexpr int FRAME_RATE = 60;
	constexpr double DELTA_TIME = 1.0 / static_cast<double>(FRAME_RATE);

	const int64_t currentTimeCount = Clock::GetTimeCount();
	const double elapsedTime = Clock::GetTimeBetweenCounts(previousTimeCount, currentTimeCount);

	if (elapsedTime > DELTA_TIME)
	{
		AnimeScene(elapsedTime);
		RenderScene();

		renderContext->Present();

		previousTimeCount = currentTimeCount;
	}
}

void RenderingEngine::AnimeScene(const double elapsedTime)
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
	bool gotPos = GetCursorPos(&currentCursorCoordinates);
	if (gotPos and GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		// Get difference
		auto deltaX1 = currentCursorCoordinates.x - cursorCoordinates.x;
		auto deltaY1 = currentCursorCoordinates.y - cursorCoordinates.y;

		// Rotate camera
		constexpr float mouseSensitivity = 0.002f; // radians per pixel, tweak as needed
		float yaw = deltaX1 * mouseSensitivity;   // horizontal rotation
		float pitch = -deltaY1 * mouseSensitivity; // vertical rotation (negative = FPS style)
		firstPersonCamera->RotateFirstPerson(yaw, pitch);
		cameraUpdated = true;
	}
	// Update coords
	cursorCoordinates = currentCursorCoordinates;

	// Mettre à jour la caméra active si déplacement
	if (cameraUpdated)
	{
		firstPersonCamera->MoveFirstPerson(deltaZ, deltaX);
		matView = firstPersonCamera->getMatView();
		matViewProj = matView * matProj;
	}
}

void RenderingEngine::RenderScene() // TODO: refactor
{
	ID3D11DeviceContext* pImmediateContext = renderContext->GetContext();
	ID3D11RenderTargetView* pRenderTargetView = renderContext->GetRenderTargetView();
	ID3D11DepthStencilView* pDepthStencilView = renderContext->GetDepthStencilView();

	constexpr float backgroundColor[4] = {0.0f, 0.5f, 0.0f, 1.0f}; // green
	pImmediateContext->ClearRenderTargetView(pRenderTargetView, backgroundColor);

	// Clear both depth and stencil
	pImmediateContext->ClearDepthStencilView(pDepthStencilView,
	                                         D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Bind render target AND depth stencil view
	ID3D11RenderTargetView* rtvs[] = {pRenderTargetView};
	pImmediateContext->OMSetRenderTargets(1, rtvs, pDepthStencilView);

	// Prepare matrices (use the matrices computed in InitScene instead of hardcoded values)
	const XMMATRIX world = XMMatrixIdentity();
	const XMMATRIX view = firstPersonCamera->getMatView();
	const XMMATRIX proj = firstPersonCamera->getMatProj();

	// Light & camera
	constexpr auto lightPos = XMFLOAT4(2, 2, -20, 1);
	XMFLOAT4 cameraPos;
	XMStoreFloat4(&cameraPos, this->firstPersonCamera->GetPosition());
	//constexpr auto cameraPos = XMFLOAT4(0, 2.0f, -4, 0);
	constexpr auto vAEcl = XMFLOAT4(0.2f, 0.2f, 0.2f, 1);
	constexpr auto vDEcl = XMFLOAT4(1, 1, 1, 1);
	constexpr auto vSEcl = XMFLOAT4(1, 1, 1, 1);

	const Transform transform{.world = world};
	const SceneData sceneData
	{
		.matViewProj = matViewProj, .lightPosition = lightPos, .cameraPosition = cameraPos, .vAEcl = vAEcl, .vDEcl = vDEcl,
		.vSEcl = vSEcl
	};

	for (auto& object : scene)
		renderer.Draw(object, pImmediateContext, transform, sceneData);
}
