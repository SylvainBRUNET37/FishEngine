#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <d3d11.h>

#include "rendering/utils/ComPtr.h"

class RenderTarget
{
public:
	RenderTarget() = default; // Use init after calling this constructor
	RenderTarget(const ComPtr<ID3D11Device>& device, const ComPtr<IDXGISwapChain>& swapChain);

	void Init(const ComPtr<ID3D11Device>& device, const ComPtr<IDXGISwapChain>& swapChain);

	[[nodiscard]] ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView; }

private:
	ComPtr<ID3D11RenderTargetView> renderTargetView;
};

#endif
