#pragma once
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <exception>
#include <psapi.h>
#include<iostream>
#include<thread>

LONG_PTR GetPID(const TCHAR* processName);
LONG_PTR GetModuleBase(const TCHAR* moduleName , HANDLE handle);

template <typename T>
T Read(HANDLE h,DWORD64 address)
{
	T cun;
	ReadProcessMemory(h, (void*)address, &cun, sizeof(T), NULL);
	return cun;
}

enum { FNameMaxBlockBits = 13 }; 
enum { FNameBlockOffsetBits = 16 };
enum { FNameMaxBlocks = 1 << FNameMaxBlockBits };
enum { FNameBlockOffsets = 1 << FNameBlockOffsetBits };
struct FNameEntryHandle
{
	uint32_t Block = 0;
	uint32_t Offset = 0;

	FNameEntryHandle(uint32_t InBlock, uint32_t InOffset)
		: Block(InBlock)
		, Offset(InOffset)
	{}

	FNameEntryHandle(uint32_t Id): Block(Id >> FNameBlockOffsetBits), Offset(Id& (FNameBlockOffsets - 1))
	{}

	explicit operator bool() const { return Block | Offset; }
};

typedef struct xyzw
{
	float x;
	float y;
	float z;
	float w;
}vec3;

uint64_t getmyselfaddress(HANDLE hp,DWORD64 base);