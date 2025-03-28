#pragma once
#include <cstdint>
#include <memory>
#include <string_view>
#include <TlHelp32.h>
#include <mutex>
#include <iostream>
#include <wtypes.h>
#include "../reader.hpp"


typedef struct _COPY_MEMORY {
	void* buffer;
	ULONG64		address;
	ULONG		size;
	HANDLE		pid;
	bool		get_pid;
	bool		base;
	bool		peb;
	bool		read;
	bool		write;
	bool		get_client;
	bool		get_engine;
	bool		get_engine_size;
	bool		get_client_size;
	const char* module_name;
	const char* process_name;
}COPY_MEMORY;

namespace Driver
{
	static std::once_flag flag;

	template<typename ... A>
	uint64_t call_hook(const A ... arguments)
	{
		std::call_once(flag, [] { LoadLibrary("user32.dll"); });
		void* control_function = GetProcAddress(LoadLibrary("win32u.dll"), "NtOpenCompositionSurfaceSectionInfo"); // same function as driver

		const auto control = static_cast<uint64_t(__stdcall*)(A...)>(control_function);
		return control(arguments ...);
	}

	static HANDLE get_process_id(const char* process_name) {
		COPY_MEMORY m{};
		m.get_pid = true;
		m.process_name = process_name;
		call_hook(&m);
		return m.pid;
	}

	static uintptr_t get_client(HANDLE pID)
	{
		COPY_MEMORY m{};
		m.get_client = true;
		m.pid = pID;
		call_hook(&m);
		return (uintptr_t)m.buffer;
	}

	static uintptr_t get_engine(HANDLE pID)
	{
		COPY_MEMORY m{};
		m.get_engine = true;
		m.pid = pID;
		call_hook(&m);
		return (uintptr_t)m.buffer;
	}

	static ULONG get_engine_size(HANDLE pID)
	{
		COPY_MEMORY m{};
		m.get_engine_size = true;
		m.pid = pID;
		call_hook(&m);
		return (ULONG)m.size;
	}

	static ULONG get_client_size(HANDLE pID)
	{
		COPY_MEMORY m{};
		m.get_client_size = true;
		m.pid = pID;
		call_hook(&m);
		return (ULONG)m.size;
	}

	static uintptr_t GetModuleBase(HANDLE pID, const char* modName)
	{
		COPY_MEMORY m{};
		m.base = true;
		m.pid = pID;
		m.module_name = modName;
		call_hook(&m);

		return (uintptr_t)m.buffer;
	}

	template<typename type>
	type rpm(uintptr_t readAddress)
	{
		type buffer{};

		COPY_MEMORY m{};
		m.read = true;
		m.pid = g::processID;
		m.address = readAddress;
		m.buffer = &buffer;
		m.size = sizeof(type);

		call_hook(&m);
		return buffer;
	}

	template<typename type>
	void rpm_pattern(uintptr_t readAddress, type* buffer, size_t size)
	{
		COPY_MEMORY m{};
		m.read = true;
		m.pid = g::processID;
		m.address = readAddress;
		m.buffer = buffer;
		m.size = size * sizeof(type);

		call_hook(&m);
	}

	template<typename type>
	void wpm(UINT_PTR writeAddress, type value)
	{
		COPY_MEMORY m{};
		m.write = true;
		m.pid = g::processID;
		m.address = writeAddress;
		m.buffer = &value;
		m.size = sizeof(value);
		call_hook(&m);
	}

	static bool is_driver_loaded() {
		try {
			// try to call simpl driver func to see if driver is loaded
			// system should always exist so we're checking that
			HANDLE system_pid = get_process_id("System");

			// if we get valid handle then driver is loaded
			if (system_pid != nullptr && system_pid != INVALID_HANDLE_VALUE)
				return true;

			return false;
		}
		catch (...) {
			std::cout << "[driver] exception occurred while checking driver status." << std::endl;
			return false;
		}
	}
};