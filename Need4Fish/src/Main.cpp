#include "pch.h"

#include "components/Components.h"
#include "PhysicsEngine/JoltSystem.h"
#include "PhysicsEngine/layers/BroadPhaseLayerInterfaceImpl.h"
#include "PhysicsEngine/layers/BroadPhaseLayers.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/device/DeviceBuilder.h"
#include "rendering/device/RenderContext.h"
#include "resources/ResourceManager.h"

#include "GameEngine.h"

using namespace JPH;
using namespace JPH::literals;
using namespace std;
using namespace DirectX;

namespace
{
	void CreateDebugConsoleWindow()
	{
		// Reference: https://stackoverflow.com/questions/16703835/how-can-i-see-cout-output-in-a-non-console-application
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}
}

int APIENTRY _tWinMain(const HINSTANCE hInstance,
                       HINSTANCE,
                       LPTSTR,
                       int)
{
	try
	{
#ifndef NDEBUG
		CreateDebugConsoleWindow();
#endif

		// Create the window
		WindowsApplication application{ hInstance, L"Need4Fish", L"Need4FishClass" };

		if (!application.Init())
			return EXIT_FAILURE;

		// Init render context
		auto renderContext = 
			DeviceBuilder::CreateRenderContext(application.GetMainWindow(), application.GetWindowData());

		// Init physics
		JoltSystem joltSystem;

		// Init game engine & run the main loop !
		GameEngine gameEngine{ &renderContext };
		gameEngine.Run();

		return EXIT_SUCCESS;
	}
	catch (const std::exception& ex)
	{
		std::cout << "The following exception occured:\n" << ex.what() << '\n';

		MessageBoxA(nullptr, ex.what(), "An exception has been thrown", MB_ICONERROR | MB_OK);

		return EXIT_FAILURE;
	}
}
