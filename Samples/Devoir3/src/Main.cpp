#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdlib>
#include <minwindef.h>

#include "Components.h"
#include "ResourceManager.h"
#include "rendering/SceneLoader.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/device/DeviceBuilder.h"
#include "rendering/device/RenderContext.h"
#include "rendering/shaders/ShaderProgramDesc.h"
#include "rendering/RenderSystem.h"

#include "EntityManagerFactory.h"

using namespace std;
using namespace DirectX;

namespace
{
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
	WindowsApplication application{hInstance, L"RenderingEngineSample", L"RenderingEngineSampleClass"};

	if (!application.Init())
		return EXIT_FAILURE;

	const auto windowData = application.GetWindowData();

	auto renderContext = DeviceBuilder::CreateRenderContext(application.GetMainWindow(), windowData);

	ResourceManager resourceManager{ renderContext.GetDevice() };
	auto sceneResources = resourceManager.LoadScene();

	RenderSystem renderSystem{ &renderContext, std::move(sceneResources.materials) };

	EntityManagerFactory<Transform, Mesh, Hierarchy> entityManagerFactory;
	auto entityManager = entityManagerFactory.Create(sceneResources);

	DWORD prevTime = GetTickCount();

	while (true)
	{
		const DWORD frameStartTime = GetTickCount();

		double elapsedTime = (frameStartTime - prevTime) / 1000.0f;
		prevTime = frameStartTime;

		if (not WindowsApplication::ProcessWindowMessages())
			break;

		renderSystem.UpdateScene(elapsedTime);

		for (const auto& [transform, mesh] : entityManager.View<Transform, Mesh>())
		{
			renderSystem.Render(mesh, transform);
		}

		renderSystem.Render();

		WaitBeforeNextFrame(frameStartTime);
	}

	return EXIT_SUCCESS;
}
