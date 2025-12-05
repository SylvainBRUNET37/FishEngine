#ifndef RESTERIZER_H
#define RESTERIZER_H

#include <d3d11.h>

#include "rendering/utils/ComPtr.h"

#include <memory>

class Rasterizer
{
public:
	Rasterizer(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	void SetCullingToNone(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	void SetCullingToBack(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	void PrepareRasterizerStateForShadowMap(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);

private:
	ComPtr<ID3D11RasterizerState> rasterizerState;
	D3D11_RASTERIZER_DESC defaultDesc;
	D3D11_RASTERIZER_DESC doubleSidedDesc;
	D3D11_RASTERIZER_DESC shadowMapDesc;

	static D3D11_RASTERIZER_DESC CreateRasterizeDesc();
	static D3D11_RASTERIZER_DESC CreateNoCullDesc();

	static D3D11_RASTERIZER_DESC CreateShadowMapRasterizeDesc();
};

#endif
