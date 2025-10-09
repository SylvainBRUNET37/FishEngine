#ifndef WINDOWS_APPLICATION_H
#define WINDOWS_APPLICATION_H

#include "systems/MainEngine.h"

class WindowsApplication
{
public:
	~WindowsApplication() = default;
	WindowsApplication(const WindowsApplication&) = delete;
	WindowsApplication& operator=(const WindowsApplication&) = delete;
	WindowsApplication(WindowsApplication&&) = delete;
	WindowsApplication& operator=(WindowsApplication&&) = delete;

	explicit WindowsApplication(const HINSTANCE hInstance)
		: instance(hInstance)
	{
	}

	bool Init()
	{
		if (!RegisterWindowClass())
			return false;

		if (!CreateMainWindow())
			return false;

		ShowWindow(mainWindow, SW_SHOW);
		UpdateWindow(mainWindow);

		return true;
	}

	[[nodiscard]] HWND& GetMainWindow() { return mainWindow; }

	static bool ProcessWindowMessages()
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

private:
	static constexpr auto className = L"PhysicsEngineWindowClass";
	static constexpr auto windowTitle = L"PhysicsEngine";

	HINSTANCE instance{};
	HWND mainWindow{};

	bool CreateMainWindow()
	{
		mainWindow = CreateWindowExW
		(
			0,
			className,
			windowTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr,
			nullptr,
			instance,
			this
		);

		return mainWindow != nullptr;
	}

	[[nodiscard]] ATOM RegisterWindowClass() const
	{
		WNDCLASSEXW wcex{};

		wcex.cbSize = sizeof(wcex);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = &WindowsApplication::WindowsMessageHandler;
		wcex.hInstance = instance;
		wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcex.lpszClassName = className;
		wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);

		return RegisterClassExW(&wcex);
	}

	[[nodiscard]] static LRESULT HandleWindowsMessage(const HWND hWnd, const UINT msg, const WPARAM wp, const LPARAM lp)
	{
		static constexpr LRESULT MESSAGE_HANDLED = 0;

		if (msg == WM_DESTROY)
		{
			PostQuitMessage(MESSAGE_HANDLED);

			return MESSAGE_HANDLED;
		}

		return DefWindowProcW(hWnd, msg, wp, lp);
	}

	static LRESULT CALLBACK WindowsMessageHandler(const HWND hWnd, const UINT message, const WPARAM wParam,
	                                              const LPARAM lParam)
	{
		if (message == WM_NCCREATE)
		{
			const auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));

			return DefWindowProcW(hWnd, message, wParam, lParam);
		}

		if (const auto self = reinterpret_cast<WindowsApplication*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA)))
			return HandleWindowsMessage(hWnd, message, wParam, lParam);

		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
};

#endif
