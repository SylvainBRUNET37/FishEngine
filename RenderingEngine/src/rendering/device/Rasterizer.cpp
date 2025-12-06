#include "pch.h"
#include "rendering/device/Rasterizer.h"

#include "rendering/utils/Util.h"

D3D11_RASTERIZER_DESC Rasterizer::CreateRasterizeDesc()
{
	D3D11_RASTERIZER_DESC desc{};

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = TRUE;
	desc.DepthClipEnable = TRUE;
	/*desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;*/

	return desc;
}

D3D11_RASTERIZER_DESC Rasterizer::CreateNoCullDesc()
{
	D3D11_RASTERIZER_DESC desc{};

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = TRUE;
	desc.DepthClipEnable = TRUE;
	/*desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;*/

	return desc;
}

D3D11_RASTERIZER_DESC Rasterizer::CreateShadowMapRasterizeDesc()
{

	D3D11_RASTERIZER_DESC desc{};

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = TRUE;
	desc.DepthClipEnable = TRUE;
	desc.DepthBias = -10000;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 1.0f;

	return desc;
}

Rasterizer::Rasterizer(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	//const auto rasterizeDesc = CreateRasterizeDesc();
	defaultDesc = CreateRasterizeDesc();
	doubleSidedDesc = CreateNoCullDesc();
	shadowMapDesc = CreateShadowMapRasterizeDesc();

	DXEssayer(device->CreateRasterizerState(&defaultDesc, &rasterizerState), "Error while creating rasterize state");
	context->RSSetState(rasterizerState);
	SetDebugName(rasterizerState, "rasterizerState-in-Rasterizer");
}

void Rasterizer::SetCullingToNone(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) {
	/*D3D11_RASTERIZER_DESC desc;
	rasterizerState->GetDesc(&desc);
	desc.CullMode = D3D11_CULL_NONE;*/
	rasterizerState->Release();
	DXEssayer(device->CreateRasterizerState(&doubleSidedDesc, &rasterizerState), "Error while creating rasterize state");
	context->RSSetState(rasterizerState);
}

void Rasterizer::SetCullingToBack(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) {
	/*D3D11_RASTERIZER_DESC desc;
	rasterizerState->GetDesc(&desc);
	desc.CullMode = D3D11_CULL_BACK;*/
	rasterizerState->Release();
	DXEssayer(device->CreateRasterizerState(&defaultDesc, &rasterizerState), "Error while creating rasterize state");
	context->RSSetState(rasterizerState);
}

void Rasterizer::PrepareRasterizerStateForShadowMap(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) {
	//D3D11_RASTERIZER_DESC desc;
	//rasterizerState->GetDesc(&desc);
	//desc.DepthBias = 100000;
	//desc.DepthBiasClamp = 0.0f;
	//desc.SlopeScaledDepthBias = 1.0f;
	rasterizerState->Release();
	DXEssayer(device->CreateRasterizerState(&shadowMapDesc, &rasterizerState), "Error while creating rasterize state");
	context->RSSetState(rasterizerState);
}