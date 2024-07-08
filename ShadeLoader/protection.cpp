#include <fstream>
#include <iomanip>
#include <algorithm>
#include <windows.h> 
#include "config.h"
#include <tlhelp32.h>

#if PROTECTION
// Anti-VM and Anti-Debug checks
bool protection() {
    // Check for common VM and debugging artifacts
    const char* vm_artifacts[] = {
        "VMware", "VirtualBox", "vbox", "qemu", "xen", "bochs", "hypervisor", "parallels", "utm",
    };
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);

    // Check number of processors
    if (sys_info.dwNumberOfProcessors < 2) {
        return true;
    }

    // Check for VM-related files in system directories
    char sys_dir[MAX_PATH];
    GetSystemDirectoryA(sys_dir, MAX_PATH);
    for (const auto& artifact : vm_artifacts) {
        std::string file_path = std::string(sys_dir) + "\\" + artifact;
        if (GetFileAttributesA(file_path.c_str()) != INVALID_FILE_ATTRIBUTES) {
            return true;
        }
    }

    // Check for a short uptime
    ULONGLONG uptime = GetTickCount64() / (5000 * 60); // 5000 * 60 = 5 min
    if (uptime < 5) {
        return true;
    }

    // Check for presence of a debugger
    if (IsDebuggerPresent()) {
        return true;
    }
    // check for common process detected
    const char* tools[] = {
        "ollydbg.exe", "x64dbg.exe", "ida64.exe", "idag.exe", "idaw.exe", "idaq.exe", "idau.exe", "scylla.exe", "scylla_x64.exe",
        "protection_id.exe", "windbg.exe", "reshacker.exe"
    };
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe)) {
            do {
                // Convert pe.szExeFile (WCHAR) to char
                char exeFile[MAX_PATH];
                WideCharToMultiByte(CP_ACP, 0, pe.szExeFile, -1, exeFile, MAX_PATH, NULL, NULL);
                for (const auto& tool : tools) {
                    if (_stricmp(exeFile, tool) == 0) {
                        CloseHandle(hSnapshot);
                        return true;
                    }
                }
            } while (Process32Next(hSnapshot, &pe));
        }
        CloseHandle(hSnapshot);
    }

    return false; // No suspicious environment detected
}
#endif
