#ifndef DEBUG_UI
#define DEBUG_UI

// Initialize and destroy ImGui
struct DebugUI
{
    explicit DebugUI(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
    ~DebugUI();

    DebugUI() = delete;

	DebugUI(const DebugUI&) = default;
    DebugUI(DebugUI&&) = default;
    DebugUI& operator=(const DebugUI&) = default;
    DebugUI& operator=(DebugUI&&) = default;
};

#endif