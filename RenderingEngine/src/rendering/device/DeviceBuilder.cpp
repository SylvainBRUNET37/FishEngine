#include "pch.h"
#include "rendering/device/DeviceBuilder.h"

#include "rendering/utils/Util.h"
#include "resources/resource.h"

RenderContext DeviceBuilder::CreateRenderContext(const HWND hwnd, const WindowData& windowData)
{
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	const size_t width = GetSystemMetrics(SM_CXSCREEN);
	const size_t height = GetSystemMetrics(SM_CYSCREEN);

	const auto swapChainDesc = CreateSwapChainDesc(hwnd, windowData);

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<IDXGISwapChain> swapChain;

	DXEssayer(
		D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			createDeviceFlags, featureLevels, _countof(featureLevels),
			D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, nullptr, &context),
		DXE_ERREURCREATIONDEVICE
	);

	return RenderContext{device, context, swapChain, windowData};
}

DXGI_SWAP_CHAIN_DESC DeviceBuilder::CreateSwapChainDesc(const HWND hwnd, const WindowData& windowData)
{
	DXGI_SWAP_CHAIN_DESC desc{};

	desc.BufferCount = 2;
	desc.BufferDesc.Width = static_cast<UINT>(windowData.screenWidth);
	desc.BufferDesc.Height = static_cast<UINT>(windowData.screenHeight);
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = hwnd;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = windowData.displayMode == DisplayMode::WINDOWED;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	return desc;
}
