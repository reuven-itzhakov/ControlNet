#pragma once
#include <string>
#include <ctime>
#include <vector>
#include <windows.h>

// Screenshot capture and management functions
bool CaptureScreenshot(const std::string& deviceGuid, std::time_t timestamp);
std::string ConvertScreenshotToBase64(const std::string& filePath);

// Periodic screenshot functions
bool StartPeriodicScreenshots();
void StopPeriodicScreenshots();

// Count screenshots for a specific date
int CountScreenshotsForDate(const std::string& deviceGuid, const std::string& date);

// Get all screenshot files for a date
std::vector<std::string> GetScreenshotFilesForDate(const std::string& deviceGuid, const std::string& date);
