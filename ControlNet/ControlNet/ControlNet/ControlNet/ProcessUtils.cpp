#include "ProcessUtils.h"
#include <windows.h>
#include <psapi.h>

std::wstring GetActiveProcessName() {
    try {
        HWND hwnd = GetForegroundWindow();
        if (!hwnd) return L"";

        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (!pid) return L"";

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (!hProcess) return L"";

        std::wstring result;
        try {
            WCHAR filename[MAX_PATH] = { 0 };
            if (GetModuleFileNameExW(hProcess, NULL, filename, MAX_PATH)) {
                std::wstring fullPath(filename);
                size_t pos = fullPath.find_last_of(L"\\/");
                if (pos != std::wstring::npos)
                    result = fullPath.substr(pos + 1);
                else
                    result = fullPath;
            }
        }
        catch (const std::exception& e) {
            // GetModuleFileNameEx failed
            result = L"";
        }

        CloseHandle(hProcess);
        return result;
    }
    catch (const std::exception& e) {
        return L"";
    }
}

std::wstring GetActiveWindowTitle() {
    try {
        HWND hwnd = GetForegroundWindow();
        if (!hwnd) return L"";

        WCHAR windowTitle[256] = {0};
        if (GetWindowTextW(hwnd, windowTitle, 256) > 0) {
            return std::wstring(windowTitle);
        }
        return L"";
    }
    catch (const std::exception& e) {
        return L"";
    }
}
