#include "pch.h"
#include "rendering/device/PostProcess.h"

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
    DXEssayer(device->CreateRenderTargetView(sceneTexture, nullptr, &sceneRenderTargetView));
    DXEssayer(device->CreateShaderResourceView(sceneTexture, nullptr, &sceneShaderResourceView));
}

void PostProcess::Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* backbuffer,
	ID3D11VertexShader* postProcessVertexShader, ID3D11PixelShader* postProcessPixelShader)
{
    // Switch render target to backbuffer
    context->OMSetRenderTargets(1, &backbuffer, nullptr);

    // Bind shaders
    context->VSSetShader(postProcessVertexShader, nullptr, 0);
    context->PSSetShader(postProcessPixelShader, nullptr, 0);

    // Draw
    context->PSSetShaderResources(0, 1, &sceneShaderResourceView);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //context->Draw(); TODO
}
