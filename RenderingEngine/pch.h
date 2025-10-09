#ifndef PCH_H
#define PCH_H

#include <iostream>
#include <vector>
#include <string>

#include <Windows.h>
#include <tchar.h>

#ifndef XM_NO_INTRINSICS_
	#define XM_NO_INTRINSICS_
#endif

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dinput.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXGI.lib")

#endif
