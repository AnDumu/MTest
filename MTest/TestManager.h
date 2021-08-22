#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>
#include <functional>

//Detour 1.5
#include "detours.h"
#pragma comment ( lib, "detours.lib" )

#include "Utilities.h"

void __stdcall PreProcessor();
void __stdcall CaptureRet();

typedef void(__stdcall* MyGenericVoidNOPARAM) ();

struct tParams
{
	DWORD Param;
	DWORD ParamValue;
};

struct tCall
{
	DWORD EAX;
	DWORD EBX;
	DWORD ECX;
	DWORD EDX;
	DWORD ESI;
	DWORD EDI;
	DWORD EBP;
	DWORD ESP;
	DWORD Flags;

	std::vector<tParams> Params;

	DWORD Ret;
};

struct tFunction
{
	std::string FunctionName;
	MyGenericVoidNOPARAM FunctionAddress;
	MyGenericVoidNOPARAM TrampolineAddress;
	DWORD CountParameters;
	BOOL bRet;
	DWORD Times;
	std::vector<tCall> Calls;
};

struct tPreFunction
{
	std::string FunctionName;
	MyGenericVoidNOPARAM FunctionAddress;
	DWORD CountParameters;
	BOOL bRet;
};

enum MANAGER_ERR
{
	eOK,
	eNO_FUNCTION_NAME,
	eNULL_ADDRESS,
	eALREADY_ADDED
};

class TestManager
{
	public:
		std::vector<tFunction> VT;

		TestManager();
		MANAGER_ERR AddFunction(tPreFunction tFunc);
		void MainProcessor(BOOL Ret);

	protected:

	private:
		
};

extern TestManager MainManager;