#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>

#include "render/render.hpp"
#include "classes/config.hpp"
#include "mem/reader.hpp"
#include "hacks/hack.hpp"

bool finish = false;

// basic window procedure for the overlay window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
    {
        g::hdcBuffer = CreateCompatibleDC(NULL);

		if (g::hdcBuffer == NULL)
		{
			std::cout << "[overlay] Failed to create buffer" << std::endl;
			return 0;
		}

        g::hbmBuffer = CreateCompatibleBitmap(GetDC(hWnd), g_game.game_bounds.right, g_game.game_bounds.bottom);
        SelectObject(g::hdcBuffer, g::hbmBuffer);

        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), 0, LWA_COLORKEY);

        std::cout << "[overlay] Window created successfully" << std::endl;
        Beep(500, 100);
        break;
    }
    case WM_ERASEBKGND:
        return TRUE;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        FillRect(g::hdcBuffer, &ps.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));

		// if the game window is in focus, draw the ESP
        if (GetForegroundWindow() == g_game.windowHandle) {
            hack::draw();
        }

        BitBlt(hdc, 0, 0, g_game.game_bounds.right, g_game.game_bounds.bottom, g::hdcBuffer, 0, 0, SRCCOPY);

        EndPaint(hWnd, &ps);
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_DESTROY:
        DeleteDC(g::hdcBuffer);
        DeleteObject(g::hbmBuffer);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// game data reading thread
void read_thread() {
    while (!finish) {
        try {
            g_game.loop();
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] Exception in loop: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

// triggerbot thread
void triggerbot_thread() {
	while (!finish) {
		try {
			hack::triggerbot();
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] Exception in triggerbot: " << e.what() << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

int main() {
    std::cout << "[general] Starting up cheat..." << std::endl;

    // config reading
    std::cout << "[config] Reading configuration." << std::endl;
    if (config::read())
        std::cout << "[updater] Sucessfully read configuration file\n" << std::endl;
    else
        std::cout << "[updater] Error reading config file, reseting to the default state\n" << std::endl;

    // offset updating
    updater::check_and_update();

	// offset reading, updating if needed
    std::cout << "[updater] Reading offsets from file offsets.json." << std::endl;
    if (updater::read())
        std::cout << "[updater] Sucessfully read offsets file\n" << std::endl;
    else
        std::cout << "[updater] Error reading offsets file, reseting to the default state\n" << std::endl;

	// init game (attach and get base addresses)
    g_game.init();

    std::cout << "[overlay] Waiting for focus on cs2 window" << std::endl;
    while (GetForegroundWindow() != g_game.windowHandle) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
		g_game.updateHWND();
        ShowWindow(g_game.windowHandle, TRUE);
    }
	std::cout << "[overlay] Found cs2 window" << std::endl;

	// create overlay window
    WNDCLASSEXA wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = WHITE_BRUSH;
    wc.hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongA(g_game.windowHandle, (-6))); // GWL_HINSTANCE)); 
    wc.lpszMenuName = " ";
    wc.lpszClassName = " ";

    RegisterClassExA(&wc);

	// get the game window bounds
    GetClientRect(g_game.windowHandle, &g_game.game_bounds);

	// create overlay window
    HINSTANCE hInstance = NULL;
    HWND hWnd = CreateWindowExA(WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW, " ", "cs2", WS_POPUP,
        g_game.game_bounds.left, g_game.game_bounds.top, g_game.game_bounds.right - g_game.game_bounds.left, g_game.game_bounds.bottom + g_game.game_bounds.left, NULL, NULL, hInstance, NULL); // NULL, NULL);

    if (hWnd == NULL)
    {
		std::cout << "[overlay] Failed to create window" << std::endl;
		return 0;
    }

	// set window to topmost (above all other windows)
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(hWnd, TRUE);

    // start our hack threads
    std::thread read(read_thread);
    std::thread triggerbot(triggerbot_thread);

	// message loop for the overlay window
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) && !finish)
    {
        if (GetAsyncKeyState(VK_END) & 0x8000) finish = true;

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // cleanup
    read.detach();
	triggerbot.detach();

    std::cout << "[overlay] Destroying overlay window." << std::endl;
    DeleteDC(g::hdcBuffer);
    DeleteObject(g::hbmBuffer);

    DestroyWindow(hWnd);

    g_game.close();

    std::cout << "[overlay] Exited successfully" << std::endl;
    return 0;
}

