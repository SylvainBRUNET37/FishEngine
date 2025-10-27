#ifndef WINDOWS_APPLICATION_H
#define WINDOWS_APPLICATION_H

#include "rendering/core/WindowData.h"
#include "rendering/device/BlendState.h"

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
		windowData.screenWidth = GetSystemMetrics(SM_CXSCREEN);
		windowData.screenHeight = GetSystemMetrics(SM_CYSCREEN);
		windowData.displayMode = DisplayMode::WINDOWED; // Windowed by default but should be a setting
	}

	bool Init();

	[[nodiscard]] HWND& GetMainWindow() { return mainWindow; }
	[[nodiscard]] WindowData GetWindowData() const noexcept { return windowData; }

	[[nodiscard]] static bool ProcessWindowsMessages();

private:
	HINSTANCE instance{};
	HWND mainWindow{};

	LPCWSTR className{};
	LPCWSTR windowTitle{};

	WindowData windowData{};

	[[nodiscard]] bool CreateMainWindow();
	[[nodiscard]] ATOM RegisterWindowClass() const;

	[[nodiscard]] static LRESULT ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	[[nodiscard]] static LRESULT CALLBACK HandleWindowsMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
