#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>

#include "classes/config.hpp"
#include "mem/reader.hpp"
#include "hacks/hack.hpp"
#include "render/overlay.hpp"
#include "render/ui.hpp"

bool finish = false;
Overlay g_overlay;

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

// imgui draw function
void draw() {
    if (GetForegroundWindow() == g_game.windowHandle) {
        // esp drawing
        hack::draw();
    }

    // menu rendering
    ui::render();
}

int main() {
    std::cout << "[general] Starting up cheat..." << std::endl;

    // config reading
    std::cout << "[config] Reading configuration." << std::endl;
    if (config::read())
        std::cout << "[updater] Successfully read configuration file\n" << std::endl;
    else
        std::cout << "[updater] Error reading config file, resetting to the default state\n" << std::endl;

    // offset updating
    updater::check_and_update();

    // offset reading, updating if needed
    std::cout << "[updater] Reading offsets from file offsets.json." << std::endl;
    if (updater::read())
        std::cout << "[updater] Successfully read offsets file\n" << std::endl;
    else
        std::cout << "[updater] Error reading offsets file, resetting to the default state\n" << std::endl;

    // init game (attach and get base addresses)
    g_game.init();

    std::cout << "[overlay] Waiting for focus on cs2 window" << std::endl;
    while (GetForegroundWindow() != g_game.windowHandle) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        g_game.updateHWND();
        ShowWindow(g_game.windowHandle, TRUE);
    }
    std::cout << "[overlay] Found cs2 window" << std::endl;

    // init overlay
    if (!g_overlay.Initialize(g_game.windowHandle)) {
        std::cout << "[overlay] Failed to initialize overlay" << std::endl;
        return 1;
    }

    // init ui
	ui::initialize();

	// start hack theads
    std::thread read(read_thread);
    std::thread triggerbot(triggerbot_thread);

    // main loop
    while (!finish) {
        if (!g_overlay.ProcessMessages()) {
            finish = true;
            break;
        }

        if (GetAsyncKeyState(VK_END) & 0x8000)
            finish = true;

        ui::process_input();

		// render our func
        g_overlay.Render(draw);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

	// wait for threads to finish
    if (read.joinable()) read.join();
    else read.detach();

    if (triggerbot.joinable()) triggerbot.join();
    else triggerbot.detach();

	// overlay cleanup
    g_overlay.Shutdown();

    g_game.close();

    std::cout << "[overlay] Exited successfully" << std::endl;
    return 0;
}