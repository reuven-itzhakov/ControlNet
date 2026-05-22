#pragma once
#include <string>

// Device tracking functions for Firestore
bool DeviceTracker_AddDevice(const std::string& email);
bool DeviceTracker_RemoveDevice(const std::string& email);