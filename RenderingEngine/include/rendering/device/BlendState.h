#ifndef BLENDER_STATE_H
#define BLENDER_STATE_H

#include <d3d11.h>

#include "rendering/utils/ComPtr.h"

class BlendState
{
public:
	explicit BlendState(const ComPtr<ID3D11Device>& device);

	[[nodiscard]] ID3D11BlendState* GetAlphaBlendEnabled() const { return alphaBlendEnable; }
	[[nodiscard]] ID3D11BlendState* GetAlphaBlendDisabled() const { return alphaBlendDisable; }

private:
	ComPtr<ID3D11BlendState> alphaBlendEnable;
	ComPtr<ID3D11BlendState> alphaBlendDisable;

	[[nodiscard]] static D3D11_BLEND_DESC CreateBlendDesc();
};

#endif
