
#include <iostream>
#include <vector>
#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdlib>
#include <minwindef.h>

#include "rendering/ModelLoader.h"
#include "rendering/RenderingEngine.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/device/GraphicsDevice.h"
#include <rendering/shaders/ShaderProgramDesc.h>

using namespace std;
using namespace DirectX;

namespace
{
	ShaderBank CreateShaderBank(ID3D11Device* device)
	{
		ShaderProgramDesc<VertexShader, PixelShader> desc;

		desc.AddShader<VertexShader>("shaders/MiniPhongVS.hlsl", "MiniPhongVS", "vs_5_0")
			.AddShader<PixelShader>("shaders/MiniPhongPS.hlsl", "MiniPhongPS", "ps_5_0");

		ShaderFactory<VertexShader, PixelShader> shaderFactory;
		auto shaderBank = shaderFactory.Compile(desc, device);

		return std::move(shaderBank);
	}
}

int APIENTRY _tWinMain(const HINSTANCE hInstance,
	HINSTANCE,
	LPTSTR,
	int)
{
	WindowsApplication application{ hInstance, L"RenderingEngineSample", L"RenderingEngineSampleClass" };

	if (!application.Init())
		return EXIT_FAILURE;

	const auto device = new GraphicsDevice{ GraphicsDevice::CDS_FENETRE, application.GetMainWindow() };
	auto shaderBank = CreateShaderBank(device->GetD3DDevice());
	RenderingEngine renderingEngine{ device, std::move(shaderBank), {WindowsApplication::ProcessWindowMessages} };

	//const auto wallMat = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	//renderingEngine.AddObjectToScene(wallMat, 2.0f, 2.0f, 2.0f);


	renderingEngine.Run();

	return EXIT_SUCCESS;
}
