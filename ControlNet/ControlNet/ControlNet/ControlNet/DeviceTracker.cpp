#include "DeviceTracker.h"
#include "DeviceUtils.h"
#include "firebase/app.h"
#include "firebase/firestore.h"
#include <vector>
#include <algorithm>

extern firebase::App* g_firebaseApp;

bool DeviceTracker_AddDevice(const std::string& email) {
    try {
        if (!g_firebaseApp) return false;
        
        std::string machineGuid = GetDeviceMachineGUID();
        if (machineGuid.empty()) return false;
        
        auto firestore = firebase::firestore::Firestore::GetInstance(g_firebaseApp);
        if (!firestore) return false;
        
        // Reference to users/users document
        auto usersDoc = firestore->Collection("users").Document("users");
        
        // Get current document data
        auto future = usersDoc.Get();
        while (future.status() == firebase::kFutureStatusPending) {
            Sleep(10);
        }
        
        firebase::firestore::MapFieldValue data;
        std::vector<firebase::firestore::FieldValue> devices;
        
        // If document exists, get current data
        if (future.error() == firebase::firestore::kErrorOk && future.result()->exists()) {
            data = future.result()->GetData();
            
            // Check if email field exists and is an array
            if (data.find(email) != data.end() && data.at(email).is_array()) {
                auto existingDevices = data.at(email).array_value();
                for (const auto& device : existingDevices) {
                    if (device.is_string()) {
                        std::string deviceStr = device.string_value();
                        // Check if Machine GUID already exists
                        if (deviceStr == machineGuid) {
                            return true; // Already exists
                        }
                        devices.push_back(device);
                    }
                }
            }
        }
        
        // Add the new Machine GUID
        devices.push_back(firebase::firestore::FieldValue::String(machineGuid));
        
        // Update the email field with the devices array
        data[email] = firebase::firestore::FieldValue::Array(devices);
        
        auto setFuture = usersDoc.Set(data, firebase::firestore::SetOptions::Merge());
        while (setFuture.status() == firebase::kFutureStatusPending) {
            Sleep(10);
        }
        
        return setFuture.error() == firebase::firestore::kErrorOk;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool DeviceTracker_RemoveDevice(const std::string& email) {
    try {
        if (!g_firebaseApp) return false;
        
        std::string machineGuid = GetDeviceMachineGUID();
        if (machineGuid.empty()) return false;
        
        auto firestore = firebase::firestore::Firestore::GetInstance(g_firebaseApp);
        if (!firestore) return false;
        
        // Reference to users/users document
        auto usersDoc = firestore->Collection("users").Document("users");
        
        // Get current document data
        auto future = usersDoc.Get();
        while (future.status() == firebase::kFutureStatusPending) {
            Sleep(10);
        }
        
        if (future.error() != firebase::firestore::kErrorOk || !future.result()->exists()) {
            return true; // Document doesn't exist, nothing to remove
        }
        
        firebase::firestore::MapFieldValue data = future.result()->GetData();
        std::vector<firebase::firestore::FieldValue> devices;
        
        // Check if email field exists and is an array
        if (data.find(email) != data.end() && data.at(email).is_array()) {
            auto existingDevices = data.at(email).array_value();
            for (const auto& device : existingDevices) {
                if (device.is_string()) {
                    std::string deviceStr = device.string_value();
                    // Only add devices that are not our Machine GUID
                    if (deviceStr != machineGuid) {
                        devices.push_back(device);
                    }
                }
            }
        }
        
        // Update the email field with the filtered devices array
        data[email] = firebase::firestore::FieldValue::Array(devices);
        
        auto setFuture = usersDoc.Set(data, firebase::firestore::SetOptions::Merge());
        while (setFuture.status() == firebase::kFutureStatusPending) {
            Sleep(10);
        }
        
        return setFuture.error() == firebase::firestore::kErrorOk;
    }
    catch (const std::exception& e) {
        return false;
    }
}