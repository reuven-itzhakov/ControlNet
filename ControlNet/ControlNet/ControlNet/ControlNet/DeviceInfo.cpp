#include "DeviceInfo.h"
#include "DeviceUtils.h"
#include "Utils.h"
#include "firebase/app.h"
#include "firebase/firestore.h"
#include <windows.h>
#include <iphlpapi.h>
#include <sstream>
#include <iomanip>
#include <memory>
#include <algorithm>

#pragma comment(lib, "iphlpapi.lib")

extern firebase::App* g_firebaseApp;

DeviceInformation CollectDeviceInformation() {
    DeviceInformation info = {};
    
    try {
        // Basic System Info
        info.computerName = GetComputerNameInfo();
        info.userName = GetUserNameInfo();
        info.systemUptime = GetSystemUptimeInfo();
        info.windowsVersion = GetWindowsVersion();
        info.architecture = GetSystemArchitecture();
        
        // Hardware Info
        info.cpuName = GetCPUInformation();
        info.cpuCores = GetCPUCores();
        info.cpuSpeed = GetCPUSpeed();
        info.totalMemory = GetTotalMemoryInfo();
        info.availableMemory = GetAvailableMemoryInfo();
        
        // Network Info
        info.primaryMacAddress = GetPrimaryMACAddress();
        info.primaryIPAddress = GetPrimaryIPAddress();
        info.allIPAddresses = GetAllIPAddresses();
        
        // Software Info (this might take longer)
        info.installedSoftware = GetInstalledSoftware();
        
        // Timestamps
        info.lastUpdated = getCurrentDate();
        if (info.firstSeen.empty()) {
            info.firstSeen = getCurrentDate();
        }
    }
    catch (const std::exception& e) {
        // Handle errors gracefully - return partial info
    }
    
    return info;
}

std::string GetComputerNameInfo() {
    try {
        char computer[256] = { 0 };
        DWORD size = 256;
        if (GetComputerNameA(computer, &size)) {
            return std::string(computer);
        }
    }
    catch (const std::exception& e) {
        // Handle error
    }
    return "Unknown";
}

std::string GetUserNameInfo() {
    try {
        char username[256] = { 0 };
        DWORD size = 256;
        if (GetUserNameA(username, &size)) {
            return std::string(username);
        }
    }
    catch (const std::exception& e) {
        // Handle error
    }
    return "Unknown";
}

std::string GetSystemUptimeInfo() {
    try {
        DWORD uptime = GetTickCount();
        return FormatUptime(uptime);
    }
    catch (const std::exception& e) {
        return "Unknown";
    }
}

std::string FormatUptime(DWORD uptimeMs) {
    DWORD seconds = uptimeMs / 1000;
    DWORD minutes = seconds / 60;
    DWORD hours = minutes / 60;
    DWORD days = hours / 24;
    
    std::stringstream ss;
    if (days > 0) {
        ss << days << " days, ";
    }
    ss << (hours % 24) << "h " << (minutes % 60) << "m " << (seconds % 60) << "s";
    return ss.str();
}

std::string GetCPUInformation() {
    try {
        HKEY hKey;
        const char* regPath = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
        
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char cpuName[256] = { 0 };
            DWORD dataSize = sizeof(cpuName);
            DWORD dataType;
            
            if (RegQueryValueExA(hKey, "ProcessorNameString", NULL, &dataType, 
                                (LPBYTE)cpuName, &dataSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return std::string(cpuName);
            }
            RegCloseKey(hKey);
        }
    }
    catch (const std::exception& e) {
        // Handle error
    }
    return "Unknown CPU";
}

std::string GetCPUCores() {
    try {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return std::to_string(sysInfo.dwNumberOfProcessors);
    }
    catch (const std::exception& e) {
        return "Unknown";
    }
}

std::string GetCPUSpeed() {
    try {
        HKEY hKey;
        const char* regPath = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
        
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD speed = 0;
            DWORD dataSize = sizeof(speed);
            DWORD dataType;
            
            if (RegQueryValueExA(hKey, "~MHz", NULL, &dataType, 
                                (LPBYTE)&speed, &dataSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return std::to_string(speed) + " MHz";
            }
            RegCloseKey(hKey);
        }
    }
    catch (const std::exception& e) {
        // Handle error
    }
    return "Unknown";
}

std::string GetTotalMemoryInfo() {
    try {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo)) {
            return FormatMemorySize(memInfo.ullTotalPhys);
        }
    }
    catch (const std::exception& e) {
        return "Unknown";
    }
    return "Unknown";
}

std::string GetAvailableMemoryInfo() {
    try {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo)) {
            return FormatMemorySize(memInfo.ullAvailPhys);
        }
    }
    catch (const std::exception& e) {
        return "Unknown";
    }
    return "Unknown";
}

std::string FormatMemorySize(DWORDLONG bytes) {
    double gb = bytes / (1024.0 * 1024.0 * 1024.0);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << gb << " GB";
    return ss.str();
}

std::string GetPrimaryMACAddress() {
    try {
        // Get the size needed for adapter info
        DWORD dwSize = 0;
        DWORD dwRetVal = GetAdaptersInfo(NULL, &dwSize);
        
        if (dwRetVal != ERROR_BUFFER_OVERFLOW) {
            return "";
        }
        
        // Allocate memory for adapter info
        std::unique_ptr<IP_ADAPTER_INFO> pAdapterInfo(
            reinterpret_cast<IP_ADAPTER_INFO*>(malloc(dwSize)));
        
        if (!pAdapterInfo) {
            return "";
        }
        
        // Get adapter info
        dwRetVal = GetAdaptersInfo(pAdapterInfo.get(), &dwSize);
        if (dwRetVal != NO_ERROR) {
            return "";
        }
        
        // Find the first active ethernet adapter
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo.get();
        while (pAdapter) {
            // Skip loopback and inactive adapters
            if (pAdapter->Type == MIB_IF_TYPE_ETHERNET && 
                pAdapter->AddressLength == 6) {
                
                // Convert MAC address to string format
                std::stringstream ss;
                for (DWORD i = 0; i < pAdapter->AddressLength; i++) {
                    if (i > 0) ss << ":";
                    ss << std::hex << std::setw(2) << std::setfill('0') 
                       << static_cast<int>(pAdapter->Address[i]);
                }
                return ss.str();
            }
            pAdapter = pAdapter->Next;
        }
    }
    catch (const std::exception& e) {
        return "";
    }
    return "Unknown";
}

std::string GetPrimaryIPAddress() {
    try {
        DWORD dwSize = 0;
        GetAdaptersInfo(NULL, &dwSize);
        
        std::unique_ptr<IP_ADAPTER_INFO> pAdapterInfo(
            reinterpret_cast<IP_ADAPTER_INFO*>(malloc(dwSize)));
        
        if (pAdapterInfo && GetAdaptersInfo(pAdapterInfo.get(), &dwSize) == NO_ERROR) {
            PIP_ADAPTER_INFO pAdapter = pAdapterInfo.get();
            while (pAdapter) {
                if (pAdapter->Type == MIB_IF_TYPE_ETHERNET && 
                    strcmp(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0") != 0) {
                    return std::string(pAdapter->IpAddressList.IpAddress.String);
                }
                pAdapter = pAdapter->Next;
            }
        }
    }
    catch (const std::exception& e) {
        return "Unknown";
    }
    return "Unknown";
}

std::vector<std::string> GetAllIPAddresses() {
    std::vector<std::string> addresses;
    try {
        DWORD dwSize = 0;
        GetAdaptersInfo(NULL, &dwSize);
        
        std::unique_ptr<IP_ADAPTER_INFO> pAdapterInfo(
            reinterpret_cast<IP_ADAPTER_INFO*>(malloc(dwSize)));
        
        if (pAdapterInfo && GetAdaptersInfo(pAdapterInfo.get(), &dwSize) == NO_ERROR) {
            PIP_ADAPTER_INFO pAdapter = pAdapterInfo.get();
            while (pAdapter) {
                PIP_ADDR_STRING pAddress = &pAdapter->IpAddressList;
                while (pAddress) {
                    if (strcmp(pAddress->IpAddress.String, "0.0.0.0") != 0) {
                        addresses.push_back(std::string(pAddress->IpAddress.String));
                    }
                    pAddress = pAddress->Next;
                }
                pAdapter = pAdapter->Next;
            }
        }
    }
    catch (const std::exception& e) {
        // Handle error
    }
    return addresses;
}

std::vector<std::string> GetInstalledSoftware() {
    std::vector<std::string> software;
    try {
        HKEY hKey;
        const char* regPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
        
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD index = 0;
            char subKeyName[256];
            DWORD subKeyNameSize = sizeof(subKeyName);
            
            while (RegEnumKeyExA(hKey, index++, subKeyName, &subKeyNameSize, 
                                NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                
                HKEY hSubKey;
                if (RegOpenKeyExA(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                    char displayName[256] = {0};
                    DWORD displayNameSize = sizeof(displayName);
                    DWORD dataType;
                    
                    if (RegQueryValueExA(hSubKey, "DisplayName", NULL, &dataType, 
                                        (LPBYTE)displayName, &displayNameSize) == ERROR_SUCCESS) {
                        std::string name(displayName);
                        if (!name.empty() && name.length() > 2) {
                            software.push_back(name);
                        }
                    }
                    RegCloseKey(hSubKey);
                }
                subKeyNameSize = sizeof(subKeyName);
            }
            RegCloseKey(hKey);
        }
        
        // Limit to first 100 entries to avoid huge lists
        if (software.size() > 100) {
            software.resize(100);
        }
    }
    catch (const std::exception& e) {
        // Handle error
    }
    return software;
}

std::string GetWindowsVersion() {
    try {
        // Use registry approach instead of deprecated GetVersionEx
        HKEY hKey;
        const char* regPath = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";
        
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char productName[256] = {0};
            DWORD productNameSize = sizeof(productName);
            DWORD dataType;
            
            std::string version;
            
            // Get ProductName
            if (RegQueryValueExA(hKey, "ProductName", NULL, &dataType, 
                                (LPBYTE)productName, &productNameSize) == ERROR_SUCCESS) {
                version = std::string(productName);
            }
            
            // Get CurrentVersion
            char currentVersion[64] = {0};
            DWORD currentVersionSize = sizeof(currentVersion);
            if (RegQueryValueExA(hKey, "CurrentVersion", NULL, &dataType, 
                                (LPBYTE)currentVersion, &currentVersionSize) == ERROR_SUCCESS) {
                if (!version.empty()) version += " ";
                version += std::string(currentVersion);
            }
            
            // Get CurrentBuild
            char currentBuild[64] = {0};
            DWORD currentBuildSize = sizeof(currentBuild);
            if (RegQueryValueExA(hKey, "CurrentBuild", NULL, &dataType, 
                                (LPBYTE)currentBuild, &currentBuildSize) == ERROR_SUCCESS) {
                if (!version.empty()) version += " Build ";
                version += std::string(currentBuild);
            }
            
            RegCloseKey(hKey);
            return version.empty() ? "Unknown" : version;
        }
    }
    catch (const std::exception& e) {
        // Handle error
    }
    return "Unknown";
}

std::string GetSystemArchitecture() {
    try {
        SYSTEM_INFO sysInfo;
        GetNativeSystemInfo(&sysInfo);
        
        switch (sysInfo.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_AMD64:
                return "x64";
            case PROCESSOR_ARCHITECTURE_INTEL:
                return "x86";
            case PROCESSOR_ARCHITECTURE_ARM:
                return "ARM";
            case PROCESSOR_ARCHITECTURE_ARM64:
                return "ARM64";
            default:
                return "Unknown";
        }
    }
    catch (const std::exception& e) {
        return "Unknown";
    }
}

// Helper function to sanitize software names for Firestore
std::string SanitizeSoftwareName(const std::string& name) {
    std::string sanitized = name;
    
    // Remove null characters and other control characters (0-31 and 127)
    sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(), 
        [](char c) { return c >= 0 && c <= 31 || c == 127; }), sanitized.end());
    
    // Limit length to reasonable size
    if (sanitized.length() > 100) {
        sanitized = sanitized.substr(0, 100);
    }
    
    // Remove leading/trailing whitespace
    size_t start = sanitized.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = sanitized.find_last_not_of(" \t\r\n");
    sanitized = sanitized.substr(start, end - start + 1);
    
    return sanitized;
}

firebase::firestore::MapFieldValue DeviceInfoToFirestoreMap(const DeviceInformation& info) {
    firebase::firestore::MapFieldValue deviceMap;
    
    try {
        // Basic System Info
        deviceMap["computerName"] = firebase::firestore::FieldValue::String(info.computerName.empty() ? "Unknown" : info.computerName);
        deviceMap["userName"] = firebase::firestore::FieldValue::String(info.userName.empty() ? "Unknown" : info.userName);
        deviceMap["systemUptime"] = firebase::firestore::FieldValue::String(info.systemUptime.empty() ? "Unknown" : info.systemUptime);
        deviceMap["windowsVersion"] = firebase::firestore::FieldValue::String(info.windowsVersion.empty() ? "Unknown" : info.windowsVersion);
        deviceMap["architecture"] = firebase::firestore::FieldValue::String(info.architecture.empty() ? "Unknown" : info.architecture);
        
        // Hardware Info
        deviceMap["cpuName"] = firebase::firestore::FieldValue::String(info.cpuName.empty() ? "Unknown" : info.cpuName);
        deviceMap["cpuCores"] = firebase::firestore::FieldValue::String(info.cpuCores.empty() ? "Unknown" : info.cpuCores);
        deviceMap["cpuSpeed"] = firebase::firestore::FieldValue::String(info.cpuSpeed.empty() ? "Unknown" : info.cpuSpeed);
        deviceMap["totalMemory"] = firebase::firestore::FieldValue::String(info.totalMemory.empty() ? "Unknown" : info.totalMemory);
        deviceMap["availableMemory"] = firebase::firestore::FieldValue::String(info.availableMemory.empty() ? "Unknown" : info.availableMemory);
        
        // Network Info
        deviceMap["primaryMacAddress"] = firebase::firestore::FieldValue::String(info.primaryMacAddress.empty() ? "Unknown" : info.primaryMacAddress);
        deviceMap["primaryIPAddress"] = firebase::firestore::FieldValue::String(info.primaryIPAddress.empty() ? "Unknown" : info.primaryIPAddress);
        
        // Convert IP addresses vector to Firestore array
        std::vector<firebase::firestore::FieldValue> ipArray;
        for (const auto& ip : info.allIPAddresses) {
            if (!ip.empty()) {
                ipArray.push_back(firebase::firestore::FieldValue::String(ip));
            }
        }
        deviceMap["allIPAddresses"] = firebase::firestore::FieldValue::Array(ipArray);
        
        // Convert software list to Firestore array with proper sanitization
        std::vector<firebase::firestore::FieldValue> softwareArray;
        int count = 0;
        for (const auto& sw : info.installedSoftware) {
            if (count >= 25) break; // Limit to 25 entries to keep document size reasonable
            
            std::string sanitized = SanitizeSoftwareName(sw);
            if (!sanitized.empty() && sanitized.length() >= 3) {
                softwareArray.push_back(firebase::firestore::FieldValue::String(sanitized));
                count++;
            }
        }
        
        // If no software was found, add a placeholder
        if (softwareArray.empty()) {
            softwareArray.push_back(firebase::firestore::FieldValue::String("No software detected"));
        }
        
        deviceMap["installedSoftware"] = firebase::firestore::FieldValue::Array(softwareArray);
        
        // Timestamps
        deviceMap["lastUpdated"] = firebase::firestore::FieldValue::String(info.lastUpdated.empty() ? "Unknown" : info.lastUpdated);
        deviceMap["firstSeen"] = firebase::firestore::FieldValue::String(info.firstSeen.empty() ? "Unknown" : info.firstSeen);
    }
    catch (const std::exception& e) {
        // Handle conversion errors gracefully
    }
    
    return deviceMap;
}

bool RegisterDeviceInfo() {
    try {
        if (!g_firebaseApp) return false;
        
        auto firestore = firebase::firestore::Firestore::GetInstance(g_firebaseApp);
        if (!firestore) return false;
        
        std::string deviceGuid = GetDeviceMachineGUID();
        if (deviceGuid.empty()) return false;
        
        // Collect device information
        DeviceInformation info = CollectDeviceInformation();
        
        // Convert to Firestore map
        firebase::firestore::MapFieldValue deviceData = DeviceInfoToFirestoreMap(info);
        
        // Ensure totalSequences is initialized to 0 if missing
        if (deviceData.find("totalSequences") == deviceData.end()) {
            deviceData["totalSequences"] = firebase::firestore::FieldValue::Integer(0);
        }
        // Ensure todaySequences and todayDate are present
        if (deviceData.find("todayDate") == deviceData.end()) {
            deviceData["todayDate"] = firebase::firestore::FieldValue::String(getCurrentDate());
        }
        if (deviceData.find("todaySequences") == deviceData.end()) {
            deviceData["todaySequences"] = firebase::firestore::FieldValue::Integer(0);
        }
        
        // Store in devices/{GUID} document
        auto deviceDoc = firestore->Collection("devices").Document(deviceGuid);
        
        auto future = deviceDoc.Set(deviceData, firebase::firestore::SetOptions::Merge());
        
        // Wait for completion
        int waited = 0;
        const int maxWaitMs = 10000; // 10 seconds
        while (future.status() == firebase::kFutureStatusPending && waited < maxWaitMs) {
            Sleep(50);
            waited += 50;
        }
        
        if (future.status() != firebase::kFutureStatusComplete) {
            return false;
        }
        
        return future.error() == firebase::firestore::kErrorOk;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool UpdateDeviceInfo() {
    // Same as RegisterDeviceInfo for now - could add logic to check if update is needed
    return RegisterDeviceInfo();
}