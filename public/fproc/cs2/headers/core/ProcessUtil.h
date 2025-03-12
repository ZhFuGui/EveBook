#ifndef CS2ASSIST_PROCESS_UTIL_H
#define CS2ASSIST_PROCESS_UTIL_H

#include <Windows.h>
#include <TlHelp32.h>
#include <string>

namespace CS2Assist {

    class ProcessUtil {
    public:
        // 获取进程PID
        static DWORD GetProcessPid(const std::wstring& processName);
            
        // 获取进程模块句柄
        static HMODULE GetModuleHandle(HANDLE processHandle, const std::string& moduleName);

        // 读取内存
        static bool ReadMemory(HANDLE processHandle, LPCVOID address, LPVOID buffer, SIZE_T size);

        // 写入内存
        static bool WriteMemory(HANDLE processHandle, LPVOID address, LPCVOID buffer, SIZE_T size);

    private:
        ProcessUtil() = delete; // 禁止实例化，仅提供静态方法
    };

} // namespace CS2Assist

#endif // CS2ASSIST_PROCESS_UTIL_H