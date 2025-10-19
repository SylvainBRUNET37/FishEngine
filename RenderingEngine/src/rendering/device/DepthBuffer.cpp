#include "pch.h"
#include "rendering/device/DepthBuffer.h"

#include "rendering/utils/Util.h"
#include "resources/resource.h"

DepthBuffer::DepthBuffer(const ComPtr<ID3D11Device>& device, const WindowData& windowData)
{
	const auto depthTextureDesc = CreateDepthTextureDesc(windowData);

	DXEssayer(device->CreateTexture2D(&depthTextureDesc, nullptr, &texture), DXE_ERREURCREATIONTEXTURE);

	const auto stencilViewDesc = CreateStencilViewDesc(depthTextureDesc);

	DXEssayer(device->CreateDepthStencilView(texture, &stencilViewDesc, &depthStencilView),
	          DXE_ERREURCREATIONDEPTHSTENCILTARGET);
}

D3D11_TEXTURE2D_DESC DepthBuffer::CreateDepthTextureDesc(const WindowData& windowData)
{
	D3D11_TEXTURE2D_DESC desc{};

	desc.Width = windowData.screenWidth;
	desc.Height = windowData.screenHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	return desc;
}

D3D11_DEPTH_STENCIL_VIEW_DESC DepthBuffer::CreateStencilViewDesc(const D3D11_TEXTURE2D_DESC& depthTextureDesc)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc{};

	desc.Format = depthTextureDesc.Format;
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	return desc;
}
