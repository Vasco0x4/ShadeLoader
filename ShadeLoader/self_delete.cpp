#include <fstream>
#include <iomanip>
#include <algorithm>
#include <windows.h>
#include "self_delete.h"

bool self_delete() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    char batch_path[MAX_PATH];
    GetTempPathA(MAX_PATH, batch_path);
    strcat_s(batch_path, sizeof(batch_path), "00.bat");

    std::ofstream batch_file(batch_path);
    if (batch_file.is_open()) {
        batch_file << ":Repeat" << std::endl;
        batch_file << "del \"" << path << "\"" << std::endl;
        batch_file << "if exist \"" << path << "\" goto Repeat" << std::endl;
        batch_file << "del \"%~f0\"" << std::endl;
        batch_file.close();
    }
    ShellExecuteA(NULL, "open", batch_path, NULL, NULL, SW_HIDE);
    return false;
}