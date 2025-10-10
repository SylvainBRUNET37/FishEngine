
#include <iostream>
#include <vector>
#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdlib>
#include <minwindef.h>

#include "rendering/RenderingEngine.h"
#include "rendering/application/WindowsApplication.h"
#include "rendering/device/Device.h"

using namespace std;
using namespace DirectX;

int APIENTRY _tWinMain(const HINSTANCE hInstance,
	HINSTANCE,
	LPTSTR,
	int)
{
	WindowsApplication application{ hInstance, L"RenderingEngineSample", L"RenderingEngineSampleClass" };

	if (!application.Init())
		return EXIT_FAILURE;

	const auto device = new Device{ Device::CDS_FENETRE, application.GetMainWindow() };
	RenderingEngine renderingEngine{ device, {WindowsApplication::ProcessWindowMessages} };

	const auto wallMat = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	renderingEngine.AddObjectToScene(wallMat, 2.0f, 2.0f, 2.0f);
	renderingEngine.Run();

	return EXIT_SUCCESS;
}
