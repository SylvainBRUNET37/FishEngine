#include "pch.h"
#include "rendering/device/DepthState.h"

#include "rendering/utils/Util.h"

DepthState::DepthState(ID3D11Device* device)
{
    // Depth transparent
    D3D11_DEPTH_STENCIL_DESC desc{};
    desc.DepthEnable = TRUE;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    desc.DepthFunc = D3D11_COMPARISON_LESS;
    desc.StencilEnable = FALSE;

    DXEssayer(device->CreateDepthStencilState(&desc, &depthNoWrite));

    // Depth opaque
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DXEssayer(device->CreateDepthStencilState(&desc, &depthDefault));
}
