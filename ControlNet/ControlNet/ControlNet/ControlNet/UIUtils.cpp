#include "UIUtils.h"
#include "FileUtils.h"
#include "Utils.h"
#include "ScreenCapture.h"
#include "json.hpp"
#include <fstream>
#include <algorithm>

using json = nlohmann::json;

// Global variables for sorting and data
std::vector<std::string> g_jsonFiles;
std::vector<LeftPaneRow> g_leftPaneRows;
std::vector<RightPaneRow> g_rightPaneRows;
int g_leftPaneSortColumn = -1;
bool g_leftPaneSortAscending = true;
int g_rightPaneSortColumn = 2; // Set default sort to time (column 2), ascending
bool g_rightPaneSortAscending = true;

void AddLeftPaneRow(HWND hLeftPane, int rowIndex, const LeftPaneRow& row) {
    try {
        if (!hLeftPane || !IsWindow(hLeftPane)) return;
        
        LVITEM lvi = { 0 };
        lvi.mask = LVIF_TEXT;
        lvi.iItem = rowIndex;
        lvi.iSubItem = 0;
        lvi.pszText = (LPWSTR)row.user.c_str();
        int idx = ListView_InsertItem(hLeftPane, &lvi);
        
        if (idx != -1) {
            ListView_SetItemText(hLeftPane, idx, 1, (LPWSTR)row.keystrokes.c_str());
            ListView_SetItemText(hLeftPane, idx, 2, (LPWSTR)row.screenshots.c_str());
            ListView_SetItemText(hLeftPane, idx, 3, (LPWSTR)row.date.c_str());
            ListView_SetItemText(hLeftPane, idx, 4, (LPWSTR)row.size.c_str());
        }
    }
    catch (const std::exception& e) {
        // ListView operation failed - ignore this row
    }
}

void loadLogs(HWND hLeftPane) {
    try {
        if (!hLeftPane || !IsWindow(hLeftPane)) return;
        
        std::string deviceFolder = getDeviceGUID();
        std::wstring deviceFolderW = std::wstring(deviceFolder.begin(), deviceFolder.end());
        
        try {
            g_jsonFiles = GetJsonFiles(deviceFolder);
        }
        catch (const std::exception& e) {
            // GetJsonFiles failed - use empty list
            g_jsonFiles.clear();
        }
        
        g_leftPaneRows.clear();
        
        for (const auto& filePath : g_jsonFiles) {
            try {
                // filePath is now in format: "{deviceFolder}\{date}\{date}.json"
                // Extract date from the folder name in the path
                std::string relativePath = filePath;
                if (filePath.find(deviceFolder) == 0) {
                    relativePath = filePath.substr(deviceFolder.length());
                    if (!relativePath.empty() && (relativePath[0] == '\\' || relativePath[0] == '/')) {
                        relativePath = relativePath.substr(1);
                    }
                }
                
                // Extract date from the first part of the path (folder name)
                std::string date;
                size_t firstSlash = relativePath.find("\\");
                if (firstSlash != std::string::npos) {
                    date = relativePath.substr(0, firstSlash);
                } else {
                    // Fallback: extract from filename
                    std::string filename = relativePath.substr(relativePath.find_last_of("\\") + 1);
                    date = filename.substr(0, filename.find_last_of("."));
                }
                std::wstring dateW = std::wstring(date.begin(), date.end());
                
                // Open file once for both size and JSON parsing
                std::ifstream inJson(filePath, std::ifstream::binary | std::ifstream::ate);
                std::streamsize size = inJson.tellg();
                std::wstring sizeW = formatSize(size);
                inJson.seekg(0);
                int keystrokeCount = 0;
                
                if (inJson) {
                    try {
                        json j;
                        inJson >> j;
                        for (auto it = j.begin(); it != j.end(); ++it) {
                            if (it.value().is_object()) {
                                keystrokeCount += static_cast<int>(it.value().size());
                            }
                        }
                    } catch (const std::exception& e) {
                        // JSON parsing failed, default to 0 keystrokes
                        keystrokeCount = 0;
                    }
                }
                
                // Count screenshots for this date
                int screenshotCount = CountScreenshotsForDate(deviceFolder, date);
                
                std::wstring keystrokeW = std::to_wstring(keystrokeCount);
                std::wstring screenshotW = std::to_wstring(screenshotCount);
                g_leftPaneRows.push_back({deviceFolderW, keystrokeW, screenshotW, dateW, sizeW, filePath});
            }
            catch (const std::exception& e) {
                // Processing this file failed, skip it
                continue;
            }
        }
        
        // Sort if needed
        if (g_leftPaneSortColumn != -1) {
            try {
                std::sort(g_leftPaneRows.begin(), g_leftPaneRows.end(), [](const LeftPaneRow& a, const LeftPaneRow& b) {
                    int col = g_leftPaneSortColumn;
                    bool asc = g_leftPaneSortAscending;
                    switch (col) {
                        case 0: return asc ? a.user < b.user : a.user > b.user;
                        case 1: return asc ? a.keystrokes < b.keystrokes : a.keystrokes > b.keystrokes;
                        case 2: return asc ? a.screenshots < b.screenshots : a.screenshots > b.screenshots;
                        case 3: return asc ? a.date < b.date : a.date > b.date;
                        case 4: return asc ? a.size < b.size : a.size > b.size;
                        default: return true;
                    }
                });
            }
            catch (const std::exception& e) {
                // Sorting failed - continue with unsorted data
            }
        }
        
        // Clear and repopulate ListView
        try {
            ListView_DeleteAllItems(hLeftPane);
            int rowIndex = 0;
            for (const auto& row : g_leftPaneRows) {
                AddLeftPaneRow(hLeftPane, rowIndex, row);
                ++rowIndex;
            }
        }
        catch (const std::exception& e) {
            // ListView operations failed
        }
    }
    catch (const std::exception& e) {
        // General loadLogs failure - leave UI in current state
    }
}