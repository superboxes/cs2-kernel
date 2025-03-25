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

		// read, check and assign values
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
		
		if (data.find("esp_box_color_team") != data.end()) {
			esp_box_color_team = {
				data["esp_box_color_team"][0].get<int>(),
				data["esp_box_color_team"][1].get<int>(),
				data["esp_box_color_team"][2].get<int>()
			};
		}

		if (data.find("esp_box_color_enemy") != data.end()) {
			esp_box_color_enemy = {
				data["esp_box_color_enemy"][0].get<int>(),
				data["esp_box_color_enemy"][1].get<int>(),
				data["esp_box_color_enemy"][2].get<int>()
			};
		}

		if (data.find("esp_skeleton_color_team") != data.end()) {
			esp_box_color_team = {
				data["esp_skeleton_color_team"][0].get<int>(),
				data["esp_skeleton_color_team"][1].get<int>(),
				data["esp_skeleton_color_team"][2].get<int>()
			};
		}

		if (data.find("esp_skeleton_color_enemy") != data.end()) {
			esp_box_color_enemy = {
				data["esp_skeleton_color_enemy"][0].get<int>(),
				data["esp_skeleton_color_enemy"][1].get<int>(),
				data["esp_skeleton_color_enemy"][2].get<int>()
			};
		}

		if (data.find("esp_name_color") != data.end()) {
			esp_name_color = {
				data["esp_name_color"][0].get<int>(),
				data["esp_name_color"][1].get<int>(),
				data["esp_name_color"][2].get<int>()
			};
		}

		if (data.find("esp_distance_color") != data.end()) {
			esp_distance_color = {
				data["esp_distance_color"][0].get<int>(),
				data["esp_distance_color"][1].get<int>(),
				data["esp_distance_color"][2].get<int>()
			};
		}

		if (data["triggerbot_enabled"].is_boolean())
			triggerbot_enabled = data["triggerbot_enabled"];
		if (data["triggerbot_delay"].is_number())
			triggerbot_delay = data["triggerbot_delay"];
		if (data["triggerbot_key"].is_number())
			triggerbot_key = data["triggerbot_key"];
		if (data["triggerbot_team_check"].is_boolean())
			triggerbot_team_check = data["triggerbot_team_check"];


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

		data["esp_box_color_team"] = { esp_box_color_team.r, esp_box_color_team.g, esp_box_color_team.b };
		data["esp_box_color_enemy"] = { esp_box_color_enemy.r, esp_box_color_enemy.g, esp_box_color_enemy.b };
		data["esp_skeleton_color_team"] = { esp_box_color_team.r, esp_box_color_team.g, esp_box_color_team.b };
		data["esp_skeleton_color_enemy"] = { esp_box_color_enemy.r, esp_box_color_enemy.g, esp_box_color_enemy.b };
		data["esp_name_color"] = { esp_name_color.r, esp_name_color.g, esp_name_color.b };
		data["esp_distance_color"] = { esp_distance_color.r, esp_distance_color.g, esp_distance_color.b };

		data["triggerbot_enabled"] = triggerbot_enabled;
		data["triggerbot_delay"] = triggerbot_delay;
		data["triggerbot_key"] = triggerbot_key;
		data["triggerbot_team_check"] = triggerbot_team_check;

		// write to file
		std::ofstream output(file_path);
		output << std::setw(4) << data << std::endl;
		output.close();
	}
}
