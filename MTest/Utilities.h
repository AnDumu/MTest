#include <iostream>
#include <Windows.h>

#include <Iphlpapi.h>
#pragma comment ( lib, "Iphlpapi.lib" )

#include <psapi.h>  //GetModuleBaseName
#pragma comment(lib, "psapi.lib") //GetModuleBaseName

#include <TlHelp32.h> //PARA LOS THREADS y PROCESOS

void WriteInProcessBYTE(DWORD Address, BYTE Value);
BYTE ReadInProcessBYTE(DWORD Address);
void WriteInProcessWORD(DWORD Address, WORD Value);
WORD ReadInProcessWORD(DWORD Address);
void WriteInProcessDWORD(DWORD Address, DWORD Value);
DWORD ReadInProcessDWORD(DWORD Address);
void WriteInProcessFloat(DWORD Address, float Value);
float ReadInProcessFloat(DWORD Address);
bool Check(const BYTE* pData, const BYTE* bMask, const char* szMask);
DWORD M_FindPattern(BYTE* bMask, char* szMask, DWORD dwOffset, DWORD dwAddress, DWORD dwLen);
DWORD M_FindPattern_Reverse(BYTE* bMask, char* szMask, DWORD dwOffset, DWORD dwAddress, DWORD dwLen);
void UnProtect(DWORD Address);
void Protect(DWORD Address);
bool AvailableAddress(LPVOID lpAddress);
DWORD GetBaseAddress();