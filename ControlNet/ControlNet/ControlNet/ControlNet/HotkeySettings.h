#pragma once
#include <windows.h>
#include <string>

// Global hotkey settings (single definition in HotkeySettings.cpp)
extern UINT g_hotkeyModifiers; // MOD_* flags
extern UINT g_hotkeyVk;        // Virtual-key (F1..F12 etc.)

// Load / save from settings.json
void LoadHotkeySettings();
void SaveHotkeySettings();

// (Re)register global hotkey (id = 1) with current settings.
// Pass NULL for hWnd; the app uses a thread-global hotkey.
void UpdateHotkey(HWND hWnd = NULL);

// Helper to convert current settings to a human-readable string (e.g. "Ctrl + Alt + F9")
std::wstring GetHotkeyDisplayString();
