#include "pch.h"
#include "rendering/application/WindowsApplication.h"

#include "rendering/utils/VerboseAssertion.h"

bool WindowsApplication::Init()
{
	if (!RegisterWindowClass())
		return false;

	if (!CreateMainWindow())
		return false;

	ShowWindow(mainWindow, SW_SHOW);
	UpdateWindow(mainWindow);

	return true;
}

bool WindowsApplication::ProcessWindowsMessages()
{
	MSG msg;

	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return true;
}

bool WindowsApplication::CreateMainWindow()
{
	mainWindow = CreateWindowExW
	(
		0,
		className,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		static_cast<int>(windowData.screenWidth), static_cast<int>(windowData.screenHeight),
		nullptr,
		nullptr,
		instance,
		this
	);

	return mainWindow != nullptr;
}

ATOM WindowsApplication::RegisterWindowClass() const
{
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &WindowsApplication::HandleWindowsMessage;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);

	return RegisterClassExW(&wcex);
}

LRESULT WindowsApplication::ProcessWindowMessage(const HWND hWnd, const UINT msg, const WPARAM wp, const LPARAM lp) const
{
	static constexpr LRESULT MESSAGE_HANDLED = 0;

	switch (msg)
	{
	case WM_PAINT:
		PAINTSTRUCT ps;

		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);

		break;
	case WM_DESTROY:
		PostQuitMessage(MESSAGE_HANDLED);
		break;
	case WM_MOUSEWHEEL:
	{
		const int delta = GET_WHEEL_DELTA_WPARAM(wp);
		mouseWheelDelta += delta;              
		return MESSAGE_HANDLED;
	}
	case WM_SIZE:
	{
		if (renderContext)
		{
			const UINT width = LOWORD(lp);
			const UINT height = HIWORD(lp);

			if (width > 0 && height > 0)
				renderContext->Resize(width, height);
		}
		break;
	}
	case WM_MOVE:
	{
		int x = (int)(short)LOWORD(lp);
		int y = (int)(short)HIWORD(lp);

		if (x >= 0 && y >= 0 && renderContext)
			renderContext->Move(x, y);
	}
	break;

	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}

	return MESSAGE_HANDLED;
}

LRESULT WindowsApplication::HandleWindowsMessage(const HWND hWnd, const UINT message, const WPARAM wParam,
                                                 const LPARAM lParam)
{
	WindowsApplication* app = nullptr;

	if (message == WM_NCCREATE)
	{
		const auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		app = static_cast<WindowsApplication*>(cs->lpCreateParams);

		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));

		app->mainWindow = hWnd;

		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
		
	app = reinterpret_cast<WindowsApplication*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (app)
		return app->ProcessWindowMessage(hWnd, message, wParam, lParam);

	return DefWindowProcW(hWnd, message, wParam, lParam);
}
