#ifndef WINDOWS_APPLICATION_H
#define WINDOWS_APPLICATION_H

#include <d3d11.h>

#include "rendering/core/WindowData.h"
#include "rendering/device/RenderContext.h"

class WindowsApplication
{
public:
	// For the zoom
	inline static int mouseWheelDelta = 0;

	~WindowsApplication() = default;
	WindowsApplication(const WindowsApplication&) = delete;
	WindowsApplication& operator=(const WindowsApplication&) = delete;
	WindowsApplication(WindowsApplication&&) = delete;
	WindowsApplication& operator=(WindowsApplication&&) = delete;

	explicit WindowsApplication(const HINSTANCE hInstance, const LPCWSTR windowTitle, const LPCWSTR className)
		: instance(hInstance), className(className), windowTitle(windowTitle), renderContext{nullptr}
	{
		windowData.screenWidth = GetSystemMetrics(SM_CXSCREEN);
		windowData.screenHeight = GetSystemMetrics(SM_CYSCREEN);
		windowData.displayMode = DisplayMode::WINDOWED; // Windowed by default but should be a setting
	}

	bool Init();
	void SetRenderContext(RenderContext* renderContext_) { if (renderContext_) renderContext = renderContext_; }

	[[nodiscard]] HWND& GetMainWindow() { return mainWindow; }
	[[nodiscard]] WindowData GetWindowData() const noexcept { return windowData; }

	[[nodiscard]] static bool ProcessWindowsMessages();

private:
	HINSTANCE instance{};
	HWND mainWindow{};

	LPCWSTR className{};
	LPCWSTR windowTitle{};

	WindowData windowData{};

	RenderContext* renderContext; // has to be set with the SetRenderContext function

	[[nodiscard]] bool CreateMainWindow();
	[[nodiscard]] ATOM RegisterWindowClass() const;

	[[nodiscard]] LRESULT ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) const;
	[[nodiscard]] static LRESULT CALLBACK HandleWindowsMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
