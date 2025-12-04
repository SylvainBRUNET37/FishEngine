#include "pch.h"
#include "include/rendering/graphics/ShadowMap.h"
#include "include/rendering/utils/Util.h"

class ShadowMapTexture2DCreationError {};
class DepthStencilViewCreationError {};
class ShaderResourceViewCreationError {};

ShadowMap::ShadowMap(ID3D11Device* device, UINT width, UINT height)
	: width(width), height(height), depthMapSRV(0), depthMapDSV(0)
{
	// Viewport that matches the shadow map dimensions
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Height = static_cast<float>(width); //same parameter is used for viewport, which wants floats, and texture description, which wants UINTs
	viewport.Width = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	
	// Texture description initialization
	// Use typeless format because the Depth Stencil View is going to interpret the bits as DXGI_FORMAT_D24_UNORM_S8_UINT,
	// whereas the Shader Resource View is going to interpret the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; //This is that thing where you OR flags together, maybe?
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = 0; //Not nullpointer?
	DXEssayer(device->CreateTexture2D(&texDesc, 0, &depthMap), ShadowMapTexture2DCreationError{});

	//Depth stencil view creation
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	DXEssayer(device->CreateDepthStencilView(depthMap, &dsvDesc, &depthMapDSV), DepthStencilViewCreationError{});
	SetDebugName(depthMapDSV, "depthMapDSV-in-ShadowMap");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	DXEssayer(device->CreateShaderResourceView(depthMap, &srvDesc, &depthMapSRV), ShaderResourceViewCreationError{});
	SetDebugName(depthMapSRV, "depthMapSRV-in-ShadowMap");

	// View saves a reference to the texture so we can release our reference
	DXRelacher(depthMap);
}

// Provides access to the shader resource view to the shadow map
ID3D11ShaderResourceView* ShadowMap::DepthMapSRV()
{
	return depthMapSRV;
}

// Prepare the OM stage for rendering to the shadow map. We disable color writes for speed.
void ShadowMap::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(1, &viewport);

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	dc->OMSetRenderTargets(1, renderTargets, depthMapDSV);
	//is below equivalent?
	//dc->OMSetRenderTargets(1, 0, depthMapDSV);

	dc->ClearDepthStencilView(depthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

