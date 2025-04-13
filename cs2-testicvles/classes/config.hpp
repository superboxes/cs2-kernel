#pragma once
#include <fstream>
#include "utils.h"
#include "json.hpp"
#include "auto_updater.hpp"
#include <imgui.h>

using json = nlohmann::json;

struct RGBA {
    int r;
    int g;
    int b;
    int a;

	// convert to COLORREF
    operator COLORREF() const {
        return RGB(r, g, b);
    }

	// convert to ImVec4
    ImVec4 toImVec4() const {
        return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }
};

namespace config {
    const std::string file_path = "config.json";

    extern bool read();
    extern void save();

    inline bool team_esp = false;
    inline float render_distance = -1.f;
    inline int flag_render_distance = 200;
    inline bool show_box_esp = true;
    inline bool show_skeleton_esp = false;
    inline bool show_head_tracker = false;
    inline bool show_extra_flags = false;

    inline float esp_box_thickness = 1.0f;
    inline float esp_skeleton_thickness = 1.0f;
    inline float esp_head_tracker_size = 3.0f;
    inline int esp_font_size = 12;

    inline bool show_player_name = true;
    inline bool show_player_distance = true;
    inline bool show_player_health = false;
    inline bool show_player_armor = false;
    inline bool show_player_weapon = false;

    inline RGBA esp_box_color_team = { 75, 175, 75, 255 };
    inline RGBA esp_box_color_enemy = { 225, 75, 75, 255 };
    inline RGBA esp_skeleton_color_team = { 75, 175, 75, 255 };
    inline RGBA esp_skeleton_color_enemy = { 225, 75, 75, 255 };
    inline RGBA esp_head_tracker_color_team = { 75, 175, 75, 255 };
    inline RGBA esp_head_tracker_color_enemy = { 225, 75, 75, 255 };
    inline RGBA esp_name_color = { 75, 75, 175, 255 };
    inline RGBA esp_distance_color = { 75, 75, 175, 255 };
    inline RGBA esp_health_color = { 75, 200, 75, 255 };
    inline RGBA esp_armor_color = { 75, 175, 225, 255 };
    inline RGBA esp_weapon_color = { 200, 200, 75, 255 };

    inline bool triggerbot_enabled = false;
    inline int triggerbot_delay = 10;
    inline int triggerbot_key = VK_XBUTTON1;
    inline bool triggerbot_team_check = true;
    inline bool triggerbot_auto_shoot = true;

    inline int ui_theme = 0;
    inline float ui_scale = 1.0f;
    inline int menu_toggle_key = VK_INSERT;
    inline RGBA ui_accent_color = { 0, 120, 215, 255 };

    inline bool aimbot_enabled = true;
    inline int aimbot_key = VK_XBUTTON2;
    inline bool aimbot_team_check = false;
    inline float aimbot_fov = 10.0f;
    inline float aimbot_smoothing = 0.2f;
}