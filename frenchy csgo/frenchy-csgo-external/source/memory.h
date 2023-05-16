#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <d3d11.h>
#include <cstdint>
#include "hacks.h"

namespace Memory
{

	std::uintptr_t processId = 0;
	void* processHandle = nullptr;
	DWORD get_process_id(const WCHAR* name) noexcept {
		PROCESSENTRY32W pe;
		pe.dwSize = sizeof(PROCESSENTRY32);

		const HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0UL);

		if (!ss) {
			return DWORD{};
		}

		DWORD pid{};

		do {
			if (!wcscmp(name, pe.szExeFile)) {
				pid = pe.th32ProcessID;
				break;
			}
		} while (Process32Next(ss, &pe));
		processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		CloseHandle(ss);

		return pid;
	}

	DWORD get_module_adress(DWORD pid, const WCHAR* name) {
		MODULEENTRY32 me{};
		me.dwSize = sizeof(MODULEENTRY32);

		const HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

		if (!ss) {
			return DWORD{};
		}

		DWORD address{};

		do {
			if (!wcscmp(name, me.szModule)) {
				address = reinterpret_cast<DWORD>(me.modBaseAddr);
				break;
			}
		} while (Module32Next(ss, &me));

		CloseHandle(ss);

		return address;
	}

	template <class T>
	T read(const HANDLE process, const DWORD address) noexcept {
		T value{};
		ReadProcessMemory(process, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), nullptr);
		return value;
	}

	

	template <class T>
	void write(const HANDLE process, const DWORD address, const T& value) noexcept {
		WriteProcessMemory(process, reinterpret_cast<LPVOID>(address), &value, sizeof(T), nullptr);
	}

}







