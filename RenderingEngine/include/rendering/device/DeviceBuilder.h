#ifndef DEVICE_BUILDER_H
#define DEVICE_BUILDER_H
#include "GraphicsDevice.h"

class DeviceBuilder
{
public:
    [[nodiscard]] static GraphicsDevice CreateDevice(HWND hwnd, GraphicsDevice::DisplayMode mode);

private:
    [[nodiscard]] static DXGI_SWAP_CHAIN_DESC CreateSwapChainDesc(HWND hwnd,
        GraphicsDevice::DisplayMode mode, UINT width, UINT height);
};

#endif