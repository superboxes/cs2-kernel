#include "auto_updater.hpp"


namespace updater {
	bool check_and_update() {
		json commit;
		if (!get_last_commit_date(commit)) {
			std::cout << "[updater] error getting last commit information from GitHub" << std::endl;
			return false;
		}

		std::string last_commit_date = commit["date"];
		std::string last_commit_author_name = commit["name"];

		std::cout << "[updater] Last GitHub repository update was made by " << last_commit_author_name << " on " << last_commit_date.substr(0, 10) << std::endl;

		std::tm commit_date = {};
		std::istringstream(last_commit_date) >> std::get_time(&commit_date, "%Y-%m-%dT%H:%M:%SZ");

		std::chrono::system_clock::time_point commitTimePoint = std::chrono::system_clock::from_time_t(std::mktime(&commit_date));

		if (file_good("offsets.json")) {
			fs::file_time_type lastModifiedTime = fs::last_write_time("offsets.json");
			auto lastModifiedClockTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
				lastModifiedTime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());

			// check if local file is older than the last commit
			if (lastModifiedClockTime < commitTimePoint) {
				std::cout << "[updater] Local file is older than the last GitHub commit." << std::endl;

				char response;

				std::cout << "[updater] Do you want to download the latest offsets? (y/n): ";
				std::cin >> response;

				if (std::tolower(response) == 'y') {
					download_offsets();
				}
			}
			else {
				std::cout << "[updater] Local file is up to date.\n" << std::endl;
			}
		}
		else {
			char response;
			std::cout << "[updater] Do you want to download the latest offsets? (y/n): ";
			std::cin >> response;


			if (std::tolower(response) == 'y') {
				download_offsets();
			}
		}

		return false;
	}

	// fetch the latest offsets from 
	/*
		const inline std::string raw_offsets = "https://raw.githubusercontent.com/a2x/cs2-dumper/refs/heads/main/output/offsets.json";
		const inline std::string raw_client_offsets = "https://raw.githubusercontent.com/a2x/cs2-dumper/refs/heads/main/output/client_dll.json";
	*/
	// and then save specific offsets from offsets.json and client_dll.json to one file offsets.json
	bool download_offsets() {
		json offsets;
		json client_offsets;

		std::cout << "[updater] Downloading offsets from GitHub repo" << std::endl;

		if (!get_json(offsets, raw_offsets)) {
			std::cout << "[updater] error getting offsets.json from GitHub repo" << std::endl;
			return false;
		}

		std::cout << "[updater] Downloading client offsets from GitHub repo" << std::endl;

		if (!get_json(client_offsets, raw_client_offsets)) {
			std::cout << "[updater] error getting client_dll.json from GitHub repo" << std::endl;
			return false;
		}

		std::cout << "[updater] Defining offsets" << std::endl;

		if (!define_offsets(offsets, client_offsets)) {
			std::cout << "[updater] error defining offsets" << std::endl;
			return false;
		}

		std::cout << "[updater] Saving offsets" << std::endl;

		save();

		return true;
	}

	bool define_offsets(json offsets, json client_offsets)
	{
		offsets::dwLocalPlayerController = offsets["client.dll"]["dwLocalPlayerController"];
		offsets::dwEntityList = offsets["client.dll"]["dwEntityList"];
		offsets::dwViewMatrix = offsets["client.dll"]["dwViewMatrix"];
		offsets::dwBuildNumber = offsets["engine2.dll"]["dwBuildNumber"];
		offsets::dwPlantedC4 = offsets["client.dll"]["dwPlantedC4"];
		offsets::dwViewAngles = offsets["client.dll"]["dwViewAngles"];

		offsets::m_flC4Blow = client_offsets["client.dll"]["classes"]["C_PlantedC4"]["fields"]["m_flC4Blow"];
		offsets::m_flNextBeep = client_offsets["client.dll"]["classes"]["C_PlantedC4"]["fields"]["m_flNextBeep"];
		offsets::m_flTimerLength = client_offsets["client.dll"]["classes"]["C_PlantedC4"]["fields"]["m_flTimerLength"];
		offsets::m_pInGameMoneyServices = client_offsets["client.dll"]["classes"]["CCSPlayerController"]["fields"]["m_pInGameMoneyServices"];
		offsets::m_iAccount = client_offsets["client.dll"]["classes"]["CCSPlayerController_InGameMoneyServices"]["fields"]["m_iAccount"];
		offsets::m_vecAbsOrigin = client_offsets["client.dll"]["classes"]["CGameSceneNode"]["fields"]["m_vecAbsOrigin"];
		offsets::m_vOldOrigin = client_offsets["client.dll"]["classes"]["C_BasePlayerPawn"]["fields"]["m_vOldOrigin"];
		offsets::m_pGameSceneNode = client_offsets["client.dll"]["classes"]["C_BaseEntity"]["fields"]["m_pGameSceneNode"];
		offsets::m_flFlashOverlayAlpha = client_offsets["client.dll"]["classes"]["C_CSPlayerPawnBase"]["fields"]["m_flFlashOverlayAlpha"];
		offsets::m_bIsDefusing = client_offsets["client.dll"]["classes"]["C_CSPlayerPawn"]["fields"]["m_bIsDefusing"];

		offsets::m_szName = client_offsets["client.dll"]["classes"]["CCSWeaponBaseVData"]["fields"]["m_szName"];
		offsets::m_pClippingWeapon = client_offsets["client.dll"]["classes"]["C_CSPlayerPawnBase"]["fields"]["m_pClippingWeapon"];
		offsets::m_ArmorValue = client_offsets["client.dll"]["classes"]["C_CSPlayerPawn"]["fields"]["m_ArmorValue"];
		offsets::m_iHealth = client_offsets["client.dll"]["classes"]["C_BaseEntity"]["fields"]["m_iHealth"];
		offsets::m_hPlayerPawn = client_offsets["client.dll"]["classes"]["CCSPlayerController"]["fields"]["m_hPlayerPawn"];
		offsets::m_sSanitizedPlayerName = client_offsets["client.dll"]["classes"]["CCSPlayerController"]["fields"]["m_sSanitizedPlayerName"];
		offsets::m_iTeamNum = client_offsets["client.dll"]["classes"]["C_BaseEntity"]["fields"]["m_iTeamNum"];
		offsets::m_iIDEntIndex = client_offsets["client.dll"]["classes"]["C_CSPlayerPawnBase"]["fields"]["m_iIDEntIndex"];
		offsets::m_aimPunchAngle = client_offsets["client.dll"]["classes"]["C_CSPlayerPawn"]["fields"]["m_aimPunchAngle"];
		offsets::m_vecViewOffset = client_offsets["client.dll"]["classes"]["C_BaseModelEntity"]["fields"]["m_vecViewOffset"];

		std::cout << "[updater] Defined offsets" << std::endl;

		return true;
	}

	// get json data from the given url
	bool get_json(json& data, std::string url)
	{
		HINTERNET hInternet, hConnect;

		hInternet = InternetOpen("AutoUpdater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (!hInternet) {
			return false;
		}

		hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
		if (!hConnect) {
			InternetCloseHandle(hInternet);
			return false;
		}

		char buffer[4096];
		DWORD bytesRead;
		std::string commitData;

		while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
			commitData.append(buffer, bytesRead);
		}

		try {
			data = json::parse(commitData);
		}
		catch (const std::exception& e) {
			std::cout << "[updater] exception while parsing json response from github" << std::endl;
			return false;
		}

		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);

		return true;
	}

	bool get_last_commit_date(json& commit) {
		HINTERNET hInternet, hConnect;

		hInternet = InternetOpen("AutoUpdater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (!hInternet) {
			return false;
		}

		hConnect = InternetOpenUrlA(hInternet, github_repo_commits.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
		if (!hConnect) {
			InternetCloseHandle(hInternet);
			return false;
		}

		char buffer[4096];
		DWORD bytesRead;
		std::string commitData;

		while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
			commitData.append(buffer, bytesRead);
		}

		json data;
		try {
			data = json::parse(commitData);
		}
		catch (const std::exception& e) {
			std::cout << "[updater] exception while parsing json response from github" << std::endl;
			return false;
		}

		if (data.empty())
			return false;

		if (data.is_array()) {
			json last_commit = data[0];
			json last_commit_author = last_commit["commit"]["author"];

			commit = last_commit_author;
		}

		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);

		return true;
	}

	bool download_file(const char* url, const char* localPath) {
		HINTERNET hInternet, hConnect;

		hInternet = InternetOpen("AutoUpdater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (!hInternet) {
			std::cerr << "InternetOpen failed." << std::endl;
			return false;
		}

		hConnect = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
		if (!hConnect) {
			std::cerr << "InternetOpenUrlA failed." << std::endl;
			InternetCloseHandle(hInternet);
			return false;
		}

		std::ofstream outFile(localPath, std::ios::binary);
		if (!outFile) {
			std::cerr << "Failed to create local file." << std::endl;
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hInternet);
			return false;
		}

		char buffer[4096];
		DWORD bytesRead;

		while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
			outFile.write(buffer, bytesRead);
		}

		outFile.close();
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);

		return true;
	}

	bool file_good(const std::string& name) {
		std::ifstream f(name.c_str());
		return f.good();
	}

	bool read() {
		if (!updater::file_good(file_path)) {
			save();
			return false;
		}

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

		/* little cross compatibility with older builds */
		if (data["dwLocalPlayer"].is_number())
			offsets::dwLocalPlayerController = data["dwLocalPlayer"];
		if (data["dwLocalPlayerController"].is_number())
			offsets::dwLocalPlayerController = data["dwLocalPlayerController"];
		if (data["dwEntityList"].is_number())
			offsets::dwEntityList = data["dwEntityList"];
		if (data["dwViewMatrix"].is_number())
			offsets::dwViewMatrix = data["dwViewMatrix"];
		if (data["dwBuildNumber"].is_number())
			offsets::dwBuildNumber = data["dwBuildNumber"];
		if (data["dwPlantedC4"].is_number())
			offsets::dwPlantedC4 = data["dwPlantedC4"];

		if (data["m_flC4Blow"].is_number())
			offsets::m_flC4Blow = data["m_flC4Blow"];
		if (data["m_flNextBeep"].is_number())
			offsets::m_flNextBeep = data["m_flNextBeep"];
		if (data["m_flTimerLength"].is_number())
			offsets::m_flTimerLength = data["m_flTimerLength"];

		if (data["m_pInGameMoneyServices"].is_number())
			offsets::m_pInGameMoneyServices = data["m_pInGameMoneyServices"];
		if (data["m_iAccount"].is_number())
			offsets::m_iAccount = data["m_iAccount"];
		if (data["m_vecAbsOrigin"].is_number())
			offsets::m_vecAbsOrigin = data["m_vecAbsOrigin"];
		if (data["m_vOldOrigin"].is_number())
			offsets::m_vOldOrigin = data["m_vOldOrigin"];
		if (data["m_pGameSceneNode"].is_number())
			offsets::m_pGameSceneNode = data["m_pGameSceneNode"];
		if (data["m_flFlashOverlayAlpha"].is_number())
			offsets::m_flFlashOverlayAlpha = data["m_flFlashOverlayAlpha"];
		if (data["m_bIsDefusing"].is_number())
			offsets::m_bIsDefusing = data["m_bIsDefusing"];
		if (data["m_szName"].is_number())
			offsets::m_szName = data["m_szName"];
		if (data["m_pClippingWeapon"].is_number())
			offsets::m_pClippingWeapon = data["m_pClippingWeapon"];
		if (data["m_ArmorValue"].is_number())
			offsets::m_ArmorValue = data["m_ArmorValue"];
		if (data["m_iHealth"].is_number())
			offsets::m_iHealth = data["m_iHealth"];
		if (data["m_hPlayerPawn"].is_number())
			offsets::m_hPlayerPawn = data["m_hPlayerPawn"];
		if (data["dwSanitizedName"].is_number())
			offsets::m_sSanitizedPlayerName = data["m_sSanitizedPlayerName"];
		if (data["m_iTeamNum"].is_number())
			offsets::m_iTeamNum = data["m_iTeamNum"];
		if (data["m_iIDEntIndex"].is_number())
			offsets::m_iIDEntIndex = data["m_iIDEntIndex"];
		if (data["dwViewAngles"].is_number())
			offsets::dwViewAngles = data["dwViewAngles"];
		if (data["m_aimPunchAngle"].is_number())
			offsets::m_aimPunchAngle = data["m_aimPunchAngle"];
		if (data["m_vecViewOffset"].is_number())
			offsets::m_vecViewOffset = data["m_vecViewOffset"];

		return true;
	}

	void save() {
		json data;

		data["dwLocalPlayerController"] = offsets::dwLocalPlayerController;
		data["dwEntityList"] = offsets::dwEntityList;
		data["dwViewMatrix"] = offsets::dwViewMatrix;
		data["dwBuildNumber"] = offsets::dwBuildNumber;
		data["dwPlantedC4"] = offsets::dwPlantedC4;

		data["m_flNextBeep"] = offsets::m_flNextBeep;
		data["m_flC4Blow"] = offsets::m_flC4Blow;
		data["m_flTimerLength"] = offsets::m_flTimerLength;

		data["m_pInGameMoneyServices"] = offsets::m_pInGameMoneyServices;
		data["m_iAccount"] = offsets::m_iAccount;
		data["m_vecAbsOrigin"] = offsets::m_vecAbsOrigin;
		data["m_vOldOrigin"] = offsets::m_vOldOrigin;
		data["m_pGameSceneNode"] = offsets::m_pGameSceneNode;
		data["m_flFlashOverlayAlpha"] = offsets::m_flFlashOverlayAlpha;
		data["m_bIsDefusing"] = offsets::m_bIsDefusing;
		data["m_szName"] = offsets::m_szName;
		data["m_pClippingWeapon"] = offsets::m_pClippingWeapon;
		data["m_ArmorValue"] = offsets::m_ArmorValue;
		data["m_iHealth"] = offsets::m_iHealth;
		data["m_hPlayerPawn"] = offsets::m_hPlayerPawn;
		data["m_sSanitizedPlayerName"] = offsets::m_sSanitizedPlayerName;
		data["m_iTeamNum"] = offsets::m_iTeamNum;
		data["m_iIDEntIndex"] = offsets::m_iIDEntIndex;
		data["dwViewAngles"] = offsets::dwViewAngles;
		data["m_aimPunchAngle"] = offsets::m_aimPunchAngle;
		data["m_vecViewOffset"] = offsets::m_vecViewOffset;

		std::ofstream output(file_path);
		output << std::setw(4) << data << std::endl;
		output.close();
	}

	bool load_driver(const char* dLocalPath, const char* kdmLocalPath)
	{
		if (!updater::file_good(dLocalPath)) {
			std::cout << "[driver] Driver not found" << std::endl;
			return false;
		}

		if (!updater::file_good(kdmLocalPath)) {
			std::cout << "[driver] KDMapper not found" << std::endl;
			return false;
		}

		std::cout << "[driver] Loading driver" << std::endl;


	}
}
