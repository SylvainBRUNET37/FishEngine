#ifndef RESTERIZER_H
#define RESTERIZER_H

#include <d3d11.h>

#include "rendering/utils/ComPtr.h"

class Rasterizer
{
public:
	Rasterizer(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);

private:
	ComPtr<ID3D11RasterizerState> rasterizerState;

	static D3D11_RASTERIZER_DESC CreateRasterizeDesc();
};

#endif
