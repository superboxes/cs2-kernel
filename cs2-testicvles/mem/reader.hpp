#pragma once
#include "../classes/vector.hpp"
#include <map>
#include <string>
#include <mutex>
#include <wtypes.h>
#include <vector>

struct view_matrix_t {
	float* operator[ ](int index) {
		return matrix[index];
	}

	float matrix[4][4];
};

class CBones {
public:
	std::map<std::string, Vector3> bonePositions;
};

class CC4 {
public:
	uintptr_t get_planted();
	uintptr_t get_node();
	Vector3 get_origin();
};

class CPlayer {
public:
	uintptr_t entity;
	int team;
	uintptr_t pCSPlayerPawn;
	uintptr_t gameSceneNode;
	uintptr_t boneArray;
	uintptr_t spottedState;
	int health;
	int armor;
	std::string name;
	Vector3 origin;
	Vector3 head;
	CBones bones;
	bool is_defusing;
	bool is_spotted;
	int32_t money;
	float flashAlpha;
	std::string weapon;
	void ReadBones();
	void ReadHead();
};

class CGame
{
public:
	uintptr_t base_client;
	uintptr_t base_client_size;
	uintptr_t base_engine;
	uintptr_t base_engine_size;
	RECT game_bounds;
	uintptr_t buildNumber;
	bool inGame;
	Vector3 localOrigin;
	bool isC4Planted;
	int localTeam;
	uintptr_t entity_list;
	int aim_entity_team;
	CC4 c4;
	int crosshairEntityId;
	uintptr_t aimedEntity;
	uintptr_t localpCSPlayerPawn;
	uintptr_t localPlayer;
	Vector3 eye;
	Vector3 viewAngles;
	Vector3 localPlayerPosition;
	std::vector<CPlayer> players = {};
	void init();
	void loop();
	void close();
	void updateHWND();
	void setViewAngle(Vector3 vwa);
	Vector3 world_to_screen(Vector3* v);
	HWND windowHandle = NULL;
private:
	view_matrix_t view_matrix;
	std::uint32_t localPlayerPawn;
	uintptr_t localList_entry2;
	uintptr_t en_list_entry;
};

inline CGame g_game;

inline std::mutex reader_mutex;

namespace g {
	inline HANDLE processID = NULL;
	inline HDC hdcBuffer = NULL;
	inline HBITMAP hbmBuffer = NULL;
}