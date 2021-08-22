#include "TestManager.h"

TestManager MainManager;

DWORD TempAddress = NULL;
DWORD TempAddressRet = NULL;

DWORD RetEAX;
DWORD BkCalled;

DWORD MainProcessor_EAX = NULL;
DWORD MainProcessor_EBX = NULL;
DWORD MainProcessor_ECX = NULL;
DWORD MainProcessor_EDX = NULL;
DWORD MainProcessor_ESI = NULL;
DWORD MainProcessor_EDI = NULL;
DWORD MainProcessor_EBP = NULL;
DWORD MainProcessor_ESP = NULL;
DWORD MainProcessor_Flags = NULL;

TestManager::TestManager()
{
	//
}

MANAGER_ERR TestManager::AddFunction(tPreFunction tFunc)
{
	DWORD Address = (DWORD)tFunc.FunctionAddress;

	if (tFunc.FunctionName.size() == 0) return MANAGER_ERR::eNO_FUNCTION_NAME;
	if (!Address) return MANAGER_ERR::eNULL_ADDRESS;

	if (*(BYTE*)Address == 0xE9)
	{
		__asm
		{
			mov eax, [Address]
			mov ebx, [eax + 0x1]
			add eax, 0x5
			add eax, ebx
			mov [Address], eax
		}
	}

	for (size_t i = 0; i < this->VT.size(); i++)
	{
		if (VT[i].FunctionAddress == (MyGenericVoidNOPARAM)Address || VT[i].FunctionName == tFunc.FunctionName)
		{
			return MANAGER_ERR::eALREADY_ADDED;
		}
	}

	//std::cout << std::hex << Address << std::endl;
	//std::cin.get();

	tFunction tmpFunction;

	tmpFunction.FunctionName = tFunc.FunctionName;
	tmpFunction.FunctionAddress = (MyGenericVoidNOPARAM)Address;
	
	__asm
	{
		lea eax, [PreProcessor]
		mov [Address], eax
	}

	tmpFunction.TrampolineAddress = (MyGenericVoidNOPARAM)DetourFunction((PBYTE)tmpFunction.FunctionAddress, (PBYTE)Address);

	WriteInProcessBYTE((DWORD)tmpFunction.FunctionAddress, 0xE8);
	WriteInProcessDWORD((DWORD)tmpFunction.FunctionAddress + 0x01, Address - ((DWORD)tmpFunction.FunctionAddress + 0x05));

	tmpFunction.CountParameters = tFunc.CountParameters;
	tmpFunction.bRet = tFunc.bRet;
	tmpFunction.Times = 0;
	tmpFunction.Calls.clear();

	VT.push_back(tmpFunction);

	return MANAGER_ERR::eOK;
}

void TestManager::MainProcessor(BOOL Ret)
{
	DWORD tmpStack;
	DWORD StackValue;
	DWORD CountParams = 0;
	BOOL Found = false;
	tParams Param;

	//std::cout << "Called from " << std::hex << TempAddress << std::endl;

	for (size_t i = 0; i < this->VT.size(); i++)
	{
		if (Ret)
		{
			if (this->VT[i].FunctionAddress == (MyGenericVoidNOPARAM)BkCalled)
			{
				this->VT[i].Calls[this->VT[i].Calls.size() - 1].Ret = RetEAX;

				break;
			}
		}
		else
		{
			if (this->VT[i].FunctionAddress == (MyGenericVoidNOPARAM)TempAddress)
			{
				TempAddress = (DWORD)this->VT[i].TrampolineAddress;
				this->VT[i].Times++;

				tCall tmpCall;

				tmpCall.EAX = MainProcessor_EAX;
				tmpCall.EBX = MainProcessor_EBX;
				tmpCall.ECX = MainProcessor_ECX;
				tmpCall.EDX = MainProcessor_EDX;
				tmpCall.ESI = MainProcessor_ESI;
				tmpCall.EDI = MainProcessor_EDI;
				tmpCall.EBP = MainProcessor_EBP;
				tmpCall.ESP = MainProcessor_ESP;
				tmpCall.Flags = MainProcessor_Flags;

				if (this->VT[i].CountParameters > 0)
				{
					tmpStack = tmpCall.ESP;

					while (TRUE)
					{
						if (!Found)
						{
							if (AvailableAddress((LPVOID)tmpStack))
							{
								StackValue = ReadInProcessDWORD(tmpStack);

								if (StackValue - 0x05 == (DWORD)this->VT[i].FunctionAddress)
								{
									tmpStack += 0x04; //fix the first call stack align
									Found = true;
								}
							}
						}
						else
						{
							CountParams++;
							StackValue = ReadInProcessDWORD(tmpStack);

							Param.Param = StackValue;
							Param.ParamValue = (AvailableAddress((LPVOID)StackValue) ? ReadInProcessDWORD(StackValue) : NULL);

							tmpCall.Params.push_back(Param);

							//std::cout << "\t" << std::dec << "Param[" << CountParams << "]: " << std::hex << Param.Param << " (dec: " << std::dec << StackValue << "), [" << std::hex << Param.ParamValue << "]" << std::endl;
						}

						tmpStack += 0x04;

						if (CountParams == this->VT[i].CountParameters)
						{
							break;
						}
					}
				}

				this->VT[i].Calls.push_back(tmpCall);

				break;
			}
		}
	}
}

__declspec(naked) void __stdcall PreProcessor()
{
	__asm
	{
		mov [MainProcessor_EAX], eax
		mov [MainProcessor_EBX], ebx
		mov [MainProcessor_ECX], ecx
		mov [MainProcessor_EDX], edx
		mov [MainProcessor_ESI], esi
		mov [MainProcessor_EDI], edi
		mov [MainProcessor_EBP], ebp
		mov [MainProcessor_ESP], esp

		pushfd
		pop [MainProcessor_Flags]
	}

	__asm
	{
		mov eax, [esp]
		sub eax, 0x05
		mov [TempAddress], eax

		mov eax, [esp + 0x4]
		mov [TempAddressRet], eax
	}

	__asm pushad;
	__asm pushfd;

	MainManager.MainProcessor(false);

	__asm popfd;
	__asm popad;

	__asm
	{
		mov eax, [MainProcessor_EAX]
		mov ebx, [MainProcessor_EBX]
		mov ecx, [MainProcessor_ECX]
		mov edx, [MainProcessor_EDX]
		mov esi, [MainProcessor_ESI]
		mov edi, [MainProcessor_EDI]
		mov ebp, [MainProcessor_EBP]
		mov esp, [MainProcessor_ESP]

		push eax

		mov eax, [esp + 0x04]
		sub eax, 0x05
		mov [BkCalled], eax

		mov eax, [CaptureRet]
		mov [esp + 0x08], eax

		pop eax

		add esp, 0x04

		jmp [TempAddress]
	}
}

__declspec(naked) void __stdcall CaptureRet()
{
	__asm
	{
		mov [RetEAX], eax
	}

	__asm pushad;
	__asm pushfd;

	MainManager.MainProcessor(true);

	__asm popfd;
	__asm popad;

	__asm
	{
		jmp [TempAddressRet]
	}
}