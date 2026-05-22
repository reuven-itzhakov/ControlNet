#include "MessageHandlers.h"
#include "UIUtils.h"
#include "FileUtils.h"
#include "UnicodeUtils.h"
#include "json.hpp"
#include <fstream>
#include <algorithm>

using json = nlohmann::json;

void HandleLeftPaneClick(HWND hWnd, int selectedIndex) {
    try {
        HWND hRightPane = GetDlgItem(hWnd, 2);
        if (!hRightPane || !IsWindow(hRightPane)) return;
        
        if (selectedIndex != -1 && selectedIndex < g_leftPaneRows.size()) {
            // Clear right table
            try {
                ListView_DeleteAllItems(hRightPane);
            }
            catch (const std::exception& e) {
                // ListView clear failed - continue anyway
            }
            
            // Read and parse JSON file
            std::ifstream in(g_leftPaneRows[selectedIndex].filePath);
            g_rightPaneRows.clear();
            
            if (in) {
                try {
                    json j;
                    in >> j;
                    
                    for (auto it = j.begin(); it != j.end(); ++it) {
                        try {
                            std::string processName = it.key();
                            // Use improved Unicode conversion
                            std::wstring processW = UnicodeUtils::SafeUTF8ToWString(processName);
                            
                            if (it.value().is_object()) {
                                for (auto innerIt = it.value().begin(); innerIt != it.value().end(); ++innerIt) {
                                    try {
                                        std::string timestampStr = innerIt.key();
                                        std::string text;
                                        
                                        // Handle different value types safely
                                        if (innerIt.value().is_string()) {
                                            text = innerIt.value().get<std::string>();
                                        }
                                        else if (innerIt.value().is_number()) {
                                            text = std::to_string(innerIt.value().get<double>());
                                        }
                                        else {
                                            text = innerIt.value().dump();
                                        }
                                        
                                        // Use improved Unicode conversion for text display
                                        std::wstring textW = UnicodeUtils::SafeUTF8ToWString(text);
                                        std::wstring timestampW = formatTimestamp(timestampStr);
                                        
                                        // Only add if Unicode conversion was successful
                                        if (!processW.empty() && !textW.empty()) {
                                            g_rightPaneRows.push_back({ processW, textW, timestampW });
                                        }
                                    }
                                    catch (const std::exception& e) {
                                        // Processing this inner item failed, skip it
                                        continue;
                                    }
                                }
                            }
                        }
                        catch (const std::exception& e) {
                            // Processing this process entry failed, skip it
                            continue;
                        }
                    }
                } catch (const std::exception& e) {
                    // JSON parsing completely failed
                }
            }
            
            // Sort if needed
            if (g_rightPaneSortColumn != -1) {
                try {
                    std::sort(g_rightPaneRows.begin(), g_rightPaneRows.end(), [](const RightPaneRow& a, const RightPaneRow& b) {
                        int col = g_rightPaneSortColumn;
                        bool asc = g_rightPaneSortAscending;
                        switch (col) {
                            case 0: return asc ? a.process < b.process : a.process > b.process;
                            case 1: return asc ? a.text < b.text : a.text > b.text;
                            case 2: return asc ? a.time < b.time : a.time > b.time;
                            default: return true;
                        }
                    });
                }
                catch (const std::exception& e) {
                    // Sorting failed - continue with unsorted data
                }
            }
            
            // Repopulate right pane
            try {
                int rowIndex = 0;
                for (const auto& row : g_rightPaneRows) {
                    try {
                        LVITEM lvi = { 0 };
                        lvi.mask = LVIF_TEXT;
                        lvi.iItem = rowIndex;
                        lvi.iSubItem = 0;
                        lvi.pszText = (LPWSTR)row.process.c_str();
                        int idx = ListView_InsertItem(hRightPane, &lvi);
                        
                        if (idx != -1) {
                            ListView_SetItemText(hRightPane, idx, 1, (LPWSTR)row.text.c_str());
                            ListView_SetItemText(hRightPane, idx, 2, (LPWSTR)row.time.c_str());
                        }
                        ++rowIndex;
                    }
                    catch (const std::exception& e) {
                        // Adding this row failed, continue with next
                        continue;
                    }
                }
            }
            catch (const std::exception& e) {
                // ListView population failed
            }
        }
    }
    catch (const std::exception& e) {
        // General HandleLeftPaneClick failure
    }
}

void HandleColumnClick(HWND hWnd, LPNMHDR pnmh, LPARAM lParam) {
    try {
        if (pnmh->hwndFrom == GetDlgItem(hWnd, 1)) { // Left pane
            try {
                NMLISTVIEW* pnmv = (NMLISTVIEW*)lParam;
                int col = pnmv->iSubItem;
                if (g_leftPaneSortColumn == col) {
                    g_leftPaneSortAscending = !g_leftPaneSortAscending;
                } else {
                    g_leftPaneSortColumn = col;
                    g_leftPaneSortAscending = true;
                }
                HWND hLeftPane = GetDlgItem(hWnd, 1);
                if (hLeftPane) {
                    loadLogs(hLeftPane);
                }
            }
            catch (const std::exception& e) {
                // Left pane column click handling failed
            }
        } else if (pnmh->hwndFrom == GetDlgItem(hWnd, 2)) { // Right pane
            try {
                NMLISTVIEW* pnmv = (NMLISTVIEW*)lParam;
                int col = pnmv->iSubItem;
                HandleRightPaneColumnClick(GetDlgItem(hWnd, 2), col);
            }
            catch (const std::exception& e) {
                // Right pane column click handling failed
            }
        }
    }
    catch (const std::exception& e) {
        // General column click handling failed
    }
}

void HandleRightPaneColumnClick(HWND hRightPane, int col) {
    try {
        if (!hRightPane || !IsWindow(hRightPane)) return;
        
        if (g_rightPaneSortColumn == col) {
            g_rightPaneSortAscending = !g_rightPaneSortAscending;
        } else {
            g_rightPaneSortColumn = col;
            g_rightPaneSortAscending = true;
        }
        
        try {
            std::sort(g_rightPaneRows.begin(), g_rightPaneRows.end(), [](const RightPaneRow& a, const RightPaneRow& b) {
                int col = g_rightPaneSortColumn;
                bool asc = g_rightPaneSortAscending;
                switch (col) {
                    case 0: return asc ? a.process < b.process : a.process > b.process;
                    case 1: return asc ? a.text < b.text : a.text > b.text;
                    case 2: return asc ? a.time < b.time : a.time > b.time;
                    default: return true;
                }
            });
        }
        catch (const std::exception& e) {
            // Sorting failed - continue with current order
        }
        
        try {
            ListView_DeleteAllItems(hRightPane);
            int rowIndex = 0;
            for (const auto& row : g_rightPaneRows) {
                try {
                    LVITEM lvi = { 0 };
                    lvi.mask = LVIF_TEXT;
                    lvi.iItem = rowIndex;
                    lvi.iSubItem = 0;
                    lvi.pszText = (LPWSTR)row.process.c_str();
                    int idx = ListView_InsertItem(hRightPane, &lvi);
                    
                    if (idx != -1) {
                        ListView_SetItemText(hRightPane, idx, 1, (LPWSTR)row.text.c_str());
                        ListView_SetItemText(hRightPane, idx, 2, (LPWSTR)row.time.c_str());
                    }
                    ++rowIndex;
                }
                catch (const std::exception& e) {
                    // Adding this row failed, continue
                    continue;
                }
            }
        }
        catch (const std::exception& e) {
            // ListView operations failed
        }
    }
    catch (const std::exception& e) {
        // General right pane column click handling failed
    }
}