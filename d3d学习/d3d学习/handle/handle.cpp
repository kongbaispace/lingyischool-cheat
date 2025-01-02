#include "handle.h"
LONG_PTR GetPID(const TCHAR* processName)
{
	PROCESSENTRY32 pe = { sizeof(pe) };
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == NULL)
		throw std::exception("创建快照失败");
	bool success;
	success = Process32First(snapshot, &pe);
	if (_tcscmp(processName, pe.szExeFile) == 0)
	{
		CloseHandle(snapshot);
		return pe.th32ProcessID;
	}
	while (success)
	{
		success = Process32Next(snapshot, &pe);
		if (_tcscmp(processName, pe.szExeFile) == 0)
		{
			CloseHandle(snapshot);
			return pe.th32ProcessID;
		}
	}
	return NULL;
}

LONG_PTR GetModuleBase(const TCHAR* moduleName , HANDLE handle)
{
	HMODULE hmodules[1024];
	DWORD size;
	EnumProcessModules(handle, hmodules, sizeof(hmodules), &size);
	for (size_t i = 0; i < size; i++)
	{
		HMODULE mod = hmodules[i];
		TCHAR name[MAX_PATH];
		GetModuleBaseName(handle, mod, name, MAX_PATH);
		if (_tcscmp(moduleName, name) == 0)
		{
			return (LONG_PTR)mod;
		}
	}
	return NULL;
}

uint64_t getmyselfaddress(HANDLE hp, DWORD64 base)
{
	const DWORD64 baseoffset = 0x4B60010;
	const uint64_t offset[]{ 0x190 ,0x30,0x618 ,0x300,0x2A0 };
	uint64_t address = NULL;
	if (!base)
	{
		return NULL;
	}

	address = Read<uint64_t>(hp, base + baseoffset);
	for (auto i : offset)
		address = Read<uint64_t>(hp, address + i);


	if (address == NULL)
		return NULL;

	else {
		return address;
	}
}