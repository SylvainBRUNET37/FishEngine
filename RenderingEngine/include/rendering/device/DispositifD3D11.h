#ifndef CDISPOSITIF_D3D11_H
#define CDISPOSITIF_D3D11_H

#include "Dispositif.h"

namespace PM3D
{
	class CDispositifD3D11 : public CDispositif
	{
	public:
		CDispositifD3D11(CDS_MODE cdsMode, HWND hWnd);
		~CDispositifD3D11() override;
		void PresentSpecific() override;

		[[nodiscard]] ID3D11Device* GetD3DDevice() const { return pD3DDevice; }
		[[nodiscard]] ID3D11DeviceContext* GetImmediateContext() const { return pImmediateContext; }
		[[nodiscard]] IDXGISwapChain* GetSwapChain() const { return pSwapChain; }
		[[nodiscard]] ID3D11RenderTargetView* GetRenderTargetView() const { return pRenderTargetView; }
	private:
		ID3D11Device* pD3DDevice;
		ID3D11DeviceContext* pImmediateContext;
		IDXGISwapChain* pSwapChain;
		ID3D11RenderTargetView* pRenderTargetView;
	};
}


#endif
