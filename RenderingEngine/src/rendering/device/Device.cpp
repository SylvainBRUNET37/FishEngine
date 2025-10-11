#include "pch.h"
#include "rendering/device/Device.h"

#include "rendering/device/InfoDispositif.h"
#include "rendering/utils/Util.h"
#include "resources/resource.h"

Device::Device(const CDS_MODE cdsMode,
               const HWND hWnd)
{
	UINT largeur;
	UINT hauteur;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	// Obtenir les informations de l’adaptateur de défaut
	largeur = 1920;
	hauteur = 1080;
	switch (cdsMode)
	{
	case CDS_FENETRE:
		sd.Windowed = TRUE;
		break;
	case CDS_PLEIN_ECRAN:
		sd.Windowed = FALSE;
		break;
	}
	DXGI_MODE_DESC desc;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Height = hauteur;
	desc.Width = largeur;
	desc.RefreshRate.Numerator = 60;
	desc.RefreshRate.Denominator = 1;
	desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	CInfoDispositif DispoVoulu(desc);
	DispoVoulu.GetDesc(desc);
	largeur = desc.Width;
	hauteur = desc.Height;

	largeurEcran = largeur;
	hauteurEcran = hauteur;

	sd.BufferCount = 1;
	sd.BufferDesc.Width = desc.Width;
	sd.BufferDesc.Height = desc.Height;
	sd.BufferDesc.Format = desc.Format;
	sd.BufferDesc.RefreshRate.Numerator = desc.RefreshRate.Numerator;
	sd.BufferDesc.RefreshRate.Denominator = desc.RefreshRate.Denominator;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	// Permettre l’échange plein écran
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// régler le problème no 1 du passage en mode fenêtré
	RECT rcClient, rcWindow;
	POINT ptDiff;
	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	MoveWindow(hWnd, rcWindow.left, rcWindow.top, largeur + ptDiff.x,
		hauteur + ptDiff.y, TRUE);
	DXEssayer(D3D11CreateDeviceAndSwapChain(0,
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
	// Création d’un « render target view »
	ID3D11Texture2D* pBackBuffer;
	DXEssayer(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<LPVOID*>(&pBackBuffer)), DXE_ERREUROBTENTIONBUFFER);
	DXEssayer(pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr,
		&pRenderTargetView), DXE_ERREURCREATIONRENDERTARGET);
	pBackBuffer->Release();

	InitDepthBuffer();

	InitBlendStates();

	pImmediateContext->OMSetRenderTargets(1,
		&pRenderTargetView,
		pDepthStencilView);
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(largeur);
	vp.Height = static_cast<FLOAT>(hauteur);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pImmediateContext->RSSetViewports(1, &vp);

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthClipEnable = TRUE;

	ID3D11RasterizerState* rasterState = nullptr;
	pD3DDevice->CreateRasterizerState(&rasterDesc, &rasterState);
	pImmediateContext->RSSetState(rasterState);
}

void Device::Present() const
{
	pSwapChain->Present(0, 0);
}

void Device::InitDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthTextureDesc;
	ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
	depthTextureDesc.Width = largeurEcran;
	depthTextureDesc.Height = hauteurEcran;
	depthTextureDesc.MipLevels = 1;

	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDesc.SampleDesc.Count = 1;
	depthTextureDesc.SampleDesc.Quality = 0;
	depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTextureDesc.CPUAccessFlags = 0;
	depthTextureDesc.MiscFlags = 0;
	DXEssayer(pD3DDevice->CreateTexture2D(&depthTextureDesc,
		nullptr, &pDepthTexture),
		DXE_ERREURCREATIONTEXTURE);

	// Création de la vue du tampon de profondeur (ou de stencil)
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSView;
	ZeroMemory(&descDSView, sizeof(descDSView));
	descDSView.Format = depthTextureDesc.Format;
	descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSView.Texture2D.MipSlice = 0;
	DXEssayer(pD3DDevice->CreateDepthStencilView(pDepthTexture,
		&descDSView,
		&pDepthStencilView),
		DXE_ERREURCREATIONDEPTHSTENCILTARGET);
}

void Device::InitBlendStates()
{
	D3D11_BLEND_DESC blendDesc;
	// Effacer la description
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	// On initialise la description pour un mélange alpha classique
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_ALL;
	// On créé l’état alphaBlendEnable
	DXEssayer(pD3DDevice->CreateBlendState(&blendDesc, &alphaBlendEnable),
		DXE_ERREURCREATION_BLENDSTATE);

	// Seul le booleen BlendEnable nécessite d’être modifié
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	// On créé l’état alphaBlendDisable
	DXEssayer(pD3DDevice->CreateBlendState(&blendDesc, &alphaBlendDisable),
		DXE_ERREURCREATION_BLENDSTATE);
}
