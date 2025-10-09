#ifndef DEVICE_H
#define DEVICE_H

#include "rendering/utils/ComPtr.h"

class Device
{
public:
	enum CDS_MODE : uint8_t
	{
		CDS_FENETRE,
		CDS_PLEIN_ECRAN
	};

	explicit Device(CDS_MODE cdsMode, HWND hWnd);

	void Present() const;

	[[nodiscard]] ID3D11Device* GetD3DDevice() const noexcept { return pD3DDevice; }
	[[nodiscard]] ID3D11DeviceContext* GetImmediateContext() const noexcept { return pImmediateContext; }
	[[nodiscard]] IDXGISwapChain* GetSwapChain() const noexcept { return pSwapChain; }
	[[nodiscard]] ID3D11RenderTargetView* GetRenderTargetView() const noexcept { return pRenderTargetView; }

private:
	ComPtr<ID3D11Device> pD3DDevice{};
	ComPtr<ID3D11DeviceContext> pImmediateContext{};
	ComPtr<IDXGISwapChain> pSwapChain{};
	ComPtr<ID3D11RenderTargetView> pRenderTargetView{};
};

#endif
