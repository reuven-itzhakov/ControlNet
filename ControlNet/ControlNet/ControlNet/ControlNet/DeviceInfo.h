#pragma once
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <atomic>
#include "firebase/firestore.h"

// Structure to hold all device information
struct DeviceInformation {
    // Basic System Info
    std::string computerName;
    std::string userName;
    std::string systemUptime;
    std::string windowsVersion;
    std::string architecture;
    
    // Hardware Info
    std::string cpuName;
    std::string cpuCores;
    std::string cpuSpeed;
    std::string totalMemory;
    std::string availableMemory;
    
    // Network Info
    std::string primaryMacAddress;
    std::string primaryIPAddress;
    std::vector<std::string> allIPAddresses;
    
    // Software Info
    std::vector<std::string> installedSoftware;
    
    // Timestamps
    std::string lastUpdated;
    std::string firstSeen;
};

// Main functions
DeviceInformation CollectDeviceInformation();
std::string GetComputerNameInfo();
std::string GetUserNameInfo();
std::string GetSystemUptimeInfo();
std::string GetCPUInformation();
std::string GetCPUCores();
std::string GetCPUSpeed();
std::string GetTotalMemoryInfo();
std::string GetAvailableMemoryInfo();
std::string GetPrimaryMACAddress();
std::string GetPrimaryIPAddress();
std::vector<std::string> GetAllIPAddresses();
std::vector<std::string> GetInstalledSoftware();
std::string GetWindowsVersion();
std::string GetSystemArchitecture();

// Helper functions
std::string FormatUptime(DWORD uptimeMs);
std::string FormatMemorySize(DWORDLONG bytes);
firebase::firestore::MapFieldValue DeviceInfoToFirestoreMap(const DeviceInformation& info);

// Device registration with Firestore
bool RegisterDeviceInfo();
bool UpdateDeviceInfo();

// Periodic device information updates
bool StartPeriodicDeviceInfoUpdates();
void StopPeriodicDeviceInfoUpdates();
bool IsPeriodicDeviceInfoUpdatesRunning();