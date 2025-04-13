#pragma once

#include <imgui.h>
#include "../classes/config.hpp"

namespace ui {
    void render();

    void render_esp_tab();
    void render_triggerbot_tab();
    void render_settings_tab();
    void initialize();

    void process_input();

    enum class MenuTab {
        ESP,
        Triggerbot,
        Aimbot,
        Settings
    };

    extern bool menu_visible;
    extern MenuTab current_tab;
}
