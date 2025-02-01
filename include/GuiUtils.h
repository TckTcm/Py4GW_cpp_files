#pragma once
#include <d3d9.h>

namespace GuiUtils {
    bool Initialize(IDirect3DDevice9* device);
    void Terminate();

    bool InitializeImGui(IDirect3DDevice9* device, HWND hwnd);
    void TerminateImGui();

    void LoadFonts();
    bool FontsLoaded();

    void PreDraw();
    void PostDraw();
}