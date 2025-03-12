#include "../../headers/core/ProcessUtil.h"
#include <psapi.h>
#include <cstring>

namespace CS2Assist {

    DWORD ProcessUtil::GetProcessPid(const std::wstring& processName) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }

        PROCESSENTRY32W entry = { sizeof(PROCESSENTRY32W) };
        if (!Process32FirstW(snapshot, &entry)) {
            CloseHandle(snapshot);
            return 0;
        }

        do {
            if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &entry));

        CloseHandle(snapshot);
        return 0;
    }

    HMODULE ProcessUtil::GetModuleHandle(HANDLE processHandle, const std::string& moduleName) {
        HMODULE modules[999] = { 0 };
        DWORD bytesNeeded = 0;

        if (!EnumProcessModulesEx(processHandle, modules, sizeof(modules), &bytesNeeded, LIST_MODULES_ALL)) {
            return nullptr;
        }

        size_t moduleCount = bytesNeeded / sizeof(HMODULE);
        char baseName[100] = { 0 };

        for (size_t i = 0; i < moduleCount; ++i) {
            if (GetModuleBaseNameA(processHandle, modules[i], baseName, sizeof(baseName)) > 0) {
                if (_stricmp(baseName, moduleName.c_str()) == 0) {
                    return modules[i];
                }
            }
        }

        return nullptr;
    }

    bool ProcessUtil::ReadMemory(HANDLE processHandle, LPCVOID address, LPVOID buffer, SIZE_T size) {
        SIZE_T bytesRead = 0;
        if (!ReadProcessMemory(processHandle, address, buffer, size, &bytesRead) || bytesRead != size) {
            return false;
        }
        return true;
    }

    bool ProcessUtil::WriteMemory(HANDLE processHandle, LPVOID address, LPCVOID buffer, SIZE_T size) {
        SIZE_T bytesWritten = 0;
        if (!WriteProcessMemory(processHandle, address, buffer, size, &bytesWritten) || bytesWritten != size) {
            return false;
        }
        return true;
    }

} // namespace CS2Assist