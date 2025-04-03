#pragma once

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <functional>
#include <string>
#include <windows.h>

#include "ui.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Overlay {
public:
    Overlay();
    ~Overlay();

    bool Initialize(HWND targetWindow);
    void Shutdown();
    void Render(std::function<void()> drawCallback);
    bool ProcessMessages();

    HWND GetWindow() const { return m_overlayWindow; }
    bool IsInitialized() const { return m_initialized; }

private:
    bool CreateOverlayWindow(HWND targetWindow);
    bool InitializeDirectX();
    void CleanupDirectX();

    // window properties
    HWND m_overlayWindow;
    HWND m_targetWindow;
    RECT m_windowBounds;
    WNDCLASSEXW m_windowClass;

	// directx stuff
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    IDXGISwapChain* m_swapChain;
    ID3D11RenderTargetView* m_renderTargetView;

    bool m_initialized;

	// window procedure
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};