#include "reader.hpp"
#include <thread>
#include <map>
#include <cmath>
#include "../classes/auto_updater.hpp"
#include "../classes/config.hpp"
#include "driver/driver.h"

std::map<std::string, int> boneMap = {
	{"head", 6},
	{"neck_0", 5},
	{"spine_1", 4},
	{"spine_2", 2},
	{"pelvis", 0},
	{"arm_upper_L", 8},
	{"arm_lower_L", 9},
	{"hand_L", 10},
	{"arm_upper_R", 13},
	{"arm_lower_R", 14},
	{"hand_R", 15},
	{"leg_upper_L", 22},
	{"leg_lower_L", 23},
	{"ankle_L", 24},
	{"leg_upper_R", 25},
	{"leg_lower_R", 26},
	{"ankle_R", 27}
};

// CC4
uintptr_t CC4::get_planted() {
	return Driver::rpm<uintptr_t>(Driver::rpm<uintptr_t>(g_game.base_client + updater::offsets::dwPlantedC4));
}

uintptr_t CC4::get_node() {
	return Driver::rpm<uintptr_t>(get_planted() + updater::offsets::m_pGameSceneNode);
}

Vector3 CC4::get_origin() {
	return Driver::rpm<Vector3>(get_node() + updater::offsets::m_vecAbsOrigin);
}

// define if you want to download the driver and mapper when the driver is not loaded
/*
#define _DRIVER_LINK ""
#define _MAPPER_LINK ""
*/

// CGame
void CGame::init() {
	// up n coming manual driver loader maybe
	if (!Driver::is_driver_loaded())
	{
		std::cout << "[driver] Driver not loaded" << std::endl;

	#if defined(_DRIVER_LINK) && defined(_MAPPER_LINK)
		// download driver
		if (!updater::file_good("main.sys"))
		{
			char response;
			std::cout << "[driver] Do you want to download the driver? (y/n): ";
			std::cin >> response;

			if (std::tolower(response) == 'y')
			{
				std::cout << "[driver] Downloading driver..." << std::endl;

				if (!updater::download_file(_DRIVER_LINK, "main.sys")) // add driver link
				{
					std::cout << "[driver] Failed to download driver" << std::endl;
					throw std::runtime_error("[driver] driver not loaded");
				}

				std::cout << "[driver] Downloaded driver" << std::endl;
			}
			else
			{
				std::cout << "[driver] Driver not loaded" << std::endl;
				throw std::runtime_error("[driver] driver not loaded");
			}
		}

		// download mmapper
		if (!updater::file_good("kdmapper.exe"))
		{
			char response;
			std::cout << "[driver] Do you want to download the mapper? (y/n): ";
			std::cin >> response;

			if (std::tolower(response) == 'y')
			{
				std::cout << "[driver] Downloading mapper..." << std::endl;

				if (!updater::download_file(_MAPPER_LINK, "kdmapper.exe")) // add mapper link
				{
					std::cout << "[driver] Failed to download mapper" << std::endl;
					throw std::runtime_error("[driver] driver not loaded");
				}

				std::cout << "[driver] Downloaded mapper" << std::endl;
			}
			else
			{
				std::cout << "[driver] Driver not loaded" << std::endl;
				throw std::runtime_error("[driver] driver not loaded");
			}
		}
	#endif

		std::cout << "[driver] Please load driver using the mapper" << std::endl;


		std::chrono::seconds duration(5);
		throw std::runtime_error("[driver] driver not loaded");
	}

	std::cout << "[cs2] Waiting for cs2.exe..." << std::endl;
	g::processID = Driver::get_process_id("cs2.exe");
	while (!g::processID)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		g::processID = Driver::get_process_id("cs2.exe");
	}

	std::cout << "[cs2] Attached to cs2.exe with PID: " << (uint32_t)(uintptr_t)g::processID << std::endl;
	std::cout << "[cs2] Attempting to get client.dll and engine2.dll" << std::endl;

	do {
		base_client = Driver::get_client(g::processID);
		base_engine = Driver::get_engine(g::processID);

		if (base_engine == 0 || base_client == 0) 
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	} while (base_client == 0 || base_engine == 0);

	std::cout << "[cs2] Successfully found modules:" << std::endl;
	std::cout << "  client.dll: 0x" << std::hex << base_client << std::dec << std::endl;
	std::cout << "  engine2.dll: 0x" << std::hex << base_engine << std::dec << std::endl;

	try {
		base_client_size = Driver::get_client_size(g::processID);
		base_engine_size = Driver::get_engine_size(g::processID);
		std::cout << "[cs2] Module sizes - client: " << base_client_size
			<< ", engine: " << base_engine_size << std::endl;
	}
	catch (const std::exception& e) {
		std::cout << "[cs2] Warning: Failed to get module sizes: " << e.what() << std::endl;
	}

	try {
		buildNumber = Driver::rpm<uintptr_t>(base_engine + updater::offsets::dwBuildNumber);
		std::cout << "[cs2] Build number: " << buildNumber << std::endl;
	}
	catch (const std::exception& e) {
		std::cout << "[cs2] Warning: Failed to get build number: " << e.what() << std::endl;
	}

	std::cout << "[cs2] Initialization complete" << std::endl;
}

void CGame::close() {
	std::cout << "[cs2] Detaching from process" << std::endl;
	g::processID = NULL;
}

void CGame::loop() {
	std::lock_guard<std::mutex> lock(reader_mutex);

	if (updater::offsets::dwLocalPlayerController == 0x0 || updater::offsets::m_iIDEntIndex == 0x0)
		throw std::runtime_error("Offsets have not been correctly set, cannot proceed.");

	inGame = false;
	isC4Planted = false;
	aim_entity_team = 0;

	localPlayer = Driver::rpm<uintptr_t>(base_client + updater::offsets::dwLocalPlayerController);
	if (!localPlayer) return;

	localPlayerPawn = Driver::rpm<std::uint32_t>(localPlayer + updater::offsets::m_hPlayerPawn);
	if (!localPlayerPawn) return;

	entity_list = Driver::rpm<uintptr_t>(base_client + updater::offsets::dwEntityList);

	localList_entry2 = Driver::rpm<uintptr_t>(entity_list + 0x8 * ((localPlayerPawn & 0x7FFF) >> 9) + 16);
	localpCSPlayerPawn = Driver::rpm<uintptr_t>(localList_entry2 + 120 * (localPlayerPawn & 0x1FF));
	if (!localpCSPlayerPawn) return;

	view_matrix = Driver::rpm<view_matrix_t>(base_client + updater::offsets::dwViewMatrix);

	localTeam = Driver::rpm<int>(localPlayer + updater::offsets::m_iTeamNum);
	localOrigin = Driver::rpm<Vector3>(localpCSPlayerPawn + updater::offsets::m_vOldOrigin);
	isC4Planted = Driver::rpm<bool>(base_client + updater::offsets::dwPlantedC4 - 0x8);

	if (config::triggerbot_enabled) 
	{
		crosshairEntityId = Driver::rpm<int>(localpCSPlayerPawn + updater::offsets::m_iIDEntIndex);
		if (crosshairEntityId > 0)
		{
			en_list_entry = Driver::rpm<uintptr_t>(g_game.entity_list + 0x8 * ((crosshairEntityId & 0x7FFF) >> 9) + 16);
			if (en_list_entry)
			{
				aimedEntity = Driver::rpm<uintptr_t>(en_list_entry + 120 * (crosshairEntityId & 0x1FF));;

				if (aimedEntity)
				{
					aim_entity_team = Driver::rpm<int>(aimedEntity + updater::offsets::m_iTeamNum);
				}
			}
		}
	}

	inGame = true;
	int playerIndex = 0;
	std::vector<CPlayer> list;
	CPlayer player;
	uintptr_t list_entry, list_entry2, playerPawn, playerMoneyServices, clippingWeapon, weaponData, playerNameData;

	while (true) {
		playerIndex++;
		list_entry = Driver::rpm<uintptr_t>(entity_list + (8 * (playerIndex & 0x7FFF) >> 9) + 16);
		if (!list_entry) break;

		player.entity = Driver::rpm<uintptr_t>(list_entry + 120 * (playerIndex & 0x1FF));
		if (!player.entity) continue;

		/**
		* Skip rendering your own character and teammates
		*
		* If you really want you can exclude your own character from the check but
		* since you are in the same team as yourself it will be excluded anyway
		**/
		player.team = Driver::rpm<int>(player.entity + updater::offsets::m_iTeamNum);
		if (config::team_esp && (player.team == localTeam)) continue;

		playerPawn = Driver::rpm<std::uint32_t>(player.entity + updater::offsets::m_hPlayerPawn);

		list_entry2 = Driver::rpm<uintptr_t>(entity_list + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
		if (!list_entry2) continue;

		player.pCSPlayerPawn = Driver::rpm<uintptr_t>(list_entry2 + 120 * (playerPawn & 0x1FF));
		if (!player.pCSPlayerPawn) continue;

		player.health = Driver::rpm<int>(player.pCSPlayerPawn + updater::offsets::m_iHealth);
		player.armor = Driver::rpm<int>(player.pCSPlayerPawn + updater::offsets::m_ArmorValue);
		if (player.health <= 0 || player.health > 100) continue;

		if (config::team_esp && (player.pCSPlayerPawn == localPlayer)) continue;

		/*
		* Unused for now, but for a vis check
		*
		* player.spottedState = Driver::rpm<uintptr_t>(player.pCSPlayerPawn + 0x1630);
		* player.is_spotted = Driver::rpm<DWORD_PTR>(player.spottedState + 0xC); // bSpottedByMask
		* player.is_spotted = Driver::rpm<bool>(player.spottedState + 0x8); // bSpotted
		*/

		playerNameData = Driver::rpm<uintptr_t>(player.entity + updater::offsets::m_sSanitizedPlayerName);
		char buffer[256];
		Driver::rpm_pattern<char>(playerNameData, buffer, sizeof(buffer));
		std::string name = std::string(buffer);
		player.name = name;

		player.gameSceneNode = Driver::rpm<uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_pGameSceneNode);
		player.origin = Driver::rpm<Vector3>(player.pCSPlayerPawn + updater::offsets::m_vOldOrigin);
		player.head = { player.origin.x, player.origin.y, player.origin.z + 75.f };

		if (player.origin.x == localOrigin.x && player.origin.y == localOrigin.y && player.origin.z == localOrigin.z)
			continue;

		if (config::render_distance != -1 && (localOrigin - player.origin).length2d() > config::render_distance) continue;
		if (player.origin.x == 0 && player.origin.y == 0) continue;

		if (config::show_skeleton_esp) {
			player.gameSceneNode = Driver::rpm<uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_pGameSceneNode);
			player.boneArray = Driver::rpm<uintptr_t>(player.gameSceneNode + 0x1F0);
			player.ReadBones();
		}

		if (config::show_head_tracker && !config::show_skeleton_esp) {
			player.gameSceneNode = Driver::rpm<uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_pGameSceneNode);
			player.boneArray = Driver::rpm<uintptr_t>(player.gameSceneNode + 0x1F0);
			player.ReadHead();
		}

		if (config::show_extra_flags) {
			/*
			* Reading values for extra flags is now seperated from the other reads
			* This removes unnecessary memory reads, improving performance when not showing extra flags
			*/
			player.is_defusing = Driver::rpm<bool>(player.pCSPlayerPawn + updater::offsets::m_bIsDefusing);

			playerMoneyServices = Driver::rpm<uintptr_t>(player.entity + updater::offsets::m_pInGameMoneyServices);
			player.money = Driver::rpm<int32_t>(playerMoneyServices + updater::offsets::m_iAccount);

			player.flashAlpha = Driver::rpm<float>(player.pCSPlayerPawn + updater::offsets::m_flFlashOverlayAlpha);

			clippingWeapon = Driver::rpm<std::uint64_t>(player.pCSPlayerPawn + updater::offsets::m_pClippingWeapon);
			std::uint64_t firstLevel = Driver::rpm<std::uint64_t>(clippingWeapon + 0x10); // First offset
			weaponData = Driver::rpm<std::uint64_t>(firstLevel + 0x20); // Final offset
			/*weaponData = Driver::rpm<std::uint64_t>(clippingWeapon + 0x10);
			weaponData = Driver::rpm<std::uint64_t>(weaponData + updater::offsets::m_szName);*/
			char buffer[MAX_PATH];
			Driver::rpm_pattern<char>(weaponData, buffer, sizeof(buffer));
			std::string weaponName = std::string(buffer);
			if (weaponName.compare(0, 7, "weapon_") == 0)
				player.weapon = weaponName.substr(7, weaponName.length()); // Remove weapon_ prefix
			else
				player.weapon = "Invalid Weapon Name";
		}

		list.push_back(player);
	}

	players.clear();
	players.assign(list.begin(), list.end());
}

uintptr_t boneAddress;
Vector3 bonePosition;
int boneIndex;
void CPlayer::ReadHead() {
	boneAddress = boneArray + 6 * 32;
	bonePosition = Driver::rpm<Vector3>(boneAddress);
	bones.bonePositions["head"] = g_game.world_to_screen(&bonePosition);
}

void CPlayer::ReadBones() {
	for (const auto& entry : boneMap) {
		const std::string& boneName = entry.first;
		boneIndex = entry.second;
		boneAddress = boneArray + boneIndex * 32;
		bonePosition = Driver::rpm<Vector3>(boneAddress);
		bones.bonePositions[boneName] = g_game.world_to_screen(&bonePosition);
	}
}

Vector3 CGame::world_to_screen(Vector3* v) {
	float _x = view_matrix[0][0] * v->x + view_matrix[0][1] * v->y + view_matrix[0][2] * v->z + view_matrix[0][3];
	float _y = view_matrix[1][0] * v->x + view_matrix[1][1] * v->y + view_matrix[1][2] * v->z + view_matrix[1][3];

	float w = view_matrix[3][0] * v->x + view_matrix[3][1] * v->y + view_matrix[3][2] * v->z + view_matrix[3][3];

	float inv_w = 1.f / w;
	_x *= inv_w;
	_y *= inv_w;

	float x = game_bounds.right * .5f;
	float y = game_bounds.bottom * .5f;

	x += 0.5f * _x * game_bounds.right + 0.5f;
	y -= 0.5f * _y * game_bounds.bottom + 0.5f;

	return { x, y, w };
}

void CGame::updateHWND() {
	windowHandle = FindWindowA(NULL, "Counter-Strike 2");
	if (windowHandle) {
		GetClientRect(windowHandle, &game_bounds);
	}
}