#include "HotkeySettings.h"
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

// Single global definitions
UINT g_hotkeyModifiers = MOD_CONTROL | MOD_ALT;
UINT g_hotkeyVk        = VK_F9;

static const char* kSettingsFile = "settings.json";

void UpdateHotkey(HWND) {
    try {
        UnregisterHotKey(NULL, 1);
        RegisterHotKey(NULL, 1, g_hotkeyModifiers, g_hotkeyVk);
    }
    catch (const std::exception& e) {
        // Hotkey update failed - ignore
    }
    catch (...) {
        // Unknown error in hotkey update
    }
}

void SaveHotkeySettings() {
    try {
        json root;
        // Preserve other existing settings if present
        std::ifstream in(kSettingsFile);
        if (in) {
            try { 
                in >> root; 
            } 
            catch (const std::exception& e) { 
                root = json::object(); 
            }
        } else {
            root = json::object();
        }
        
        root["app_settings"]["hotkey_modifiers"] = g_hotkeyModifiers;
        root["app_settings"]["hotkey_key"] = g_hotkeyVk;
        
        std::ofstream out(kSettingsFile);
        if (out) {
            try {
                out << root.dump(2);
            }
            catch (const std::exception& e) {
                // JSON serialization failed
            }
        }
    }
    catch (const std::exception& e) {
        // Settings save failed - ignore
    }
}

void LoadHotkeySettings() {
    try {
        std::ifstream in(kSettingsFile);
        if (!in) return;
        
        try {
            json root; 
            in >> root;
            
            if (root.contains("app_settings")) {
                auto& s = root["app_settings"];
                if (s.contains("hotkey_modifiers")) {
                    try {
                        g_hotkeyModifiers = s["hotkey_modifiers"].get<UINT>();
                    }
                    catch (const std::exception& e) {
                        // Modifier parsing failed - keep default
                    }
                }
                if (s.contains("hotkey_key")) {
                    try {
                        g_hotkeyVk = s["hotkey_key"].get<UINT>();
                    }
                    catch (const std::exception& e) {
                        // Key parsing failed - keep default
                    }
                }
            }
        } 
        catch (const std::exception& e) {
            // JSON parsing failed - use defaults
        }
    }
    catch (const std::exception& e) {
        // File access failed - use defaults
    }
}

std::wstring GetHotkeyDisplayString() {
    try {
        std::wstring parts;
        if (g_hotkeyModifiers & MOD_CONTROL) parts += L"Ctrl + ";
        if (g_hotkeyModifiers & MOD_ALT)      parts += L"Alt + ";
        if (g_hotkeyModifiers & MOD_SHIFT)    parts += L"Shift + ";
        if (g_hotkeyModifiers & MOD_WIN)      parts += L"Win + ";
        
        if (g_hotkeyVk >= VK_F1 && g_hotkeyVk <= VK_F24) {
            wchar_t buf[8]; 
            swprintf(buf, 8, L"F%d", g_hotkeyVk - VK_F1 + 1); 
            parts += buf;
        } else {
            // Fallback to hex code
            wchar_t buf[8]; 
            swprintf(buf, 8, L"0x%02X", g_hotkeyVk); 
            parts += buf;
        }
        return parts;
    }
    catch (const std::exception& e) {
        return L"Unknown";
    }
}
