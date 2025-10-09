#include <iostream>
#include <vector>
#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdlib>
#include <minwindef.h>

#include "rendering/application/WindowsApplication.h"
#include "systems/MainEngine.h"

using namespace std;
using namespace DirectX;

int APIENTRY _tWinMain(const HINSTANCE hInstance,
	HINSTANCE,
	LPTSTR,
	int)
{
	WindowsApplication application{ hInstance };

	if (!application.Init())
		return EXIT_FAILURE;

	const auto dispositif = new PM3D::CDispositifD3D11{ PM3D::CDS_FENETRE, application.GetMainWindow() };
	MainEngine mainEngine{ dispositif, {WindowsApplication::ProcessWindowMessages} };

	const auto wallMat = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	mainEngine.AddObjectToScene(wallMat, 2.0f, 2.0f, 2.0f);
	mainEngine.Run();

	return EXIT_SUCCESS;
}
