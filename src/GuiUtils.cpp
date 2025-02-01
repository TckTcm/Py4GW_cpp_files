#pragma once
#include "GuiUtils.h"
#include <GWCA/Managers/MemoryMgr.h>

namespace {
    bool fonts_loaded = false;
}

namespace GuiUtils {
    bool Initialize(IDirect3DDevice9* device) {
        if (!InitializeImGui(device, GW::MemoryMgr::GetGWWindowHandle())) {
            return false;
        }
        LoadFonts();
        return true;
    }

    void Terminate() {
        TerminateImGui();
    }

    bool InitializeImGui(IDirect3DDevice9* device, HWND hwnd) {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX9_Init(device);
        
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = false;
        io.IniFilename = "imgui_settings.ini";

        return true;
    }

    void TerminateImGui() {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void LoadFonts() {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();
        fonts_loaded = true;
    }

    bool FontsLoaded() {
        return fonts_loaded;
    }

    void PreDraw() {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void PostDraw() {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }
}