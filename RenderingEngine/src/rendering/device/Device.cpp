#include "pch.h"
#include "rendering/device/Device.h"

#include "rendering/utils/Util.h"
#include "resources/resource.h"

Device::Device(const CDS_MODE cdsMode,
               const HWND hWnd)
{
	int largeur;
	int hauteur;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	constexpr UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	switch (cdsMode)
	{
	case CDS_FENETRE:
		RECT rc;
		GetClientRect(hWnd, &rc);
		largeur = rc.right - rc.left;
		hauteur = rc.bottom - rc.top;
		sd.BufferCount = 1;
		sd.BufferDesc.Width = largeur;
		sd.BufferDesc.Height = hauteur;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		break;
	}
	// Création dispositif, chaine d'échange et contexte
	DXEssayer(D3D11CreateDeviceAndSwapChain(nullptr,
	                                        D3D_DRIVER_TYPE_HARDWARE,
	                                        nullptr,
	                                        createDeviceFlags,
	                                        featureLevels, numFeatureLevels,
	                                        D3D11_SDK_VERSION,
	                                        &sd,
	                                        &pSwapChain,
	                                        &pD3DDevice,
	                                        nullptr,
	                                        &pImmediateContext),
		DXE_ERREURCREATIONDEVICE);
	// Création d'un « render target view »
	ID3D11Texture2D* pBackBuffer;
	DXEssayer(pSwapChain->GetBuffer(0,
	                                __uuidof(ID3D11Texture2D),
	                                reinterpret_cast<LPVOID*>(&pBackBuffer)),
		DXE_ERREUROBTENTIONBUFFER);
	DXEssayer(pD3DDevice->CreateRenderTargetView(pBackBuffer,
	                                             nullptr,
	                                             &pRenderTargetView),
								DXE_ERREURCREATIONRENDERTARGET);
	pBackBuffer->Release();
	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(largeur);
	vp.Height = static_cast<FLOAT>(hauteur);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pImmediateContext->RSSetViewports(1, &vp);
}

void Device::Present() const
{
	pSwapChain->Present(0, 0);
}
