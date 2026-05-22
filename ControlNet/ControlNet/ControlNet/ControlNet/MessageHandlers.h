#pragma once
#include <windows.h>
#include <commctrl.h>

// Message handling utilities
void HandleLeftPaneClick(HWND hWnd, int selectedIndex);
void HandleColumnClick(HWND hWnd, LPNMHDR pnmh, LPARAM lParam);
void HandleRightPaneColumnClick(HWND hRightPane, int col);