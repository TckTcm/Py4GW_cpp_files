#pragma once

#include "Headers.h"
// Forward declarations
namespace GW {
    struct HookStatus;
}

class DLLMain {
public:
    static DLLMain& Instance();

    // Core initialization and cleanup
    bool Initialize();
    void Terminate();

    // Main update functions
    void Update(GW::HookStatus* status = nullptr);
    void Draw(IDirect3DDevice9* device);

    // State management
    bool IsRunning() const { return running; }
    bool IsInitialized() const { return initialized; }
    void SignalTerminate() { running = false; }

    // Window management
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK SafeWndProc(const HWND hWnd, const UINT Message, const WPARAM wParam, const LPARAM lParam) noexcept;
    bool AttachWndProc();
    void DetachWndProc();

    // DX9 hooks management
    bool AttachRenderHook();
    void DetachRenderHook();

private:
    DLLMain();
    ~DLLMain();
    DLLMain(const DLLMain&) = delete;
    DLLMain& operator=(const DLLMain&) = delete;

    // Initialization helpers
    bool InitializeGWCA();
	bool InitializePy4GW();
    bool InitializeImGui(IDirect3DDevice9* device);

    // State variables
    
    bool running;
    bool initialized;
    bool imgui_initialized;
    bool imgui_initializing = false;
    bool render_hook_attached;
    bool wndproc_attached;
    WNDPROC old_wndproc;
    HWND gw_window_handle;

    // Time tracking
    DWORD last_tick;

};

// Thread procedure for the main loop
DWORD WINAPI MainLoop(LPVOID lpParam);