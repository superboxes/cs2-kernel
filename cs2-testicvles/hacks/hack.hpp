#pragma once
#define _USE_MATH_DEFINES

#include <thread>
#include <cmath>
#include "../mem/reader.hpp"
#include "../classes/config.hpp"
#include "../mem/driver/driver.h"
#include "imgui.h"
#include <math.h>

namespace hack {
	// all the bone connections for the skeleton ESP
	std::vector<std::pair<std::string, std::string>> boneConnections = {
						{"neck_0", "spine_1"},
						{"spine_1", "spine_2"},
						{"spine_2", "pelvis"},
						{"spine_1", "arm_upper_L"},
						{"arm_upper_L", "arm_lower_L"},
						{"arm_lower_L", "hand_L"},
						{"spine_1", "arm_upper_R"},
						{"arm_upper_R", "arm_lower_R"},
						{"arm_lower_R", "hand_R"},
						{"pelvis", "leg_upper_L"},
						{"leg_upper_L", "leg_lower_L"},
						{"leg_lower_L", "ankle_L"},
						{"pelvis", "leg_upper_R"},
						{"leg_upper_R", "leg_lower_R"},
						{"leg_lower_R", "ankle_R"}
	};

	// esp drawing
	void draw() {
		std::lock_guard<std::mutex> lock(reader_mutex);

		ImDrawList* drawList = ImGui::GetBackgroundDrawList();

		if (config::render_distance > 0.f && g_game.localOrigin.calculate_distance(g_game.c4.get_origin()) > config::render_distance)
			return;

		// c4 esp
		if (g_game.isC4Planted)
		{
			Vector3 c4Origin = g_game.c4.get_origin();
			const Vector3 c4ScreenPos = g_game.world_to_screen(&c4Origin);

			if (c4ScreenPos.z >= 0.01f) {
				float c4Distance = g_game.localOrigin.calculate_distance(c4Origin);
				float c4RoundedDistance = std::round(c4Distance / 500.f);

				float height = 10 - c4RoundedDistance;
				float width = height * 1.4f;

				drawList->AddRectFilled(
					ImVec2(c4ScreenPos.x - (width / 2), c4ScreenPos.y - (height / 2)),
					ImVec2(c4ScreenPos.x + (width / 2), c4ScreenPos.y + (height / 2)),
					ImColor(config::esp_box_color_enemy.toImVec4())
				);

				// c4 distance text
				drawList->AddText(
					ImVec2(c4ScreenPos.x + (width / 2 + 5), c4ScreenPos.y),
					ImColor(config::esp_name_color.toImVec4()),
					"C4"
				);
			}
		}

		int playerIndex = 0;
		uintptr_t list_entry;

		// loop through all players
		for (auto player = g_game.players.begin(); player < g_game.players.end(); player++) {
			if (!config::team_esp && g_game.localTeam == player->team)
				continue;

			float distance = g_game.localOrigin.calculate_distance(player->origin);
			if (config::render_distance > 0.f && distance > config::render_distance)
				continue;

			const Vector3 screenPos = g_game.world_to_screen(&player->origin);
			const Vector3 screenHead = g_game.world_to_screen(&player->head);

			if (
				screenPos.z < 0.01f ||
				!utils.is_in_bounds(screenPos, g_game.game_bounds.right, g_game.game_bounds.bottom)
				)
				continue;

			const float height = screenPos.y - screenHead.y;
			const float width = height / 2.4f;

			int roundedDistance = std::round(distance / 10.f);

			ImColor skeletonColor = (g_game.localTeam == player->team) ?
				ImColor(config::esp_skeleton_color_team.toImVec4()) :
				ImColor(config::esp_skeleton_color_enemy.toImVec4());

			ImColor headTrackerColor = (g_game.localTeam == player->team) ?
				ImColor(config::esp_head_tracker_color_team.toImVec4()) :
				ImColor(config::esp_head_tracker_color_enemy.toImVec4());

			ImColor boxColor = (g_game.localTeam == player->team) ?
				ImColor(config::esp_box_color_team.toImVec4()) :
				ImColor(config::esp_box_color_enemy.toImVec4());

			if (config::show_head_tracker) {
				drawList->AddCircle(
					ImVec2(player->bones.bonePositions["head"].x, player->bones.bonePositions["head"].y - width / 12),
					width / 5 * config::esp_head_tracker_size / 3.0f, 
					headTrackerColor,
					0, 
					config::esp_head_tracker_size 
				);
			}

			if (config::show_skeleton_esp) {
				for (const auto& connection : boneConnections) {
					const std::string& boneFrom = connection.first;
					const std::string& boneTo = connection.second;

					drawList->AddLine(
						ImVec2(player->bones.bonePositions[boneFrom].x, player->bones.bonePositions[boneFrom].y),
						ImVec2(player->bones.bonePositions[boneTo].x, player->bones.bonePositions[boneTo].y),
						skeletonColor,
						config::esp_skeleton_thickness
					);
				}
			}

			// box esp
			if (config::show_box_esp)
			{
				drawList->AddRect(
					ImVec2(screenHead.x - width / 2, screenHead.y),
					ImVec2(screenHead.x + width / 2, screenHead.y + height),
					boxColor,
					0.0f,
					0, 
					config::esp_box_thickness 
				);
			}

			if (roundedDistance > config::flag_render_distance)
				continue;

			ImFont* font = ImGui::GetFont();
			float fontScale = config::esp_font_size / 12.0f;

			if (config::show_player_health) {
				// draw health bar
				drawList->AddRectFilled(
					ImVec2(screenHead.x - (width / 2 + 5), screenHead.y + (height * (100 - player->health) / 100)),
					ImVec2(screenHead.x - (width / 2 + 3), screenHead.y + height),
					ImColor(
						(255 - player->health),
						(55 + player->health * 2),
						75,
						255
					)
				);

				// render health text
				drawList->AddText(
					font,
					config::esp_font_size,
					ImVec2(screenHead.x + (width / 2 + 5), screenHead.y + 10),
					ImColor(config::esp_health_color.toImVec4()),
					(std::to_string(player->health) + "hp").c_str()
				);
			}

			if (config::show_player_armor) {
				// draw armor bar
				drawList->AddRectFilled(
					ImVec2(screenHead.x - (width / 2 + 10), screenHead.y + (height * (100 - player->armor) / 100)),
					ImVec2(screenHead.x - (width / 2 + 8), screenHead.y + height),
					ImColor(config::esp_armor_color.toImVec4())
				);

				// render armor text
				drawList->AddText(
					font,
					config::esp_font_size,
					ImVec2(screenHead.x + (width / 2 + 5), screenHead.y + 20),
					ImColor(config::esp_armor_color.toImVec4()),
					(std::to_string(player->armor) + "armor").c_str()
				);
			}

			float textY = screenHead.y + 10;

			// plyaer name
			if (config::show_player_name) {
				drawList->AddText(
					font,
					config::esp_font_size,
					ImVec2(screenHead.x + (width / 2 + 5), textY),
					ImColor(config::esp_name_color.toImVec4()),
					player->name.c_str()
				);
				textY += config::esp_font_size;
			}

			// player weapon
			if (config::show_player_weapon) {
				drawList->AddText(
					font,
					config::esp_font_size,
					ImVec2(screenHead.x + (width / 2 + 5), textY),
					ImColor(config::esp_weapon_color.toImVec4()),
					player->weapon.c_str()
				);
				textY += config::esp_font_size;
			}

			// player distance
			if (config::show_player_distance) {
				drawList->AddText(
					font,
					config::esp_font_size,
					ImVec2(screenHead.x + (width / 2 + 5), textY),
					ImColor(config::esp_distance_color.toImVec4()),
					(std::to_string(roundedDistance) + "m away").c_str()
				);
				textY += config::esp_font_size;
			}

			if (config::show_extra_flags)
			{
				// render money
				drawList->AddText(
					font,
					config::esp_font_size,
					ImVec2(screenHead.x + (width / 2 + 5), textY),
					ImColor(0, 125, 0, 255),
					("$" + std::to_string(player->money)).c_str()
				);
				textY += config::esp_font_size;

				// render flash status
				if (player->flashAlpha > 100)
				{
					drawList->AddText(
						font,
						config::esp_font_size,
						ImVec2(screenHead.x + (width / 2 + 5), textY),
						ImColor(config::esp_distance_color.toImVec4()),
						"Player is flashed"
					);
					textY += config::esp_font_size;
				}

				// render defusing status
				if (player->is_defusing)
				{
					drawList->AddText(
						font,
						config::esp_font_size,
						ImVec2(screenHead.x + (width / 2 + 5), textY),
						ImColor(config::esp_distance_color.toImVec4()),
						"Player is defusing"
					);
				}
			}
		}
	}

	// triggerbot
	void triggerbot()
	{
		if (!config::triggerbot_enabled)
			return;

		// only trigger when the key is pressed (if configured) and auto shoot is disabled
		if (config::triggerbot_key != 0 && !(GetAsyncKeyState(config::triggerbot_key) & 0x8000) && !config::triggerbot_auto_shoot)
			return;

		// double threading shenanigans
		std::lock_guard<std::mutex> lock(reader_mutex);

		// check if in game
		if (!g_game.inGame)
			return;

		// friendly fire check, 
		// check if entity is on team 0 (no team) (easiest way to check if its a player alive and well, 
		// dodging the health check and all that)
		if ((g_game.aim_entity_team == g_game.localTeam && config::triggerbot_team_check) || g_game.aim_entity_team == 0)
			return;

		if (config::triggerbot_delay > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(config::triggerbot_delay));

		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}

	void aimbot() {
		if (!config::aimbot_enabled)
			return;

		if (config::aimbot_key != 0 && !(GetAsyncKeyState(config::aimbot_key) & 0x8000))
			return;

		std::lock_guard<std::mutex> lock(reader_mutex);

		Vector3 closestEnemy = Vector3(0, 0, 0);
		boolean first = true;

		for (auto player = g_game.players.begin(); player < g_game.players.end(); player++) {
			Vector3 playerPosition = player->origin;
			Vector3 playerHead = player->head;
			int health = player->health;

			if (first || (health > 0 && health <= 100) && playerPosition.magnitude(g_game.localPlayerPosition) < closestEnemy.magnitude(g_game.localPlayerPosition)) {
				if (player->team == g_game.localTeam && config::aimbot_team_check) {
					continue;
				}

				float pitch = g_game.localPlayerPosition.pitchAngle(playerPosition);
				float yaw = g_game.localPlayerPosition.yawAngle(playerPosition);

				Vector3 newAngles = Vector3(pitch, yaw, 0.0f);
				Vector3 delta = g_game.viewAngles.delta(newAngles);
				if (delta.normalize() > config::aimbot_fov) continue;

				closestEnemy = playerPosition;
				first = false;
			}
		}

		if (closestEnemy.IsZero()) {
			return;
		}

		float pitch = g_game.localPlayerPosition.pitchAngle(closestEnemy);
		float yaw = g_game.localPlayerPosition.yawAngle(closestEnemy);

		Vector3 newAngles = Vector3(pitch, yaw, 0.0f);
		Vector3 delta = g_game.viewAngles.delta(newAngles);

		Vector3 smoothAngles = g_game.viewAngles + delta * config::aimbot_smoothing;

		g_game.setViewAngle(smoothAngles);
	}
}