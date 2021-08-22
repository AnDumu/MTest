#include "Utilities.h"

DWORD dwOldProtectionByte;
void WriteInProcessBYTE(DWORD Address, BYTE Value)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };

	if (VirtualQuery((LPVOID)Address, &mbi, sizeof(mbi)) == 0)
	{
		VirtualQueryEx(GetCurrentProcess(), (LPVOID)Address, &mbi, sizeof(mbi));
	}

	VirtualProtect((LPVOID)mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &dwOldProtectionByte);

	*(BYTE*)Address = Value;

	VirtualProtect((LPVOID)mbi.BaseAddress, mbi.RegionSize, dwOldProtectionByte, &dwOldProtectionByte);
}

BYTE ReadInProcessBYTE(DWORD Address)
{
	return *(BYTE*)Address;
}

DWORD dwOldProtectionWORD;
void WriteInProcessWORD(DWORD Address, WORD Value)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };

	if (VirtualQuery((LPVOID)Address, &mbi, sizeof(mbi)) == 0)
	{
		VirtualQueryEx(GetCurrentProcess(), (LPVOID)Address, &mbi, sizeof(mbi));
	}

	VirtualProtect((LPVOID)mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &dwOldProtectionWORD);

	*(WORD*)Address = Value;

	VirtualProtect((LPVOID)mbi.BaseAddress, mbi.RegionSize, dwOldProtectionWORD, &dwOldProtectionWORD);
}

WORD ReadInProcessWORD(DWORD Address)
{
	return *(WORD*)Address;
}

DWORD dwOldProtectionDWORD;
void WriteInProcessDWORD(DWORD Address, DWORD Value)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };

	if (VirtualQuery((LPVOID)Address, &mbi, sizeof(mbi)) == 0)
	{
		VirtualQueryEx(GetCurrentProcess(), (LPVOID)Address, &mbi, sizeof(mbi));
	}

	VirtualProtect((LPVOID)mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &dwOldProtectionDWORD);

	*(DWORD*)Address = Value;

	VirtualProtect((LPVOID)mbi.BaseAddress, mbi.RegionSize, dwOldProtectionDWORD, &dwOldProtectionDWORD);
}

DWORD ReadInProcessDWORD(DWORD Address)
{
	return *(DWORD*)Address;
}

DWORD dwOldProtectionFLOAT;
void WriteInProcessFloat(DWORD Address, float Value)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };

	if (VirtualQuery((LPVOID)Address, &mbi, sizeof(mbi)) == 0)
	{
		VirtualQueryEx(GetCurrentProcess(), (LPVOID)Address, &mbi, sizeof(mbi));
	}

	VirtualProtect((LPVOID)mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &dwOldProtectionFLOAT);

	*(float*)Address = Value; //typecast variable to a pointer of a byte and dereference the pointer, letting you change what it holds

	VirtualProtect((LPVOID)mbi.BaseAddress, mbi.RegionSize, dwOldProtectionFLOAT, &dwOldProtectionFLOAT);
}

float ReadInProcessFloat(DWORD Address)
{
	return *(float*)Address;
}

bool Check(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask != '?' && *pData != *bMask)
			return false;

	return (*szMask) == NULL;
}

/*
* "\x00\x15\xFF\x00", "?xx?", 0x03, Add, 0x300
*/
DWORD M_FindPattern(BYTE* bMask, char* szMask, DWORD dwOffset, DWORD dwAddress, DWORD dwLen)
{
	__try
	{
		for (DWORD i = 0; i < dwLen; i++)
			if (Check((BYTE*)(dwAddress + i), bMask, szMask))
				return (DWORD)(dwAddress + i + dwOffset);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		//MessageBox(NULL, "Find Pattern Error", "Error", MB_OK);
	}

	return 0;
}

DWORD M_FindPattern_Reverse(BYTE* bMask, char* szMask, DWORD dwOffset, DWORD dwAddress, DWORD dwLen)
{
	__try
	{
		for (int i = dwLen; i >= 0 ; i--)
			if (Check((BYTE*)(dwAddress + i), bMask, szMask))
				return (DWORD)(dwAddress + i + dwOffset);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		//MessageBox(NULL, "Find Pattern Error", "Error", MB_OK);
	}

	return 0;
}

DWORD dwOldProtection;
void UnProtect(DWORD Address)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };

	if (VirtualQuery((LPVOID)Address, &mbi, sizeof(mbi)) == 0)
	{
		VirtualQueryEx(GetCurrentProcess(), (LPVOID)Address, &mbi, sizeof(mbi));
	}

	VirtualProtect((LPVOID)mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &dwOldProtection);
}

void Protect(DWORD Address)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };

	if (VirtualQuery((LPVOID)Address, &mbi, sizeof(mbi)) == 0)
	{
		VirtualQueryEx(GetCurrentProcess(), (LPVOID)Address, &mbi, sizeof(mbi));
	}

	VirtualProtect((LPVOID)mbi.BaseAddress, mbi.RegionSize, dwOldProtection, &dwOldProtection);
}

bool AvailableAddress(LPVOID lpAddress)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };

	if (!VirtualQuery((LPCVOID)lpAddress, &mbi, sizeof(mbi))) return false;

	if (!mbi.BaseAddress) return false;

	return !IsBadCodePtr((FARPROC)lpAddress);
}

DWORD GetBaseAddress()
{
	TCHAR szProcessName[MAX_PATH] = { 0 }; // TEXT("<unknown>");
	HMODULE hMod;
	DWORD cbNeeded;

	if (EnumProcessModulesEx(GetCurrentProcess(), &hMod, sizeof(hMod), &cbNeeded, LIST_MODULES_32BIT | LIST_MODULES_64BIT))
	{
		GetModuleBaseName(GetCurrentProcess(), hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));

		if (hMod) return (DWORD)hMod;
	}

	return 0;
}