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
	[[nodiscard]] ID3D11DepthStencilView* GetDepthStencilView() const { return pDepthStencilView; }

	uint32_t GetLargeur() const { return largeurEcran; }
	uint32_t GetHauteur() const { return hauteurEcran; }

	void ActiverMelangeAlpha() const
	{
		pImmediateContext->OMSetBlendState(alphaBlendEnable, nullptr, 0xffffffff);
	}

	void DesactiverMelangeAlpha() const
	{
		pImmediateContext->OMSetBlendState(alphaBlendDisable, nullptr, 0xffffffff);
	}

private:
	uint32_t largeurEcran;
	uint32_t hauteurEcran;

	ComPtr<ID3D11Device> pD3DDevice{};
	ComPtr<ID3D11DeviceContext> pImmediateContext{};
	ComPtr<IDXGISwapChain> pSwapChain{};
	ComPtr<ID3D11RenderTargetView> pRenderTargetView{};

	ID3D11Texture2D* pDepthTexture;
	ID3D11DepthStencilView* pDepthStencilView;

	// Pour le mélange alpha (transparence)
	ID3D11BlendState* alphaBlendEnable;
	ID3D11BlendState* alphaBlendDisable;

	ID3D11RasterizerState* mSolidCullBackRS;

	void InitDepthBuffer();
	void InitBlendStates();
};

#endif
