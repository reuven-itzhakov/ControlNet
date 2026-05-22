#pragma once
#include <string>
#include <windows.h>

// Custom message posted to target window when an auth operation completes.
// wParam: 1 = success, 0 = failure
// lParam: (LPARAM)nullptr currently (reserved for extended info)
constexpr UINT WM_AUTH_RESULT = WM_APP + 100;

// Initialize / shutdown auth service (optional hooks, currently no-op)
void AuthService_Init();
void AuthService_Shutdown();

// Begin async sign in. Disables UI should be handled by caller.
// On completion, posts WM_AUTH_RESULT to notifyHwnd.
void AuthService_SignInAsync(const std::string& email, const std::string& password, HWND notifyHwnd);

// Sign out (synchronous)
void AuthService_SignOut();

// Helper getters
bool AuthService_IsSignedIn();
std::string AuthService_CurrentEmail();
