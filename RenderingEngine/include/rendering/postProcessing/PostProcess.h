#ifndef POST_PROCESS_H
#define POST_PROCESS_H

#include <d3d11.h>

#include "rendering/utils/ComPtr.h"

class PostProcess
{
public:
	PostProcess(ID3D11Device* device, size_t screenWidth, size_t screenHeight);

    void Draw(ID3D11DeviceContext* context,
        ID3D11RenderTargetView* backbuffer,
        ID3D11VertexShader* postProcessVertexShader,
        ID3D11PixelShader* postProcessPixelShader,
        ID3D11ShaderResourceView* distortionSRV, 
        ID3D11ShaderResourceView* depthSRV);

    [[nodiscard]] ID3D11RenderTargetView* GetRenderTargetView() const { return sceneRenderTargetView; }
    [[nodiscard]] ID3D11ShaderResourceView* GetShaderResourceView() const { return sceneShaderResourceView; }

private:
    ComPtr<ID3D11Texture2D> sceneTexture;
    ComPtr<ID3D11RenderTargetView> sceneRenderTargetView;
    ComPtr<ID3D11ShaderResourceView> sceneShaderResourceView;
    ComPtr<ID3D11SamplerState> postProcessSampler;
};

#endif
