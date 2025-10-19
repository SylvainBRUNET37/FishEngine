#include "pch.h"
#include "rendering/device/RenderContext.h"

RenderContext::RenderContext(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context,
                             const ComPtr<IDXGISwapChain>& swapChain, const WindowData& windowData)
	: screenWidth(windowData.screenWidth),
	  screenHeight(windowData.screenHeight),
	  device(device),
	  context(context),
	  swapChain(swapChain),
	  rasterizer(device, context),
	  renderTarget(device, swapChain),
	  depthBuffer(device, windowData),
	  blendState(device)
{
	SetRenderTarget();
	SetupViewPort();
}

void RenderContext::Present() const
{
	// The result is an error if the user leave the window
	[[maybe_unused]]
	const auto result = swapChain->Present(0, 0);
}

void RenderContext::SetRenderTarget() const
{
	const auto renderTargetView = renderTarget.GetRenderTargetView();
	const auto depthStencilView = depthBuffer.GetStencilView();

	context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void RenderContext::SetupViewPort() const
{
	D3D11_VIEWPORT viewPort{};

	viewPort.Width = static_cast<FLOAT>(screenWidth);
	viewPort.Height = static_cast<FLOAT>(screenHeight);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	context->RSSetViewports(1, &viewPort);
}
