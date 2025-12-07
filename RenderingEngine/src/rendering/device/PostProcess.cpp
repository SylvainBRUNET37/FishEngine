#include "pch.h"
#include "rendering/postProcessing/PostProcess.h"

#include "rendering/utils/Util.h"

PostProcess::PostProcess(ID3D11Device* device, const size_t screenWidth, const size_t screenHeight)
{
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = static_cast<UINT>(screenWidth);
    desc.Height = static_cast<UINT>(screenHeight);
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    DXEssayer(device->CreateTexture2D(&desc, nullptr, &sceneTexture));
    SetDebugName(sceneTexture, "sceneTexture-in-PostProcess");
    DXEssayer(device->CreateRenderTargetView(sceneTexture, nullptr, &sceneRenderTargetView));
    SetDebugName(sceneRenderTargetView, "sceneRenderTargetView-in-PostProcess");
    DXEssayer(device->CreateShaderResourceView(sceneTexture, nullptr, &sceneShaderResourceView));
    SetDebugName(sceneShaderResourceView, "sceneShaderResourceView-in-PostProcess");

    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    DXEssayer(device->CreateSamplerState(&sampDesc, &postProcessSampler));
}

void PostProcess::Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* backbuffer,
	ID3D11VertexShader* postProcessVertexShader, ID3D11PixelShader* postProcessPixelShader, 
    ID3D11ShaderResourceView* distortionSRV,
    ID3D11ShaderResourceView* depthSRV)
{
    // Switch render target to backbuffer
    context->OMSetRenderTargets(1, &backbuffer, nullptr);

    // Bind shaders
    context->VSSetShader(postProcessVertexShader, nullptr, 0);
    context->PSSetShader(postProcessPixelShader, nullptr, 0);

    // Bind scene texture as input
    ID3D11ShaderResourceView* srvs[3] =
    {
        sceneShaderResourceView,
        distortionSRV,
        depthSRV
    };
    context->PSSetShaderResources(0, 3, srvs);

    // Set post process sampler
    context->PSSetSamplers(0, 1, &postProcessSampler);

    // Render without vertex buffer
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(nullptr);

    // Draw
    context->Draw(3, 0);
}
