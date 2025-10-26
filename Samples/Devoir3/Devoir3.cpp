#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdlib>
#include <minwindef.h>

#include "rendering/SceneLoader.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/device/DeviceBuilder.h"
#include "rendering/device/RenderContext.h"
#include "rendering/shaders/ShaderProgramDesc.h"
#include "rendering/shaders/ShaderFactory.h"
#include <rendering/ecs/EntityManager.h>

#include "Components.h"
#include "rendering/RenderSystem.h"

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

	Model CreateFiatModel(ID3D11Device* device, const ShaderBank& shaderBank)
	{
		const ShaderProgram shaderProgram
		{
			device,
			shaderBank.Get<VertexShader>("shaders/MiniPhongVS.hlsl"),
			shaderBank.Get<PixelShader>("shaders/MiniPhongPS.hlsl"),
		};

		const filesystem::path filePath = "assets\\fiat.glb";

		SceneLoader modelLoader{ shaderProgram };
		return modelLoader.LoadScene(filePath, device);
	}

	void WaitBeforeNextFrame(const DWORD frameStartTime)
	{
		static constexpr double TARGET_FPS = 60.0;
		static constexpr double FRAME_TIME = 1000.0 / TARGET_FPS;

		const DWORD frameEnd = GetTickCount();
		const DWORD frameDuration = frameEnd - frameStartTime;

		if (frameDuration < FRAME_TIME)
			Sleep(static_cast<DWORD>(FRAME_TIME - frameDuration));
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

	const auto windowData = application.GetWindowData();

	auto renderContext = DeviceBuilder::CreateRenderContext(application.GetMainWindow(), windowData);
	auto shaderBank = CreateShaderBank(renderContext.GetDevice());

	auto model = CreateFiatModel(renderContext.GetDevice(), shaderBank);

	RenderSystem renderSystem{ &renderContext };

	EntityManager<Transform, Renderable> entityManager;

	const auto entity = entityManager.Create();
	entityManager.AddComponent<Transform>(entity, XMMatrixIdentity());
	entityManager.AddComponent<Renderable>(entity, std::move(model));

	DWORD prevTime = GetTickCount();

	while (true)
	{
		const DWORD frameStartTime = GetTickCount();

		double elapsedTime = (frameStartTime - prevTime) / 1000.0f;
		prevTime = frameStartTime;

		if (not WindowsApplication::ProcessWindowMessages())
			break;

		renderSystem.UpdateScene(elapsedTime);

		for (const auto& [transform, renderable] : entityManager.View<Transform, Renderable>())
		{
			renderSystem.Draw(renderable.model, transform);
		}

		renderSystem.Render();

		WaitBeforeNextFrame(frameStartTime);
	}

	return EXIT_SUCCESS;
}
