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

	bool Init();

	[[nodiscard]] HWND& GetMainWindow() { return mainWindow; }

	[[nodiscard]] static bool ProcessWindowMessages();

private:
	HINSTANCE instance{};
	HWND mainWindow{};

	LPCWSTR className{};
	LPCWSTR windowTitle{};

	[[nodiscard]] bool CreateMainWindow();
	[[nodiscard]] ATOM RegisterWindowClass() const;

	[[nodiscard]] static LRESULT ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	[[nodiscard]] static LRESULT CALLBACK HandleWindowsMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
