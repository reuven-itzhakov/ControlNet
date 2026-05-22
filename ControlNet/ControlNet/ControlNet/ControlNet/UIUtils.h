#pragma once
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>

// Data structures for UI
struct LeftPaneRow {
    std::wstring user;
    std::wstring keystrokes;
    std::wstring screenshots;
    std::wstring date;
    std::wstring size;
    std::string filePath;
};

struct RightPaneRow {
    std::wstring process;
    std::wstring text;
    std::wstring time;
};

// UI utility functions
void AddLeftPaneRow(HWND hLeftPane, int rowIndex, const LeftPaneRow& row);
void loadLogs(HWND hLeftPane);

// Global variables for sorting and data
extern std::vector<std::string> g_jsonFiles;
extern std::vector<LeftPaneRow> g_leftPaneRows;
extern std::vector<RightPaneRow> g_rightPaneRows;
extern int g_leftPaneSortColumn;
extern bool g_leftPaneSortAscending;
extern int g_rightPaneSortColumn;
extern bool g_rightPaneSortAscending;