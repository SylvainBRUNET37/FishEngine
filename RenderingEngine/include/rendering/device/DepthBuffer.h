#ifndef DEPTH_BUFFER_H
#define DEPTH_BUFFER_H

#include <d3d11.h>

#include "rendering/core/WindowData.h"
#include "rendering/utils/ComPtr.h"

class DepthBuffer
{
public:
	DepthBuffer(const ComPtr<ID3D11Device>& device, const WindowData& windowData);
	[[nodiscard]] ID3D11DepthStencilView* GetStencilView() const { return depthStencilView; }
	[[nodiscard]] ID3D11ShaderResourceView* GetDepthSRV() const { return depthSRV; }

private:
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11DepthStencilView> depthStencilView;
	ComPtr<ID3D11ShaderResourceView> depthSRV;

	[[nodiscard]] static D3D11_TEXTURE2D_DESC CreateDepthTextureDesc(const WindowData& windowData);
	[[nodiscard]] static D3D11_DEPTH_STENCIL_VIEW_DESC CreateStencilViewDesc(
		const D3D11_TEXTURE2D_DESC& depthTextureDesc);
	[[nodiscard]] static D3D11_SHADER_RESOURCE_VIEW_DESC CreateSRVDesc();
};

#endif
