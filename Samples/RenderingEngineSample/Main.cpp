#include <iostream>
#include <vector>
#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdlib>
#include <minwindef.h>

#include "rendering/SceneLoader.h"
#include "rendering/RenderingEngine.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/device/DeviceBuilder.h"
#include "rendering/device/RenderContext.h"
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

		return shaderBank;
	}

	Model CreateHumanModel(ID3D11Device* device, const ShaderBank& shaderBank)
	{
		ShaderProgram shaderProgram
		{
			device,
			shaderBank.Get<VertexShader>("shaders/MiniPhongVS.hlsl"),
			shaderBank.Get<PixelShader>("shaders/MiniPhongPS.hlsl"),
		};

		//const filesystem::path filePath = "assets\\Jin\\jin.obj";
		const filesystem::path filePath = "assets\\terrain.glb";

		SceneLoader modelLoader;
		return modelLoader.LoadScene(filePath, device, std::move(shaderProgram));
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

	const auto windowData = application.GetWindowData();

	auto renderContext = DeviceBuilder::CreateRenderContext(application.GetMainWindow(), windowData);
	auto shaderBank = CreateShaderBank(renderContext.GetDevice());
	RenderingEngine renderingEngine{&renderContext, std::move(shaderBank), {WindowsApplication::ProcessWindowMessages}};

	renderingEngine.AddObjectToScene(CreateHumanModel(renderContext.GetDevice(), std::move(shaderBank)));
	renderingEngine.Run();

	return EXIT_SUCCESS;
}
