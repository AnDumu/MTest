// MTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "TestManager.h"

class Test1
{
    public:
        Test1()
        {

        }

        void FunctionTest1()
        {
            std::cout << "\t" << "void FunctionTest1() called from Test1 class" << std::endl;
        }

        int FunctionTest2(int x, int y)
        {
            std::cout << "\t" "int FunctionTest2(int x, int y)(" << std::dec << x << ", " << y << ") called from Test1 class" << std::endl;

            return x + y;
        }

    protected:

    private:

};

void TryAddLog(tPreFunction tFunc)
{
    switch (MainManager.AddFunction(tFunc))
    {
        case MANAGER_ERR::eOK:
            std::cout << "Function " << tFunc.FunctionName.c_str() << "@" << std::dec << tFunc.CountParameters << " 0x" << std::hex << tFunc.FunctionAddress << " was added successfully!!!" << std::endl;

            break;
        case MANAGER_ERR::eNO_FUNCTION_NAME:
            std::cout << "ERROR! Function Name is missing." << std::endl;

            break;
        case MANAGER_ERR::eNULL_ADDRESS:
            std::cout << "ERROR! Function " << tFunc.FunctionName.c_str() << " has NULL address." << std::endl;

            break;
        case MANAGER_ERR::eALREADY_ADDED:
            std::cout << "ERROR! Function " << tFunc.FunctionName.c_str() << "@" << std::dec << tFunc.CountParameters << " already added." << std::endl;

            break;
    }
}

void AddTest_Test1Class_FunctionTest1(Test1 & pclass, int CountParameters, BOOL HasRet)
{
    tPreFunction tmp;

    tmp.FunctionName = "void Test1::FunctionTest1()";
    tmp.FunctionAddress = NULL;
    tmp.CountParameters = CountParameters;
    tmp.bRet = HasRet;


    // ########### ERROR - FUNCTION NAME EMPTY ###########
    tmp.FunctionName = "";
    TryAddLog(tmp);
    // #############################



    // ########### ERROR - FUNCTION ADDRESS NULL ###########
    tmp.FunctionName = "void Test1::FunctionTest1()";
    TryAddLog(tmp);
    // #############################



    // ########### SUCCESS ###########
    /*
    * 00671505  MTest.exe+11505 - E9 E6250000           - jmp MTest.Test1::FunctionTest1
    * 00673AF0  MTest.Test1::FunctionTest1 - 55                    - push ebp
    */

    __asm
    {
        lea eax, Test1::FunctionTest1
        mov [tmp.FunctionAddress], eax
    }

    //std::cout << std::hex << tmp.FunctionAddress << std::endl;

    TryAddLog(tmp);
    // #############################



    // ########### ERROR - ALREADY ADDED ###########
    __asm
    {
        lea eax, Test1::FunctionTest1
        cmp byte ptr [eax], 0xE9
        jne pass
        mov ebx, [eax + 0x1]
        add eax, 0x5
        add eax, ebx
pass:
        mov [tmp.FunctionAddress], eax
    }

    //std::cout << std::hex << tmp.FunctionAddress << std::endl;

    TryAddLog(tmp);
    // #############################

    std::cout << std::endl << "Press enter to call " << tmp.FunctionName.c_str() << std::endl;
    std::cin.get();

    pclass.FunctionTest1();
    pclass.FunctionTest1();
    pclass.FunctionTest1();

    std::cout << "Done!" << std::endl;
    std::cout << "----------------------------------------------" << std::endl << std::endl;
    std::cin.get();
}

void AddTest_Test1Class_FunctionTest2(Test1 & pclass, int CountParameters, BOOL HasRet)
{
    tPreFunction tmp;
    int hRet;

    tmp.FunctionName = "int Test1::FunctionTest2(int x, int y)";
    tmp.FunctionAddress = NULL;
    tmp.CountParameters = CountParameters;
    tmp.bRet = HasRet;

    __asm
    {
        lea eax, Test1::FunctionTest2
        mov [tmp.FunctionAddress], eax
    }

    //std::cout << std::hex << tmp.FunctionAddress << std::endl;

    TryAddLog(tmp);

    std::cout << std::endl << "Press enter to call " << tmp.FunctionName.c_str() << std::endl;
    std::cin.get();

    hRet = pclass.FunctionTest2(1, 2);
    std::cout << "\t" << "Ret: " << std::hex << hRet << " (dec: " << std::dec << hRet << ")" << std::endl;
    hRet = pclass.FunctionTest2(3, 4);
    std::cout << "\t" << "Ret: " << std::hex << hRet << " (dec: " << std::dec << hRet << ")" << std::endl;
    hRet = pclass.FunctionTest2(5, 6);
    std::cout << "\t" << "Ret: " << std::hex << hRet << " (dec: " << std::dec << hRet << ")" << std::endl;
    hRet = pclass.FunctionTest2(GetBaseAddress(), 7);
    std::cout << "\t" << "Ret: " << std::hex << hRet << " (dec: " << std::dec << hRet << ")" << std::endl;

    std::cout << "Done!" << std::endl;
    std::cout << "----------------------------------------------" << std::endl << std::endl;
    std::cin.get();
}

void Init()
{
    Test1 testing;

    std::cout << "Class Address: " << std::hex << &testing << std::endl; //stored in ECX

    AddTest_Test1Class_FunctionTest1(testing, 0, false);
    AddTest_Test1Class_FunctionTest2(testing, 2, true);
}

void ShowResults()
{
    for (size_t i = 0; i < MainManager.VT.size(); i++)
    {
        std::cout << "(0x" << std::hex << MainManager.VT[i].FunctionAddress << ") '" << MainManager.VT[i].FunctionName.c_str() << "' has been called " << std::dec << MainManager.VT[i].Times << " time(s)" << std::endl << std::endl;

        for (size_t i2 = 0; i2 < MainManager.VT[i].Times; i2++)
        {
            std::cout << "[" << std::dec << i2 << "]:" << std::endl;
            std::cout << "\t" << "EAX: 0x" << std::hex << MainManager.VT[i].Calls[i2].EAX << std::endl;
            std::cout << "\t" << "EBX: 0x" << std::hex << MainManager.VT[i].Calls[i2].EBX << std::endl;
            std::cout << "\t" << "ECX: 0x" << std::hex << MainManager.VT[i].Calls[i2].ECX << std::endl;
            std::cout << "\t" << "EDX: 0x" << std::hex << MainManager.VT[i].Calls[i2].EDX << std::endl;
            std::cout << "\t" << "ESI: 0x" << std::hex << MainManager.VT[i].Calls[i2].ESI << std::endl;
            std::cout << "\t" << "EDI: 0x" << std::hex << MainManager.VT[i].Calls[i2].EDI << std::endl;
            std::cout << "\t" << "EBP: 0x" << std::hex << MainManager.VT[i].Calls[i2].EBP << std::endl;
            std::cout << "\t" << "ESP: 0x" << std::hex << MainManager.VT[i].Calls[i2].ESP << std::endl;
            std::cout << "\t" << "Flags: 0x" << std::hex << MainManager.VT[i].Calls[i2].Flags << std::endl;

            if (MainManager.VT[i].Calls[i2].Params.size() > 0)
            {
                for (size_t i3 = 0; i3 < MainManager.VT[i].Calls[i2].Params.size(); i3++)
                {
                    std::cout << "\t\t" << "Param[" << std::dec << i3 << "]: " << std::endl;
                    std::cout << "\t\t\t" << std::hex << "0x" << MainManager.VT[i].Calls[i2].Params[i3].Param << " (dec: " << std::dec << MainManager.VT[i].Calls[i2].Params[i3].Param << "), [" << std::hex << MainManager.VT[i].Calls[i2].Params[i3].ParamValue << "]" << std::endl;
                }
            }

            if (MainManager.VT[i].bRet)
            {
                std::cout << std::endl << "\t" << ">>> Ret: 0x" << std::hex << MainManager.VT[i].Calls[i2].Ret << " (dec: " << std::dec << MainManager.VT[i].Calls[i2].Ret << ")" << std::endl;
            }

            std::cout << std::endl;

            std::cin.get();
        }
    }
}

int main()
{
    Init();

    ShowResults();

    std::cin.get();

    return 0;
}
