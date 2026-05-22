#pragma once
#include <windows.h>
#include <string>
#include "HotkeySettings.h" // central hotkey declarations
#include "AuthService.h"
#include "firebase/auth.h"

INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
