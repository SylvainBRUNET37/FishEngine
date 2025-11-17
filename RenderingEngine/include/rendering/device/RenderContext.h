#ifndef DEVICE_H
#define DEVICE_H

#include "BlendState.h"
#include "DepthBuffer.h"
#include "PostProcess.h"
#include "Rasterizer.h"
#include "RenderTarget.h"
#include "rendering/utils/ComPtr.h"

class RenderContext
{
public:
	explicit RenderContext(const ComPtr<ID3D11Device>& device,
	                       const ComPtr<ID3D11DeviceContext>& context,
	                       const ComPtr<IDXGISwapChain>& swapChain, const WindowData& windowData);

	void Present() const;

	[[nodiscard]] ID3D11Device* GetDevice() const noexcept { return device; }
	[[nodiscard]] ID3D11DeviceContext* GetContext() const noexcept { return context; }
	[[nodiscard]] IDXGISwapChain* GetSwapChain() const noexcept { return swapChain; }

	[[nodiscard]] ID3D11RenderTargetView* GetRenderTargetView() const noexcept
	{
		return renderTarget.GetRenderTargetView();
	}

	[[nodiscard]] PostProcess& GetPostProcess() noexcept
	{
		return postProcess;
	}

	[[nodiscard]] ID3D11DepthStencilView* GetDepthStencilView() const noexcept { return depthBuffer.GetStencilView(); }

	[[nodiscard]] size_t GetScreenWidth() const noexcept { return screenWidth; }
	[[nodiscard]] size_t GetScreenHeight() const noexcept { return screenHeight; }

	void EnableAlphaBlending() const
	{
		context->OMSetBlendState(blendState.GetAlphaBlendEnabled(), nullptr, 0xffffffff);
	}

	void DisableAlphaBlending() const
	{
		context->OMSetBlendState(blendState.GetAlphaBlendDisabled(), nullptr, 0xffffffff);
	}


private:
	size_t screenWidth;
	size_t screenHeight;

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<IDXGISwapChain> swapChain;

	Rasterizer rasterizer;
	RenderTarget renderTarget;
	DepthBuffer depthBuffer;
	BlendState blendState;
	PostProcess postProcess;

	void SetRenderTarget() const;
	void SetupViewPort() const;
};

#endif
