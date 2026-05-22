#include "FileUtils.h"
#include <windows.h>
#include <ctime>

std::vector<std::string> GetJsonFiles(const std::string& folder) {
    std::vector<std::string> files;
    
    // Search for date subdirectories (YYYY-MM-DD pattern)
    std::string searchPath = folder + "\\*";
    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &fd);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // Check if it's a directory and not "." or ".."
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                strcmp(fd.cFileName, ".") != 0 &&
                strcmp(fd.cFileName, "..") != 0) {
                
                // Look for JSON file inside the date folder
                std::string dateFolder = folder + "\\" + fd.cFileName;
                std::string jsonFilePath = dateFolder + "\\" + fd.cFileName + ".json";
                
                // Check if the JSON file exists
                WIN32_FIND_DATAA jsonFd;
                HANDLE hJsonFind = FindFirstFileA(jsonFilePath.c_str(), &jsonFd);
                if (hJsonFind != INVALID_HANDLE_VALUE) {
                    files.push_back(jsonFilePath);
                    FindClose(hJsonFind);
                }
            }
        } while (FindNextFileA(hFind, &fd));
        FindClose(hFind);
    }
    
    return files;
}

std::wstring formatSize(std::streamsize size) {
    wchar_t buf[32];
    if (size < 1024)
        swprintf(buf, 32, L"%lld B", static_cast<long long>(size));
    else if (size < 1024 * 1024)
        swprintf(buf, 32, L"%.2f KB", size / 1024.0);
    else
        swprintf(buf, 32, L"%.2f MB", size / (1024.0 * 1024.0));
    return buf;
}

std::wstring formatTimestamp(const std::string& timestampStr) {
    time_t timestamp = 0;
    try {
        timestamp = std::stoll(timestampStr);
    } catch (...) {
        return L"";
    }
    struct tm tmStruct;
    localtime_s(&tmStruct, &timestamp);
    wchar_t buf[16];
    wcsftime(buf, sizeof(buf)/sizeof(wchar_t), L"%H:%M:%S", &tmStruct);
    return buf;
}