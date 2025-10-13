#include "pch.h"
#include "rendering/RenderingEngine.h"

#include <filesystem>

#include "rendering/ModelLoader.h"
#include "rendering/shapes/Model.h"
#include "rendering/utils/Clock.h"

using namespace std;
using namespace DirectX;

void RenderingEngine::InitAnimation()
{
	previousTimeCount = Clock::GetTimeCount();
	RenderScene();
}

void RenderingEngine::InitScene()
{
	const XMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
	const XMVECTOR focusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	const XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	matView = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

	constexpr float fieldOfView = XM_PI / 4.0f; // 45 degrees
	const float aspectRatio = static_cast<float>(device->GetLargeur()) / static_cast<float>(device->GetHauteur());
	constexpr float nearPlane = 2.0f;
	constexpr float farPlane = 20.0f;

	matProj = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlane, farPlane);
	matViewProj = matView * matProj;

	InitObjects();
}

void RenderingEngine::InitObjects()
{
	ShaderProgram shaderProgram
	{
		device->GetD3DDevice(),
		shaderBank.Get<VertexShader>("shaders/MiniPhongVS.hlsl"),
		shaderBank.Get<PixelShader>("shaders/MiniPhongPS.hlsl"),
		shaderBank.Get<ShaderBank::Layout>("MiniPhong")
	};

	const filesystem::path filePath = "assets\\Jin\\jin.obj";

	ModelLoader modelLoader;
	auto model = modelLoader.LoadModel(filePath, device, std::move(shaderProgram));

	scene.emplace_back(std::move(model));
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

		device->Present();

		previousTimeCount = currentTimeCount;
	}
}

void RenderingEngine::AnimeScene(const double elapsedTime) const
{
	for (const auto& object : scene)
		object.Anime(elapsedTime);
}

void RenderingEngine::RenderScene() // TODO: refactor
{
	ID3D11DeviceContext* pImmediateContext = device->GetImmediateContext();
	ID3D11RenderTargetView* pRenderTargetView = device->GetRenderTargetView();
	ID3D11DepthStencilView* pDepthStencilView = device->GetDepthStencilView();

	constexpr float backgroundColor[4] = {0.0f, 0.5f, 0.0f, 1.0f}; // green
	pImmediateContext->ClearRenderTargetView(pRenderTargetView, backgroundColor);

	// Clear both depth and stencil
	pImmediateContext->ClearDepthStencilView(pDepthStencilView,
	                                         D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Bind render target AND depth stencil view (was passing nullptr)
	ID3D11RenderTargetView* rtvs[] = {pRenderTargetView};
	pImmediateContext->OMSetRenderTargets(1, rtvs, pDepthStencilView);

	// Prepare matrices (use the matrices computed in InitScene instead of hardcoded values)
	const XMMATRIX world = XMMatrixRotationY(static_cast<float>(GetTickCount64()) / 2000.0f);
	const XMMATRIX view = XMMatrixLookAtLH(
		XMVectorSet(0, 2.0f, -4, 0),
		XMVectorSet(0, 1.0f, 0, 0),
		XMVectorSet(0, 1, 0, 0));
	const XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 800.0f / 600.0f, 0.1f, 100.0f);

	// Light & camera
	constexpr auto lightPos = XMFLOAT4(2, 2, -2, 1);
	constexpr auto cameraPos = XMFLOAT4(0, 1.5f, -4, 1);
	constexpr auto vAEcl = XMFLOAT4(0.2f, 0.2f, 0.2f, 1);
	constexpr auto vDEcl = XMFLOAT4(1, 1, 1, 1);
	constexpr auto vSEcl = XMFLOAT4(1, 1, 1, 1);

	const Transform transform{.world = world, .view = view, .proj = proj};
	constexpr SceneData sceneData
	{
		.lightPosition = lightPos, .cameraPosition = cameraPos, .vAEcl = vAEcl, .vDEcl = vDEcl,
		.vSEcl = vSEcl
	};

	for (auto& object : scene)
		object.Draw(pImmediateContext, transform, sceneData);
}
