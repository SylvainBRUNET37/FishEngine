#include "pch.h"
#include "rendering/device/DistortionProcess.h"

#include "rendering/utils/Util.h"

DistortionProcess::DistortionProcess(ID3D11Device* device, const size_t screenWidth, const size_t screenHeight)
{
	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Width = static_cast<UINT>(screenWidth);
	texDesc.Height = static_cast<UINT>(screenHeight);
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	DXEssayer(device->CreateTexture2D(&texDesc, nullptr, &texture));
	SetDebugName(texture, "texture-in-DistortionProcess");
	DXEssayer(device->CreateRenderTargetView(texture, nullptr, &renderTargetView));
	DXEssayer(device->CreateShaderResourceView(texture, nullptr, &shaderResourceView));
	SetDebugName(renderTargetView, "renderTargetView-in-DistortionProcess");
	SetDebugName(shaderResourceView, "shaderResourceView-in-DistortionProcess");
}
