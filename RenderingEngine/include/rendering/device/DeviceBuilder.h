#ifndef DEVICE_BUILDER_H
#define DEVICE_BUILDER_H

#include "RenderContext.h"
#include "rendering/core/WindowData.h"

class DeviceBuilder
{
public:
	[[nodiscard]] static RenderContext CreateRenderContext(HWND hwnd, const WindowData& windowData);

private:
	[[nodiscard]] static DXGI_SWAP_CHAIN_DESC CreateSwapChainDesc(HWND hwnd, const WindowData& windowData);
};

#endif
