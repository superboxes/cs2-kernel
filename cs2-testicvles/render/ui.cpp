#define NOMINMAX
#include "ui.hpp"
#include <iostream>
#include <windows.h>

namespace ui {
    bool menu_visible = false;
    MenuTab current_tab = MenuTab::ESP;
    float fps = 0.0f;

    int* currently_binding = nullptr;
    bool waiting_for_key = false;

    bool color_picker_open = false;
    RGBA* selected_color = nullptr;
    std::string color_picker_label;

    std::map<int, std::string> key_names = {
        { VK_LBUTTON, "LMB" },
        { VK_RBUTTON, "RMB" },
        { VK_MBUTTON, "MMB" },
        { VK_XBUTTON1, "Mouse4" },
        { VK_XBUTTON2, "Mouse5" },
        { VK_BACK, "Backspace" },
        { VK_TAB, "Tab" },
        { VK_RETURN, "Enter" },
        { VK_SHIFT, "Shift" },
        { VK_CONTROL, "Ctrl" },
        { VK_MENU, "Alt" },
        { VK_CAPITAL, "Caps Lock" },
        { VK_ESCAPE, "Esc" },
        { VK_SPACE, "Space" },
        { VK_INSERT, "Insert" },
        { VK_DELETE, "Delete" },
        { VK_HOME, "Home" },
        { VK_END, "End" },
        { VK_F1, "F1" },
        { VK_F2, "F2" },
        { VK_F3, "F3" },
        { VK_F4, "F4" },
        { VK_F5, "F5" },
        { VK_F6, "F6" },
        { VK_F7, "F7" },
        { VK_F8, "F8" },
        { VK_F9, "F9" },
        { VK_F10, "F10" },
        { VK_F11, "F11" },
        { VK_F12, "F12" }
    };

    void apply_theme(int theme_index) {
        ImGuiStyle& style = ImGui::GetStyle();

        switch (theme_index) {
        case 0:
            ImGui::StyleColorsDark();
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.3f, 0.6f, 1.0f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.4f, 0.8f, 1.0f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.4f, 0.8f, 0.8f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.5f, 0.9f, 1.0f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.3f, 0.7f, 1.0f);
            style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.4f, 0.8f, 0.6f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.5f, 0.9f, 0.7f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.1f, 0.3f, 0.7f, 0.8f);
            break;
        case 1:
            ImGui::StyleColorsLight();
            break;
        case 2:
            ImGui::StyleColorsClassic();
            break;
        case 3:
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.85f, 0.18f, 0.22f, 1.00f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.98f, 0.27f, 0.32f, 1.00f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.85f, 0.18f, 0.22f, 0.80f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.98f, 0.27f, 0.32f, 1.00f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.75f, 0.10f, 0.15f, 1.00f);
            style.Colors[ImGuiCol_Header] = ImVec4(0.85f, 0.18f, 0.22f, 0.60f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.98f, 0.27f, 0.32f, 0.70f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.75f, 0.10f, 0.15f, 0.80f);
            break;
        case 4:
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.70f, 0.80f, 0.80f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.80f, 0.95f, 1.00f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.80f, 0.55f, 0.60f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.90f, 0.70f, 0.80f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.70f, 0.50f, 1.00f);
            style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 0.80f, 0.55f, 0.40f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.90f, 0.70f, 0.60f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.70f, 0.50f, 0.80f);
            style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.90f, 0.90f, 1.00f);
            break;
        case 5:
            ImVec4 accentColor = config::ui_accent_color.toImVec4();
            ImVec4 accentColorDarker = ImVec4(accentColor.x * 0.7f, accentColor.y * 0.7f, accentColor.z * 0.7f, accentColor.w);
            ImVec4 accentColorBrighter = ImVec4(
                std::min(accentColor.x * 1.3f, 1.0f),
                std::min(accentColor.y * 1.3f, 1.0f),
                std::min(accentColor.z * 1.3f, 1.0f),
                accentColor.w
            );

            style.Colors[ImGuiCol_TitleBg] = accentColorDarker;
            style.Colors[ImGuiCol_TitleBgActive] = accentColor;
            style.Colors[ImGuiCol_Button] = accentColor;
            style.Colors[ImGuiCol_ButtonHovered] = accentColorBrighter;
            style.Colors[ImGuiCol_ButtonActive] = accentColorDarker;
            style.Colors[ImGuiCol_Header] = accentColor;
            style.Colors[ImGuiCol_HeaderHovered] = accentColorBrighter;
            style.Colors[ImGuiCol_HeaderActive] = accentColorDarker;
            break;
        }
    }

    void initialize() {
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();

        style.Alpha = 0.9f;
        style.WindowRounding = 5.0f;
        style.FrameRounding = 4.0f;

        style.ScaleAllSizes(config::ui_scale);

        apply_theme(config::ui_theme);

        std::cout << "[ui] UI initialized" << std::endl;
    }

    void process_input() {
        if (GetAsyncKeyState(config::menu_toggle_key) & 1) {
            menu_visible = !menu_visible;
        }

        if (waiting_for_key && currently_binding) {
            for (int i = 1; i < 256; i++) {
                if (GetAsyncKeyState(i) & 1) {
                    *currently_binding = i;
                    currently_binding = nullptr;
                    waiting_for_key = false;
                    break;
                }
            }
        }
    }

    void show_key_bind_button(const char* label, int* key_var) {
        std::string key_name = "[" + (key_names.count(*key_var) ? key_names[*key_var] : std::to_string(*key_var)) + "]";

        if (waiting_for_key && currently_binding == key_var) {
            key_name = "[Press Any Key]";
        }

        if (ImGui::Button((key_name + " " + label).c_str())) {
            if (!waiting_for_key) {
                waiting_for_key = true;
                currently_binding = key_var;
            }
        }
    }

    void show_color_picker(const char* label, RGBA* color) {
        ImVec4 col = ImVec4(color->r / 255.0f, color->g / 255.0f, color->b / 255.0f, color->a / 255.0f);

        ImGui::ColorButton(label, col, ImGuiColorEditFlags_AlphaPreview);
        ImGui::SameLine();
        if (ImGui::Button(label)) {
            color_picker_open = true;
            selected_color = color;
            color_picker_label = label;
        }
    }

    void render_esp_tab() {
        if (ImGui::CollapsingHeader("ESP Core Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Show Box ESP", &config::show_box_esp);
            ImGui::Checkbox("Show Skeleton ESP", &config::show_skeleton_esp);
            ImGui::Checkbox("Show Head Tracker", &config::show_head_tracker);
            ImGui::Checkbox("Show Team ESP", &config::team_esp);
            ImGui::Checkbox("Show Extra Flags", &config::show_extra_flags);

            ImGui::SliderFloat("Render Distance", &config::render_distance, -1.0f, 2000.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::Text("(-1 = unlimited)");
            ImGui::SliderInt("Flag Render Distance", &config::flag_render_distance, 0, 1000, "%d", ImGuiSliderFlags_AlwaysClamp);
        }

        if (ImGui::CollapsingHeader("ESP Elements", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Show Player Name", &config::show_player_name);
            ImGui::Checkbox("Show Player Distance", &config::show_player_distance);
            ImGui::Checkbox("Show Player Health", &config::show_player_health);
            ImGui::Checkbox("Show Player Armor", &config::show_player_armor);
            ImGui::Checkbox("Show Player Weapon", &config::show_player_weapon);
        }

        if (ImGui::CollapsingHeader("ESP Visual Style", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat("Box Thickness", &config::esp_box_thickness, 0.5f, 5.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Skeleton Thickness", &config::esp_skeleton_thickness, 0.5f, 5.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Head Tracker Size", &config::esp_head_tracker_size, 1.0f, 10.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderInt("Font Size", &config::esp_font_size, 8, 24, "%d", ImGuiSliderFlags_AlwaysClamp);
        }

        if (ImGui::CollapsingHeader("ESP Colors", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Box Colors");
            show_color_picker("Team Box Color", &config::esp_box_color_team);
            show_color_picker("Enemy Box Color", &config::esp_box_color_enemy);

            ImGui::Text("Skeleton Colors");
            show_color_picker("Team Skeleton Color", &config::esp_skeleton_color_team);
            show_color_picker("Enemy Skeleton Color", &config::esp_skeleton_color_enemy);

            ImGui::Text("Head Tracker Colors");
            show_color_picker("Team Head Tracker", &config::esp_head_tracker_color_team);
            show_color_picker("Enemy Head Tracker", &config::esp_head_tracker_color_enemy);

            ImGui::Text("Info Colors");
            show_color_picker("Name Color", &config::esp_name_color);
            show_color_picker("Distance Color", &config::esp_distance_color);
            show_color_picker("Health Color", &config::esp_health_color);
            show_color_picker("Armor Color", &config::esp_armor_color);
            show_color_picker("Weapon Color", &config::esp_weapon_color);
        }
    }

    void render_triggerbot_tab() {
        ImGui::Checkbox("Enable Triggerbot", &config::triggerbot_enabled);

        show_key_bind_button("Triggerbot Key", &config::triggerbot_key);

        ImGui::Checkbox("Check Team (don't shoot teammates)", &config::triggerbot_team_check);
        ImGui::SliderInt("Delay (ms)", &config::triggerbot_delay, 0, 500);
        ImGui::Checkbox("Auto-shoot (no key required)", &config::triggerbot_auto_shoot);
    }

    void render_settings_tab() {
        if (ImGui::Button("Save Config")) {
            config::save();
            ImGui::OpenPopup("Config Saved");
        }
        ImGui::SameLine();

        if (ImGui::Button("Load Config")) {
            config::read();
            ImGui::OpenPopup("Config Loaded");
        }

        if (ImGui::BeginPopupModal("Config Saved", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Configuration has been saved successfully.");
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Config Loaded", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Configuration has been loaded successfully.");
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();
        ImGui::Text("General Settings");

        static const char* themes[] = { "Dark", "Light", "Classic", "Cherry" };
        static int theme_index = 0;
        if (ImGui::Combo("Theme", &theme_index, themes, IM_ARRAYSIZE(themes))) {
            // theme selection
            switch (theme_index) {
            case 0: ImGui::StyleColorsDark(); break;
            case 1: ImGui::StyleColorsLight(); break;
            case 2: ImGui::StyleColorsClassic(); break;
            case 3:
                // custom theme
                ImGuiStyle & style = ImGui::GetStyle();
                style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
                style.Colors[ImGuiCol_TitleBg] = ImVec4(0.85f, 0.18f, 0.22f, 1.00f);
                style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.98f, 0.27f, 0.32f, 1.00f);
                style.Colors[ImGuiCol_Button] = ImVec4(0.85f, 0.18f, 0.22f, 0.80f);
                break;
            }
        }

        ImGui::Text("Menu Toggle Key: %s", key_names[config::menu_toggle_key].c_str());

        ImGui::Separator();
        ImGui::Text("About");
        ImGui::Text("creator: kestas69");
        ImGui::Text("Press END to exit application");
    }

	void render_aimbot_tab() {
		ImGui::Checkbox("Enable Aimbot", &config::aimbot_enabled);
		ImGui::Checkbox("Aimbot Team Check", &config::aimbot_team_check);
		ImGui::SliderFloat("Aimbot FOV", &config::aimbot_fov, 0.0f, 15.0f);
		ImGui::SliderFloat("Aimbot Smoothness", &config::aimbot_smoothing, 0.2f, 1);
		
        show_key_bind_button("Aimbot Key", &config::aimbot_key);
	}

    void render() {
        if (menu_visible) {
            ImGui::Begin("CS2 Cheat", NULL, ImGuiWindowFlags_AlwaysAutoResize);

            if (ImGui::BeginTabBar("TabBar")) {
                if (ImGui::BeginTabItem("ESP")) {
                    current_tab = MenuTab::ESP;
                    render_esp_tab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Triggerbot")) {
                    current_tab = MenuTab::Triggerbot;
                    render_triggerbot_tab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Settings")) {
                    current_tab = MenuTab::Settings;
                    render_settings_tab();
                    ImGui::EndTabItem();
                }

				if (ImGui::BeginTabItem("Aimbot")) {
					current_tab = MenuTab::Aimbot;
					render_aimbot_tab();
					ImGui::EndTabItem();
				}

                ImGui::EndTabBar();
            }

            ImGui::End();
        }
    }
}