#include "pch.h"
#include "rendering/RenderingEngine.h"

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
	constexpr float aspectRatio = 1.0f; // TODO: replace with actual viewport ratio
	constexpr float nearPlane = 2.0f;
	constexpr float farPlane = 20.0f;

	matProj = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlane, farPlane);
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
		device->Present();

		AnimeScene(elapsedTime);
		RenderScene();

		previousTimeCount = currentTimeCount;
	}
}

void RenderingEngine::AnimeScene(const double elapsedTime) const
{
	for (const auto& object : scene)
		object->Anime(elapsedTime);
}

void RenderingEngine::RenderScene() const
{
	ID3D11DeviceContext* pImmediateContext = device->GetImmediateContext();
	ID3D11RenderTargetView* pRenderTargetView = device->GetRenderTargetView();

	constexpr float backgroundColor[4] = {0.0f, 0.5f, 0.0f, 1.0f}; // green
	pImmediateContext->ClearRenderTargetView(pRenderTargetView, backgroundColor);

	for (const auto& object : scene)
		object->Draw(matViewProj);
}
