#include "overlay.hpp"
#include <iostream>

Overlay::Overlay() :
    m_overlayWindow(nullptr),
    m_targetWindow(nullptr),
    m_device(nullptr),
    m_deviceContext(nullptr),
    m_swapChain(nullptr),
    m_renderTargetView(nullptr),
    m_initialized(false)
{
    ZeroMemory(&m_windowBounds, sizeof(RECT));
    ZeroMemory(&m_windowClass, sizeof(WNDCLASSEXW));
}

Overlay::~Overlay() {
    Shutdown();
}

bool Overlay::Initialize(HWND targetWindow) {
    m_targetWindow = targetWindow;

    std::cout << "[overlay] Initializing overlay for target window" << std::endl;

    if (!CreateOverlayWindow(targetWindow)) {
        std::cout << "[overlay] Failed to create overlay window" << std::endl;
        return false;
    }

    if (!InitializeDirectX()) {
        std::cout << "[overlay] Failed to initialize DirectX" << std::endl;
        return false;
    }

    // imgui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 0.9f;
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.3f, 0.6f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.4f, 0.8f, 1.0f);

    ImGui_ImplWin32_Init(m_overlayWindow);
    ImGui_ImplDX11_Init(m_device, m_deviceContext);

    m_initialized = true;
    return true;
}

void Overlay::Shutdown() {
    if (!m_initialized)
        return;

    // cleanup imgui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // cleanup DirectX
    CleanupDirectX();

    // destroy window
    if (m_overlayWindow) {
        std::cout << "[overlay] Destroying overlay window" << std::endl;
        DestroyWindow(m_overlayWindow);
        m_overlayWindow = nullptr;
    }

    m_initialized = false;
}

bool Overlay::CreateOverlayWindow(HWND targetWindow) {
    // get the game window bounds
    GetClientRect(targetWindow, &m_windowBounds);

    // register overlay window class
    HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongA(targetWindow, (-6)));

    m_windowClass.cbSize = sizeof(WNDCLASSEXW);
    m_windowClass.style = CS_HREDRAW | CS_VREDRAW;
    m_windowClass.lpfnWndProc = WndProc;
    m_windowClass.hInstance = hInstance;
    m_windowClass.lpszClassName = L" ";

    RegisterClassExW(&m_windowClass);

    // create overlay window with proper styles for menu interaction
    m_overlayWindow = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE,
        L" ", L"cs2", WS_POPUP,
        m_windowBounds.left, m_windowBounds.top,
        m_windowBounds.right - m_windowBounds.left,
        m_windowBounds.bottom - m_windowBounds.top,
        NULL, NULL, hInstance, NULL
    );

    if (m_overlayWindow == NULL) {
        std::cout << "[overlay] Failed to create window" << std::endl;
        return false;
    }

    // transparent color
    SetLayeredWindowAttributes(m_overlayWindow, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // DWM transparency
    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(m_overlayWindow, &margins);

    // borderless window and transparent
    BOOL composition_enabled = FALSE;
    DwmIsCompositionEnabled(&composition_enabled);
    if (composition_enabled) {
        DWM_BLURBEHIND bb = { 0 };
        bb.dwFlags = DWM_BB_ENABLE;
        bb.fEnable = TRUE;
        DwmEnableBlurBehindWindow(m_overlayWindow, &bb);
    }

    // set window pos and show
    SetWindowPos(m_overlayWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(m_overlayWindow, SW_SHOW);
    UpdateWindow(m_overlayWindow);

    return true;
}

bool Overlay::InitializeDirectX() {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferDesc.RefreshRate.Numerator = 60U;
    sd.BufferDesc.RefreshRate.Denominator = 1U;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.SampleDesc.Count = 1U;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2U;
    sd.OutputWindow = m_overlayWindow;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    constexpr D3D_FEATURE_LEVEL levels[2]{
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
    };

    D3D_FEATURE_LEVEL level{};

    // debug flag if needed
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        levels,
        2U,
        D3D11_SDK_VERSION,
        &sd,
        &m_swapChain,
        &m_device,
        &level,
        &m_deviceContext
    );

    if (FAILED(hr)) {
        std::cout << "[overlay] Failed to create D3D11 device: " << std::hex << hr << std::endl;
        return false;
    }

    ID3D11Texture2D* back_buffer{ nullptr };
    hr = m_swapChain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

    if (FAILED(hr) || !back_buffer) {
        std::cout << "[overlay] Failed to get back buffer: " << std::hex << hr << std::endl;
        return false;
    }

    hr = m_device->CreateRenderTargetView(back_buffer, nullptr, &m_renderTargetView);
    back_buffer->Release();

    if (FAILED(hr)) {
        std::cout << "[overlay] Failed to create render target view: " << std::hex << hr << std::endl;
        return false;
    }

    return true;
}

void Overlay::CleanupDirectX() {
    if (m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }

    if (m_swapChain) {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }

    if (m_deviceContext) {
        m_deviceContext->Release();
        m_deviceContext = nullptr;
    }

    if (m_device) {
        m_device->Release();
        m_device = nullptr;
    }
}

void Overlay::Render(std::function<void()> drawCallback) {
    if (!m_initialized)
        return;

    // start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // exec drawcallback
    if (drawCallback) {
        drawCallback();
    }

    // set window transparency based on menu visibility
    if (ui::menu_visible) {
        // remove transparent click-through style
        LONG ex_style = GetWindowLong(m_overlayWindow, GWL_EXSTYLE);
        SetWindowLong(m_overlayWindow, GWL_EXSTYLE, (ex_style & ~WS_EX_TRANSPARENT));

        // enable mouse input in imgui
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

        SetForegroundWindow(m_overlayWindow);
        SetActiveWindow(m_overlayWindow);
    }
    else {
        // transparent click-through style
        LONG ex_style = GetWindowLong(m_overlayWindow, GWL_EXSTYLE);
        SetWindowLong(m_overlayWindow, GWL_EXSTYLE, (ex_style | WS_EX_TRANSPARENT));

        // disable mouse input
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }

    // render ImGui
    ImGui::Render();

    // set render target and clear with transparent color
    const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, clear_color);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // no vsync
    m_swapChain->Present(0, 0);
}

bool Overlay::ProcessMessages() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	// chgeck if target window is still valid
    if (!IsWindow(m_targetWindow)) {
        return false;
    }

    return true;
}
LRESULT CALLBACK Overlay::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // handle imgui msgs
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
        return TRUE; 
    }

    // handle window messages
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}