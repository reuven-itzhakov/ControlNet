#pragma once
#ifndef DBCOMMUNICATION_H
#define DBCOMMUNICATION_H

#include <string>
#include <vector>
#include <cstdint>
#include "firebase/app.h"
#include "firebase/firestore.h"
#include "json.hpp"

bool verifyDocumentExists(const std::string& path);
bool addDocument(const std::string& path, const std::string& document, const firebase::firestore::MapFieldValue& value);
bool updateDocument(const std::string& path, const std::string& document, const firebase::firestore::MapFieldValue& value);
firebase::firestore::MapFieldValue ConvertJsonToObject(const std::string& path);
std::vector<std::string> verifyAllDocumentsExists();
std::vector<std::string> GetFilesWin(const std::string& path);

// Startup synchronization function
bool SyncMissingDocumentsToFirestore();

// Periodic update functions
bool StartPeriodicUpdates();
void StopPeriodicUpdates();
bool UpdateTodayDocumentNow();

// Device counters
bool EnsureDeviceCounterExists(const std::string& field, int64_t initialValue = 0);
bool IncrementDeviceCounter(const std::string& field, int64_t delta = 1);

// Daily device counter helpers
bool EnsureDeviceDailyCounterFresh(const std::string& counterField, const std::string& dateField);
bool IncrementDeviceDailyCounter(const std::string& counterField, const std::string& dateField, int64_t delta = 1);

#endif // DBCOMMUNICATION_H