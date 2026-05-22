#include "DeviceUtils.h"
#include <windows.h>
#include <string>

std::string GetDeviceMachineGUID() {
    try {
        HKEY hKey = NULL;
        const char* regPath = "SOFTWARE\\Microsoft\\Cryptography";
        const char* valueName = "MachineGuid";
        
        // Open the registry key
        LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hKey);
        if (result != ERROR_SUCCESS) {
            return "";
        }
        
        // Query the value size first
        DWORD dataType = 0;
        DWORD dataSize = 0;
        result = RegQueryValueExA(hKey, valueName, NULL, &dataType, NULL, &dataSize);
        if (result != ERROR_SUCCESS || dataType != REG_SZ) {
            RegCloseKey(hKey);
            return "";
        }
        
        // Allocate buffer and get the actual value
        std::string machineGuid(dataSize - 1, '\0'); // -1 to exclude null terminator
        result = RegQueryValueExA(hKey, valueName, NULL, &dataType, 
                                 reinterpret_cast<LPBYTE>(&machineGuid[0]), &dataSize);
        
        RegCloseKey(hKey);
        
        if (result == ERROR_SUCCESS) {
            return machineGuid;
        }
        
        return "";
    }
    catch (const std::exception& e) {
        return "";
    }
}