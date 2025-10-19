#ifndef DEPTH_BUFFER_H
#define DEPTH_BUFFER_H
#include "rendering/utils/ComPtr.h"

class DepthBuffer
{
public:
    DepthBuffer(const ComPtr<ID3D11Device>& device, UINT width, UINT height);
    [[nodiscard]] ID3D11DepthStencilView* GetStencilView() const { return depthStencilView; }

private:
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11DepthStencilView> depthStencilView;

    [[nodiscard]] static D3D11_TEXTURE2D_DESC CreateDepthTextureDesc(UINT width, UINT height);
    [[nodiscard]] static D3D11_DEPTH_STENCIL_VIEW_DESC CreateStencilViewDesc(const D3D11_TEXTURE2D_DESC& depthTextureDesc);
};

#endif
