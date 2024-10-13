#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            // Code to initialize the DLL and its resources
            break;

        case DLL_PROCESS_DETACH:
            // Code to clean up the DLL and its resources
            break;

        case DLL_THREAD_ATTACH:
            // Code to initialize a new thread
            break;

        case DLL_THREAD_DETACH:
            // Code to clean up a thread
            break;
    }

    return TRUE;
}
