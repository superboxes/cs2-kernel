#include "config.hpp"

namespace config {
    bool read() {
        // check if file exists, if not, save default values
        if (!updater::file_good(file_path)) {
            save();
            return false;
        }

        // read file
        std::ifstream f(file_path);

        json data;
        try {
            data = json::parse(f);
        }
        catch (const std::exception& e) {
            save();
        }

        if (data.empty())
            return false;

        if (data["show_box_esp"].is_boolean())
            show_box_esp = data["show_box_esp"];
        if (data["show_skeleton_esp"].is_boolean())
            show_skeleton_esp = data["show_skeleton_esp"];
        if (data["show_head_tracker"].is_boolean())
            show_head_tracker = data["show_head_tracker"];
        if (data["team_esp"].is_boolean())
            team_esp = data["team_esp"];
        if (data["render_distance"].is_number())
            render_distance = data["render_distance"];
        if (data["flag_render_distance"].is_number())
            flag_render_distance = data["flag_render_distance"];
        if (data["show_extra_flags"].is_boolean())
            show_extra_flags = data["show_extra_flags"];

        if (data["esp_box_thickness"].is_number())
            esp_box_thickness = data["esp_box_thickness"];
        if (data["esp_skeleton_thickness"].is_number())
            esp_skeleton_thickness = data["esp_skeleton_thickness"];
        if (data["esp_head_tracker_size"].is_number())
            esp_head_tracker_size = data["esp_head_tracker_size"];
        if (data["esp_font_size"].is_number())
            esp_font_size = data["esp_font_size"];

        if (data["show_player_name"].is_boolean())
            show_player_name = data["show_player_name"];
        if (data["show_player_distance"].is_boolean())
            show_player_distance = data["show_player_distance"];
        if (data["show_player_health"].is_boolean())
            show_player_health = data["show_player_health"];
        if (data["show_player_armor"].is_boolean())
            show_player_armor = data["show_player_armor"];
        if (data["show_player_weapon"].is_boolean())
            show_player_weapon = data["show_player_weapon"];

		if (data["aimbot_enabled"].is_boolean())
			aimbot_enabled = data["aimbot_enabled"];
		if (data["aimbot_key"].is_number())
			aimbot_key = data["aimbot_key"];
		if (data["aimbot_team_check"].is_boolean())
			aimbot_team_check = data["aimbot_team_check"];
		if (data["aimbot_fov"].is_number())
			aimbot_fov = data["aimbot_fov"];
		if (data["aimbot_smoothing"].is_number())
			aimbot_smoothing = data["aimbot_smoothing"];

        auto loadRGBA = [&data](const std::string& key, RGBA& color) {
            if (data.find(key) != data.end() && data[key].is_array() && data[key].size() >= 4) {
                color = {
                    data[key][0].get<int>(),
                    data[key][1].get<int>(),
                    data[key][2].get<int>(),
                    data[key][3].get<int>()
                };
            }
            };

        loadRGBA("esp_box_color_team", esp_box_color_team);
        loadRGBA("esp_box_color_enemy", esp_box_color_enemy);
        loadRGBA("esp_skeleton_color_team", esp_skeleton_color_team);
        loadRGBA("esp_skeleton_color_enemy", esp_skeleton_color_enemy);
        loadRGBA("esp_head_tracker_color_team", esp_head_tracker_color_team);
        loadRGBA("esp_head_tracker_color_enemy", esp_head_tracker_color_enemy);
        loadRGBA("esp_name_color", esp_name_color);
        loadRGBA("esp_distance_color", esp_distance_color);
        loadRGBA("esp_health_color", esp_health_color);
        loadRGBA("esp_armor_color", esp_armor_color);
        loadRGBA("esp_weapon_color", esp_weapon_color);

        if (data["triggerbot_enabled"].is_boolean())
            triggerbot_enabled = data["triggerbot_enabled"];
        if (data["triggerbot_delay"].is_number())
            triggerbot_delay = data["triggerbot_delay"];
        if (data["triggerbot_key"].is_number())
            triggerbot_key = data["triggerbot_key"];
        if (data["triggerbot_team_check"].is_boolean())
            triggerbot_team_check = data["triggerbot_team_check"];
        if (data["triggerbot_auto_shoot"].is_boolean())
            triggerbot_auto_shoot = data["triggerbot_auto_shoot"];

        if (data["ui_theme"].is_number())
            ui_theme = data["ui_theme"];
        if (data["ui_scale"].is_number())
            ui_scale = data["ui_scale"];
        if (data["menu_toggle_key"].is_number())
            menu_toggle_key = data["menu_toggle_key"];
        loadRGBA("ui_accent_color", ui_accent_color);

        return true;
    }

    void save() {
        json data;

        data["show_box_esp"] = show_box_esp;
        data["show_skeleton_esp"] = show_skeleton_esp;
        data["show_head_tracker"] = show_head_tracker;
        data["team_esp"] = team_esp;
        data["render_distance"] = render_distance;
        data["flag_render_distance"] = flag_render_distance;
        data["show_extra_flags"] = show_extra_flags;

        data["esp_box_thickness"] = esp_box_thickness;
        data["esp_skeleton_thickness"] = esp_skeleton_thickness;
        data["esp_head_tracker_size"] = esp_head_tracker_size;
        data["esp_font_size"] = esp_font_size;

        data["show_player_name"] = show_player_name;
        data["show_player_distance"] = show_player_distance;
        data["show_player_health"] = show_player_health;
        data["show_player_armor"] = show_player_armor;
        data["show_player_weapon"] = show_player_weapon;

        data["esp_box_color_team"] = { esp_box_color_team.r, esp_box_color_team.g, esp_box_color_team.b, esp_box_color_team.a };
        data["esp_box_color_enemy"] = { esp_box_color_enemy.r, esp_box_color_enemy.g, esp_box_color_enemy.b, esp_box_color_enemy.a };
        data["esp_skeleton_color_team"] = { esp_skeleton_color_team.r, esp_skeleton_color_team.g, esp_skeleton_color_team.b, esp_skeleton_color_team.a };
        data["esp_skeleton_color_enemy"] = { esp_skeleton_color_enemy.r, esp_skeleton_color_enemy.g, esp_skeleton_color_enemy.b, esp_skeleton_color_enemy.a };
        data["esp_head_tracker_color_team"] = { esp_head_tracker_color_team.r, esp_head_tracker_color_team.g, esp_head_tracker_color_team.b, esp_head_tracker_color_team.a };
        data["esp_head_tracker_color_enemy"] = { esp_head_tracker_color_enemy.r, esp_head_tracker_color_enemy.g, esp_head_tracker_color_enemy.b, esp_head_tracker_color_enemy.a };
        data["esp_name_color"] = { esp_name_color.r, esp_name_color.g, esp_name_color.b, esp_name_color.a };
        data["esp_distance_color"] = { esp_distance_color.r, esp_distance_color.g, esp_distance_color.b, esp_distance_color.a };
        data["esp_health_color"] = { esp_health_color.r, esp_health_color.g, esp_health_color.b, esp_health_color.a };
        data["esp_armor_color"] = { esp_armor_color.r, esp_armor_color.g, esp_armor_color.b, esp_armor_color.a };
        data["esp_weapon_color"] = { esp_weapon_color.r, esp_weapon_color.g, esp_weapon_color.b, esp_weapon_color.a };

        data["triggerbot_enabled"] = triggerbot_enabled;
        data["triggerbot_delay"] = triggerbot_delay;
        data["triggerbot_key"] = triggerbot_key;
        data["triggerbot_team_check"] = triggerbot_team_check;
        data["triggerbot_auto_shoot"] = triggerbot_auto_shoot;

		data["aimbot_enabled"] = aimbot_enabled;
		data["aimbot_key"] = aimbot_key;
		data["aimbot_team_check"] = aimbot_team_check;
		data["aimbot_fov"] = aimbot_fov;
		data["aimbot_smoothing"] = aimbot_smoothing;

        data["ui_theme"] = ui_theme;
        data["ui_scale"] = ui_scale;
        data["menu_toggle_key"] = menu_toggle_key;
        data["ui_accent_color"] = { ui_accent_color.r, ui_accent_color.g, ui_accent_color.b, ui_accent_color.a };

        // write to file
        std::ofstream output(file_path);
        output << std::setw(4) << data << std::endl;
        output.close();
    }
}