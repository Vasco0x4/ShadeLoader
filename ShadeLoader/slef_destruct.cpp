#include <windows.h>
#include <iostream>

void delete_executable(const char* executable_path) {
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, GetCurrentProcessId());
    if (hProcess != NULL) {
        WaitForSingleObject(hProcess, INFINITE);
        CloseHandle(hProcess);
    }
    if (DeleteFileA(executable_path)) {
        std::cout << "[+] Executable deleted: " << executable_path << std::endl;
    }
    else {
        std::cerr << "[-] Failed to delete executable: " << executable_path << std::endl;
    }
}
