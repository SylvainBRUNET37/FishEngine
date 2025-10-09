#ifndef WINDOWS_APPLICATION_H
#define WINDOWS_APPLICATION_H

class WindowsApplication
{
public:
	~WindowsApplication() = default;
	WindowsApplication(const WindowsApplication&) = delete;
	WindowsApplication& operator=(const WindowsApplication&) = delete;
	WindowsApplication(WindowsApplication&&) = delete;
	WindowsApplication& operator=(WindowsApplication&&) = delete;

	explicit WindowsApplication(const HINSTANCE hInstance, const LPCWSTR windowTitle, const LPCWSTR className)
		: instance(hInstance), className(className), windowTitle(windowTitle)
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
	HINSTANCE instance{};
	HWND mainWindow{};

	LPCWSTR className{};
	LPCWSTR windowTitle{};

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
		wcex.lpfnWndProc = &WindowsApplication::HandleWindowsMessage;
		wcex.hInstance = instance;
		wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcex.lpszClassName = className;
		wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);

		return RegisterClassExW(&wcex);
	}

	[[nodiscard]] static LRESULT ProcessWindowMessage(const HWND hWnd, const UINT msg, const WPARAM wp, const LPARAM lp)
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
		default:
			return DefWindowProcW(hWnd, msg, wp, lp);
		}

		return MESSAGE_HANDLED;
	}

	static LRESULT CALLBACK HandleWindowsMessage(const HWND hWnd, const UINT message, const WPARAM wParam,
	                                              const LPARAM lParam)
	{
		if (message == WM_NCCREATE)
		{
			const auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));

			return DefWindowProcW(hWnd, message, wParam, lParam);
		}

		if (const auto self = reinterpret_cast<WindowsApplication*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA)))
			return ProcessWindowMessage(hWnd, message, wParam, lParam);

		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
};

#endif
