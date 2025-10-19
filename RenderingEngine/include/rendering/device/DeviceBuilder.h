#ifndef DEVICE_BUILDER_H
#define DEVICE_BUILDER_H

#include "RenderContext.h"

class DeviceBuilder
{
public:
	[[nodiscard]] static RenderContext CreateRenderContext(HWND hwnd, RenderContext::DisplayMode mode);

private:
	[[nodiscard]] static DXGI_SWAP_CHAIN_DESC CreateSwapChainDesc(HWND hwnd,
	                                                              RenderContext::DisplayMode mode, UINT width,
	                                                              UINT height);
};

#endif
