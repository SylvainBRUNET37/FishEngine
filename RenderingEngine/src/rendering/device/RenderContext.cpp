#include "pch.h"
#include "rendering/device/RenderContext.h"

#include "rendering/graphics/camera/BaseCamera.h"
#include "rendering/utils/Util.h"

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
	  blendState(device),
	  postProcess(device, windowData.screenWidth, windowData.screenHeight),
	  distortionProcess(device, windowData.screenWidth, windowData.screenHeight),
	  depthState(device)
{
	SetRenderTarget();
	viewPort.Width = static_cast<FLOAT>(screenWidth);
	viewPort.Height = static_cast<FLOAT>(screenHeight);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	SetupViewPort();

	EnableDefaultDepth();

	//viewPort.Width = 1;
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
	//D3D11_VIEWPORT viewPort{};
	//Compiler complains of Expression Must Be a Modifiable Lvalue if viewPort is local...

	/*viewPort.Width = static_cast<FLOAT>(screenWidth);
	viewPort.Height = static_cast<FLOAT>(screenHeight);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;*/

	context->RSSetViewports(1, &viewPort);
}

void RenderContext::Resize(const UINT width, const UINT height)
{
	screenWidth = width;
	screenHeight = height;

	// Unbind previous render target and clear state
	context->OMSetRenderTargets(0, nullptr, nullptr);
	context->ClearState();
	context->Flush();

	renderTarget = RenderTarget{};

	// Resize swap chain buffers
	DXEssayer(swapChain->ResizeBuffers
		(
			0,
			width,
			height,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		));

	// Recreate member objets to take the new size in count
	renderTarget = RenderTarget{device, swapChain};
	postProcess = PostProcess{device, width, height};
	distortionProcess = DistortionProcess{device, width, height};

	// Recreate the depth buffer
	WindowData windowData{};
	windowData.screenWidth = width;
	windowData.screenHeight = height;
	depthBuffer = DepthBuffer{device, windowData};

	BaseCameraData::aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	viewPort.Width = static_cast<FLOAT>(width);
	viewPort.Height = static_cast<FLOAT>(height);
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	// Update viewport and set new render target
	SetRenderTarget();
	SetupViewPort();

	mustUpdateUI = true;
}

void RenderContext::Move(UINT x, UINT y)
{
	screenStartX = x;
	screenStartY = y;
	mustUpdateUI = true;
}
