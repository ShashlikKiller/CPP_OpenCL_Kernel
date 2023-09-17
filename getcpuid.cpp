#include "getcpuid.h"

    struct cpuid_regs
    {
        DWORD   Eax;
        DWORD   Ebx;
        DWORD   Ecx;
        DWORD   Edx;
    };

    std::string SplitIntoChars(DWORD Value)
    {
        std::string Str;
        char const* pCursor = (char const*)&Value;
        for (int i = 0; i < sizeof(Value); ++i) {
            Str += pCursor[i];
        }
        return Str;
    }

    std::string GetCpuVendorSubstring(DWORD Eax)
    {
        cpuid_regs Regs;
        __cpuid((int*)&Regs, Eax);
        std::string Str;
        Str += SplitIntoChars(Regs.Eax);
        Str += SplitIntoChars(Regs.Ebx);
        Str += SplitIntoChars(Regs.Ecx);
        Str += SplitIntoChars(Regs.Edx);
        return Str;
    }

    std::string GetCpuVendorString()
    {
        std::string VendorString;
        cpuid_regs Regs;
        __cpuid((int*)&Regs, 0x80000000);
        if (Regs.Eax >= 0x80000004)
        {
            VendorString =
                GetCpuVendorSubstring(0x80000002) +
                GetCpuVendorSubstring(0x80000003) +
                GetCpuVendorSubstring(0x80000004)
                ;
        }
        return VendorString;
    }

//int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int fShow)
//{
    //UNREFERENCED_PARAMETER(hInstance);
    //UNREFERENCED_PARAMETER(hPrevInstance);
    //UNREFERENCED_PARAMETER(pCmdLine);
    //UNREFERENCED_PARAMETER(fShow);

    //MessageBoxA(
    //    NULL,
    //    GetCpuVendorString().c_str(),
    //    "INFO - CPUID Vendor String",
    //    MB_ICONINFORMATION
    //);
    //return EXIT_SUCCESS;
//}