#ifndef DISTORTION_PROCESS_H
#define DISTORTION_PROCESS_H

#include <d3d11.h>

#include "rendering/utils/ComPtr.h"

class DistortionProcess
{
public:
    DistortionProcess(ID3D11Device* device, size_t screenWidth, size_t screenHeight);

    [[nodiscard]] ID3D11Texture2D* GetTexture() const { return texture; }
    [[nodiscard]] ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView; }
    [[nodiscard]] ID3D11ShaderResourceView* GetShaderResourceView() const { return shaderResourceView; }

private:
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<ID3D11ShaderResourceView> shaderResourceView;
};

#endif
