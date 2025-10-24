#include "pch.h"
#include "rendering/device/Rasterizer.h"

#include "rendering/utils/Util.h"

D3D11_RASTERIZER_DESC Rasterizer::CreateRasterizeDesc()
{
	D3D11_RASTERIZER_DESC desc{};

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_FRONT;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthClipEnable = TRUE;

	return desc;
}

Rasterizer::Rasterizer(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	const auto resterizeDesc = CreateRasterizeDesc();

	DXEssayer(device->CreateRasterizerState(&resterizeDesc, &rasterizerState), "Error while creating resterize state");
	context->RSSetState(rasterizerState);
}
