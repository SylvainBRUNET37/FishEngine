#include "pch.h"
#include "rendering/device/RenderTarget.h"

#include "rendering/utils/Util.h"
#include "resources/resource.h"

RenderTarget::RenderTarget(const ComPtr<ID3D11Device>& device, const ComPtr<IDXGISwapChain>& swapChain)
{
    ComPtr<ID3D11Texture2D> backBuffer;

    DXEssayer(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)), DXE_ERREUROBTENTIONBUFFER);
    DXEssayer(device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView), DXE_ERREURCREATIONRENDERTARGET);
}
