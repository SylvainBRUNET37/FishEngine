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
#include "rendering/shaders/ShaderProgramDesc.h"
#include "rendering/shaders/ShaderFactory.h"

using namespace std;
using namespace DirectX;

namespace
{
	ShaderBank CreateShaderBank(ID3D11Device* device)
	{
		ShaderProgramDesc<VertexShader, PixelShader> desc;

		desc.AddDesc<VertexShader>("shaders/MiniPhongVS.hlsl", "MiniPhongVS", "vs_5_0")
		    .AddDesc<PixelShader>("shaders/MiniPhongPS.hlsl", "MiniPhongPS", "ps_5_0");

		ShaderFactory<VertexShader, PixelShader> shaderFactory;
		auto shaderBank = shaderFactory.CreateShaderBank(desc, device);

		shaderBank.Set<ShaderBank::Layout>("MiniPhong",
		                                   {
			                                   {
				                                   "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
				                                   D3D11_INPUT_PER_VERTEX_DATA, 0
			                                   },
			                                   {
				                                   "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
				                                   D3D11_INPUT_PER_VERTEX_DATA, 0
			                                   },
			                                   {
				                                   "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
				                                   D3D11_INPUT_PER_VERTEX_DATA, 0
			                                   }
		                                   });

		return std::move(shaderBank);
	}

	Model CreateHumanModel(const GraphicsDevice* device, const ShaderBank& shaderBank)
	{
		ShaderProgram shaderProgram
		{
			device->GetD3DDevice(),
			shaderBank.Get<VertexShader>("shaders/MiniPhongVS.hlsl"),
			shaderBank.Get<PixelShader>("shaders/MiniPhongPS.hlsl"),
			shaderBank.Get<ShaderBank::Layout>("MiniPhong")
		};

		//const filesystem::path filePath = "assets\\Jin\\jin.obj";
		const filesystem::path filePath = "assets\\fiat.glb";

		ModelLoader modelLoader;
		return modelLoader.LoadModel(filePath, device, std::move(shaderProgram));
	}
}

int APIENTRY _tWinMain(const HINSTANCE hInstance,
                       HINSTANCE,
                       LPTSTR,
                       int)
{
	WindowsApplication application{hInstance, L"RenderingEngineSample", L"RenderingEngineSampleClass"};

	if (!application.Init())
		return EXIT_FAILURE;

	const auto device = new GraphicsDevice{GraphicsDevice::CDS_FENETRE, application.GetMainWindow()};
	auto shaderBank = CreateShaderBank(device->GetD3DDevice());
	RenderingEngine renderingEngine{device, std::move(shaderBank), {WindowsApplication::ProcessWindowMessages}};

	renderingEngine.AddObjectToScene(CreateHumanModel(device, shaderBank));
	renderingEngine.Run();

	return EXIT_SUCCESS;
}
