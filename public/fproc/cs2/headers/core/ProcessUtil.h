#ifndef CS2ASSIST_PROCESS_UTIL_H
#define CS2ASSIST_PROCESS_UTIL_H

#include <Windows.h>
#include <TlHelp32.h>
#include <string>

namespace CS2Assist {

    class ProcessUtil {
    public:
        // ��ȡ����PID
        static DWORD GetProcessPid(const std::wstring& processName);
            
        // ��ȡ����ģ����
        static HMODULE GetModuleHandle(HANDLE processHandle, const std::string& moduleName);

        // ��ȡ�ڴ�
        static bool ReadMemory(HANDLE processHandle, LPCVOID address, LPVOID buffer, SIZE_T size);

        // д���ڴ�
        static bool WriteMemory(HANDLE processHandle, LPVOID address, LPCVOID buffer, SIZE_T size);

    private:
        ProcessUtil() = delete; // ��ֹʵ���������ṩ��̬����
    };

} // namespace CS2Assist

#endif // CS2ASSIST_PROCESS_UTIL_H