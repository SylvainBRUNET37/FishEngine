#include "pch.h"
#include "rendering/device/BlendState.h"

#include "rendering/utils/Util.h"
#include "resources/resource.h"

D3D11_BLEND_DESC BlendState::CreateBlendDesc()
{
	D3D11_BLEND_DESC desc{};

	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return desc;
}

BlendState::BlendState(const ComPtr<ID3D11Device>& device)
{
	auto blendDesc = CreateBlendDesc();

	DXEssayer(device->CreateBlendState(&blendDesc, &alphaBlendEnable), DXE_ERREURCREATION_BLENDSTATE);

	blendDesc.RenderTarget[0].BlendEnable = FALSE;

	DXEssayer(device->CreateBlendState(&blendDesc, &alphaBlendDisable), DXE_ERREURCREATION_BLENDSTATE);
}
