# Project Organization Summary

This document describes how the ControlNet project functions have been organized into logical modules.

## File Structure Overview

### Core Application Files
- **ControlNet.cpp/h**: Main application entry point, window creation, and message loop
- **framework.h**: Standard Windows includes and definitions
- **Resource.h**: Resource definitions for dialogs and UI elements

### Utility Modules

#### FileUtils.h/cpp
**Purpose**: File operations and formatting utilities
- `GetJsonFiles()`: Enumerate JSON files in a directory
- `formatSize()`: Format file size in human-readable format
- `formatTimestamp()`: Convert timestamp to HH:MM:SS format

#### UIUtils.h/cpp  
**Purpose**: User interface utilities and data management
- `LeftPaneRow`, `RightPaneRow`: Data structures for ListView
- `AddLeftPaneRow()`: Add row to left ListView
- `loadLogs()`: Load and populate log data
- Global variables for sorting and UI data management

#### MessageHandlers.h/cpp
**Purpose**: Window message processing utilities
- `HandleLeftPaneClick()`: Process left pane click events
- `HandleColumnClick()`: Process column header clicks
- `HandleRightPaneColumnClick()`: Process right pane sorting

#### Utils.h/cpp
**Purpose**: General system utilities
- `getDeviceGUID()`: Get Windows Machine GUID (replaces computer name)
- `getCurrentDate()`: Get current date string
- `getCurrentTimestamp()`: Get current timestamp

#### DeviceUtils.h/cpp
**Purpose**: Device identification utilities
- `GetDeviceMachineGUID()`: Get Windows Machine GUID from registry

#### DeviceInfo.h/cpp
**Purpose**: Comprehensive device information collection and management
- `CollectDeviceInformation()`: Collect all device information
- `RegisterDeviceInfo()`: Register device info with Firestore
- `UpdateDeviceInfo()`: Update device info in Firestore
- `GetComputerNameInfo()`: Get computer name
- `GetUserNameInfo()`: Get current user name
- `GetSystemUptimeInfo()`: Get system uptime
- `GetCPUInformation()`: Get CPU name and details
- `GetCPUCores()`: Get number of CPU cores
- `GetCPUSpeed()`: Get CPU speed
- `GetTotalMemoryInfo()`: Get total system memory
- `GetAvailableMemoryInfo()`: Get available system memory
- `GetPrimaryMACAddress()`: Get primary network adapter MAC address
- `GetPrimaryIPAddress()`: Get primary IP address
- `GetAllIPAddresses()`: Get all network adapter IP addresses
- `GetInstalledSoftware()`: Get list of installed software
- `GetWindowsVersion()`: Get Windows version information
- `GetSystemArchitecture()`: Get system architecture (x86/x64/ARM)
- `DeviceInfoToFirestoreMap()`: Convert device info to Firestore format

#### UnicodeUtils.h/cpp
**Purpose**: Advanced Unicode and emoji handling
- `GetUnicodeFromKeyboard()`: Enhanced keyboard input processing for complex characters
- `IsPrintableUnicode()`: Check if Unicode text is printable (including emojis)
- `IsValidUnicodeSequence()`: Validate Unicode sequences and surrogate pairs
- `GetUnicodeCharacterCount()`: Count actual characters (handling surrogate pairs)
- `SafeWStringToUTF8()`: Safe Unicode to UTF-8 conversion
- `SafeUTF8ToWString()`: Safe UTF-8 to Unicode conversion
- Helper functions for surrogate pair detection and emoji code point identification

#### ProcessUtils.h/cpp
**Purpose**: Process and window management utilities
- `GetActiveProcessName()`: Get name of foreground process
- `GetActiveWindowTitle()`: Get title of active window

### Application-Specific Modules

#### FirebaseManager.h/cpp
**Purpose**: Firebase initialization and management
- `InitializeFirebase()`: Initialize Firebase services
- Global Firebase app, auth, and database variables

#### DeviceTracker.h/cpp
**Purpose**: Device tracking for user authentication
- `DeviceTracker_AddDevice()`: Add device GUID to user's device list in Firestore
- `DeviceTracker_RemoveDevice()`: Remove device GUID from user's device list

#### KeyLogger.h/cpp
**Purpose**: Keyboard and mouse hook management (Enhanced for Unicode)
- `LowLevelKeyboardProc()`: Low-level keyboard hook with improved emoji support
- `LowLevelMouseProc()`: Low-level mouse hook

#### JsonLog.h/cpp
**Purpose**: JSON logging operations (Enhanced for Unicode)
- `WriteKeystrokeToJson()`: Write keystroke data to JSON files with proper emoji support
- `createNestedFolders()`: Create directory structure

#### DBcommunication.h/cpp
**Purpose**: Database and Firestore operations (Enhanced with Periodic Updates)
- `ConvertJsonToObject()`: Convert JSON to Firestore format
- `verifyDocumentExists()`: Check if document exists
- `addDocument()`: Add document to Firestore
- `updateDocument()`: Update Firestore document
- `verifyAllDocumentsExists()`: Batch document verification
- `SyncMissingDocumentsToFirestore()`: Startup synchronization of missing documents
- `StartPeriodicUpdates()`: Start background thread for periodic updates
- `StopPeriodicUpdates()`: Stop periodic update thread safely  
- `UpdateTodayDocumentNow()`: Manual immediate update of today's document
- `PeriodicUpdateWorker()`: Background worker thread (every 5 minutes)

#### AuthService.h/cpp
**Purpose**: Authentication management with device tracking
- `AuthService_SignInAsync()`: Async sign-in operations with device registration
- `AuthService_SignOut()`: Sign-out operations with device removal
- `AuthService_IsSignedIn()`: Authentication status check

#### HotkeySettings.h/cpp
**Purpose**: Hotkey configuration management
- `LoadHotkeySettings()`: Load hotkey configuration
- `SaveHotkeySettings()`: Save hotkey configuration
- `UpdateHotkey()`: Register/update global hotkey

#### SettingsDialog.h/cpp
**Purpose**: Settings dialog management
- `SettingsDlgProc()`: Settings dialog message handler

#### LogWriter.h/cpp
**Purpose**: Asynchronous log writing
- `LogWriter_Init()`: Initialize background writer
- `LogWriter_Enqueue()`: Queue log entries
- `LogWriter_Shutdown()`: Shutdown writer thread

## Device Identification System

### Machine GUID Implementation
The system now uses Windows Machine GUID instead of computer names for device identification:

- **Registry Path**: `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\MachineGuid`
- **Format**: Standard GUID format (e.g., `12345678-1234-1234-1234-123456789abc`)
- **Benefits**: 
  - More reliable than MAC addresses
  - Unique per Windows installation
  - Persistent across hardware changes
  - No network dependencies

### Comprehensive Device Information Collection
The system now collects detailed device information without requiring administrator privileges:

#### **System Information**
- Computer Name
- Current User Name  
- System Uptime (formatted as "X days, Xh Xm Xs")
- Windows Version (from registry)
- System Architecture (x86/x64/ARM/ARM64)

#### **Hardware Information**
- CPU Name (from registry)
- CPU Core Count
- CPU Speed (MHz)
- Total Physical Memory (formatted in GB)
- Available Physical Memory (formatted in GB)

#### **Network Information**
- Primary MAC Address (first active Ethernet adapter)
- Primary IP Address (first active adapter IP)
- All IP Addresses (array of all adapter IPs)

#### **Software Information**
- Installed Software List (from registry, limited to 100 entries)
- First Seen Date
- Last Updated Date

### Database Structure Changes
All database operations now use Machine GUID and comprehensive device tracking:

- **Local Storage**: `{GUID}/YYYY-MM-DD.json`
- **Firestore Paths**: 
  - `records/{GUID}/keyStrokes/{date}` - Keystroke records
  - `devices/{GUID}` - Complete device information
- **User Device Tracking**: `users/users/{email}` contains array of GUIDs

## Authentication and Device Tracking

### Sign-in Process
1. User authenticates with email/password
2. System retrieves Machine GUID
3. GUID is added to user's device array in `users/users/{email}`
4. Complete device information is collected and stored in `devices/{GUID}`
5. Duplicate GUIDs are automatically prevented

### Sign-out Process
1. System retrieves current user's email
2. Machine GUID is removed from user's device array
3. User is signed out from Firebase Auth

### Firestore Document Structure
```json
{
  "users/users": {
    "user@gmail.com": ["12345678-1234-1234-1234-123456789abc"],
    "another@email.com": ["12345678-1234-1234-1234-123456789abc", "87654321-4321-4321-4321-cba987654321"]
  },
  "devices": {
    "12345678-1234-1234-1234-123456789abc": {
      "computerName": "DESKTOP-ABC123",
      "userName": "JohnDoe",
      "systemUptime": "2 days, 14h 23m 45s",
      "windowsVersion": "Windows 11 Pro 10.0 Build 22000",
      "architecture": "x64",
      "cpuName": "Intel(R) Core(TM) i7-10700K CPU @ 3.80GHz",
      "cpuCores": "8",
      "cpuSpeed": "3792 MHz",
      "totalMemory": "16.00 GB",
      "availableMemory": "8.45 GB",
      "primaryMacAddress": "12:34:56:78:90:ab",
      "primaryIPAddress": "192.168.1.100",
      "allIPAddresses": ["192.168.1.100", "169.254.1.1"],
      "installedSoftware": ["Microsoft Office", "Google Chrome", "Adobe Reader", "..."],
      "firstSeen": "2024-12-19",
      "lastUpdated": "2024-12-19"
    }
  },
  "records": {
    "12345678-1234-1234-1234-123456789abc": {
      "keyStrokes": {
        "2024-12-19": { ... },
        "2024-12-20": { ... }
      }
    }
  }
}
```

## Unicode and Emoji Support Improvements

### Problem Addressed
The original code had issues handling complex Unicode characters, particularly emojis like "????" (Austrian flag), which are composed of multiple Unicode code points (Regional Indicator Symbols).

### Solutions Implemented

1. **Enhanced Unicode Input Processing**
   - Larger buffer sizes for `ToUnicodeEx()` calls
   - Better handling of surrogate pairs and complex sequences
   - Proper validation of Unicode sequences

2. **Improved UTF-8 Conversion**
   - Replaced `std::codecvt` with Windows API functions (`WideCharToMultiByte`/`MultiByteToWideChar`)
   - Added validation and error handling for conversion failures
   - Safe conversion functions that handle all Unicode ranges

3. **Emoji Detection and Support**
   - Comprehensive emoji code point range checking
   - Support for Regional Indicator Symbols (flag emojis)
   - Proper handling of surrogate pairs in Windows wide strings

4. **UI Display Improvements**
   - Proper Unicode rendering in ListView controls
   - Safe conversion for JSON data display
   - Validation before displaying text to prevent crashes

### Specific Emoji Support
- **Basic Emojis**: ?? ?? ?? (U+1F600-U+1F64F)
- **Symbols and Pictographs**: ?? ?? ? (U+1F300-U+1F5FF, U+1F680-U+1F6FF)
- **Flag Emojis**: ???? ???? ???? (U+1F1E6-U+1F1FF Regional Indicators)
- **Extended Emojis**: ?? ?? ?? (U+1F900-U+1F9FF)

## Benefits of This Organization

1. **Separation of Concerns**: Each module has a specific responsibility
2. **Maintainability**: Code is easier to locate, modify, and debug
3. **Unicode Compatibility**: Proper handling of all Unicode characters including complex emojis
4. **Device Reliability**: Machine GUID provides more stable device identification
5. **Authentication Security**: Device tracking prevents unauthorized access
6. **Comprehensive Monitoring**: Detailed device information for security and analytics
7. **Privacy Compliant**: No admin privileges required, respects user boundaries
8. **Reusability**: Utility functions can be easily reused across the project
9. **Readability**: Main application logic is cleaner and easier to understand
10. **Testing**: Individual modules can be tested in isolation
11. **Build Performance**: Changes to specific functionality only require rebuilding affected modules
12. **Internationalization**: Better support for international characters and symbols

## Integration Points

- **ControlNet.cpp** includes all necessary headers and coordinates between modules
- **UIUtils** depends on **FileUtils** and **UnicodeUtils** for formatting and Unicode handling
- **MessageHandlers** depends on **UIUtils**, **FileUtils**, and **UnicodeUtils** for data processing
- **KeyLogger** uses **UnicodeUtils** for proper character input handling
- **JsonLog** uses **UnicodeUtils** for safe UTF-8 conversion and **Utils** for device identification
- **DBcommunication** uses **Utils** for device identification and system information
- **AuthService** uses **DeviceTracker** for device registration/removal and **DeviceInfo** for comprehensive device data
- **DeviceTracker** uses **DeviceUtils** for Machine GUID retrieval
- **DeviceInfo** uses **DeviceUtils** and **Utils** for device identification and system information
- **FirebaseManager** provides centralized Firebase initialization for the entire application

This organization follows standard C++ project structure conventions, provides comprehensive Unicode support, implements reliable device identification with detailed system information collection, and establishes a solid foundation for future development and maintenance.