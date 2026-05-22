#include "framework.h"
#include <windows.h>
#include <commctrl.h>
#include <string>
#include "resource.h"
#include "SettingsDialog.h"
#include "HotkeySettings.h"
#include "firebase/auth.h"
#include "AuthService.h"

#pragma comment(lib, "Comctl32.lib")

// External Firebase auth (declared elsewhere)
extern firebase::auth::Auth* g_firebaseAuth;

// Forward declarations for page logic
static void InitHotkeyPage(HWND hPage);
static void UpdateUserPageState(HWND hPage);
static INT_PTR CALLBACK HotkeyPageProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK UserPageProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK ProgramPageProc(HWND, UINT, WPARAM, LPARAM);

// ---------------- Child page procedures ----------------
static INT_PTR CALLBACK HotkeyPageProc(HWND hDlg, UINT msg, WPARAM, LPARAM) {
    try {
        if (msg == WM_INITDIALOG) { 
            InitHotkeyPage(hDlg); 
            return TRUE; 
        }
        return FALSE;
    }
    catch (const std::exception& e) {
        return FALSE;
    }
}

static INT_PTR CALLBACK UserPageProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM) {
    try {
        switch(msg) {
        case WM_INITDIALOG: 
            try {
                UpdateUserPageState(hDlg); 
            }
            catch (const std::exception& e) {
                // State update failed - continue with dialog
            }
            return TRUE;
            
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
            case IDC_USER_SIGNIN: {
                try {
                    char email[128] = {0}; 
                    char pwd[128] = {0};
                    GetDlgItemTextA(hDlg, IDC_USER_EMAIL_EDIT, email, sizeof(email));
                    GetDlgItemTextA(hDlg, IDC_USER_PASSWORD_EDIT, pwd, sizeof(pwd));
                    EnableWindow(GetDlgItem(hDlg, IDC_USER_SIGNIN), FALSE);
                    AuthService_SignInAsync(email, pwd, GetParent(hDlg));
                    SecureZeroMemory(pwd, sizeof(pwd));
                }
                catch (const std::exception& e) {
                    // Sign in failed - re-enable button
                    EnableWindow(GetDlgItem(hDlg, IDC_USER_SIGNIN), TRUE);
                }
                return TRUE; 
            }
            case IDC_USER_SIGNOUT: {
                try {
                    AuthService_SignOut();
                    UpdateUserPageState(hDlg);
                    MessageBoxA(GetParent(GetParent(hDlg)), "Signed out.", "Info", MB_OK|MB_ICONINFORMATION);
                }
                catch (const std::exception& e) {
                    // Sign out failed
                    MessageBoxA(GetParent(GetParent(hDlg)), "Sign out failed.", "Error", MB_OK|MB_ICONERROR);
                }
                return TRUE; 
            }
            }
            break;
        }
        return FALSE;
    }
    catch (const std::exception& e) {
        return FALSE;
    }
}

static INT_PTR CALLBACK ProgramPageProc(HWND, UINT msg, WPARAM, LPARAM) {
    try {
        return msg == WM_INITDIALOG ? TRUE : FALSE;
    }
    catch (const std::exception& e) {
        return FALSE;
    }
}

// ---------------- Helpers ----------------
static void InitHotkeyPage(HWND hPage) {
    try {
        HWND hMod = GetDlgItem(hPage, IDC_HOTKEY_MODIFIER);
        HWND hKey = GetDlgItem(hPage, IDC_HOTKEY_KEY);
        if (!hMod || !hKey) return;
        
        try {
            SendMessage(hMod, CB_RESETCONTENT,0,0);
            SendMessage(hMod, CB_ADDSTRING,0,(LPARAM)L"Ctrl + Alt");
            SendMessage(hMod, CB_ADDSTRING,0,(LPARAM)L"Ctrl");
            SendMessage(hMod, CB_ADDSTRING,0,(LPARAM)L"Alt");
            SendMessage(hMod, CB_ADDSTRING,0,(LPARAM)L"None");
            
            int modIdx=0; 
            if (g_hotkeyModifiers==MOD_CONTROL) modIdx=1; 
            else if (g_hotkeyModifiers==MOD_ALT) modIdx=2; 
            else if (g_hotkeyModifiers==0) modIdx=3;
            SendMessage(hMod, CB_SETCURSEL, modIdx,0);
            
            SendMessage(hKey, CB_RESETCONTENT,0,0);
            for (int vk=VK_F1; vk<=VK_F12; ++vk) { 
                wchar_t buf[8]; 
                swprintf(buf,8,L"F%d", vk-VK_F1+1); 
                SendMessage(hKey, CB_ADDSTRING,0,(LPARAM)buf);
            } 
            
            if (g_hotkeyVk>=VK_F1 && g_hotkeyVk<=VK_F12) {
                SendMessage(hKey, CB_SETCURSEL, g_hotkeyVk-VK_F1,0);
            }
        }
        catch (const std::exception& e) {
            // Control initialization failed - leave with defaults
        }
    }
    catch (const std::exception& e) {
        // Hotkey page initialization failed
    }
}

static void UpdateUserPageState(HWND hPage) {
    try {
        bool signedIn = false;
        try {
            signedIn = g_firebaseAuth && g_firebaseAuth->current_user().is_valid();
        }
        catch (const std::exception& e) {
            // Firebase auth check failed - assume not signed in
            signedIn = false;
        }
        
        auto Show = [&](int id, BOOL vis){ 
            try {
                ShowWindow(GetDlgItem(hPage,id), vis?SW_SHOW:SW_HIDE); 
            }
            catch (const std::exception& e) {
                // Show/hide failed for this control
            }
        };
        
        Show(IDC_USER_EMAIL_EDIT,        !signedIn);
        Show(IDC_USER_PASSWORD_EDIT,     !signedIn);
        Show(IDC_USER_EMAIL_LABEL,       !signedIn);
        Show(IDC_USER_PASSWORD_LABEL,    !signedIn);
        Show(IDC_USER_SIGNIN,            !signedIn);
        Show(IDC_USER_SIGNEDIN_LABEL,     signedIn);
        Show(IDC_USER_SIGNEDIN_EMAIL_EDIT,signedIn);
        Show(IDC_USER_SIGNOUT,            signedIn);
        
        if (signedIn) {
            try {
                std::string email = g_firebaseAuth->current_user().email();
                SetWindowTextA(GetDlgItem(hPage, IDC_USER_SIGNEDIN_EMAIL_EDIT), email.c_str());
            }
            catch (const std::exception& e) {
                // Email retrieval failed
                SetWindowTextA(GetDlgItem(hPage, IDC_USER_SIGNEDIN_EMAIL_EDIT), "Error retrieving email");
            }
        } else {
            try {
                SetWindowTextA(GetDlgItem(hPage, IDC_USER_EMAIL_EDIT), "");
                SetWindowTextA(GetDlgItem(hPage, IDC_USER_PASSWORD_EDIT), "");
                SetWindowTextA(GetDlgItem(hPage, IDC_USER_SIGNEDIN_EMAIL_EDIT), "");
            }
            catch (const std::exception& e) {
                // Text clearing failed - ignore
            }
        }
    }
    catch (const std::exception& e) {
        // User page state update failed
    }
}

// ---------------- Main Settings Dialog ----------------
INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hTab=nullptr, hHotkeyPage=nullptr, hUserPage=nullptr, hProgramPage=nullptr;
    
    try {
        switch(message) {
        case WM_CLOSE: {
            try {
                int res = MessageBox(hDlg, L"Discard changes?\n\nAny unsaved changes will be lost.", L"Discard Changes", MB_OKCANCEL|MB_ICONWARNING|MB_DEFBUTTON2);
                if (res==IDOK) EndDialog(hDlg, IDC_SETTINGS_CANCEL);
            }
            catch (const std::exception& e) {
                // Force close on error
                EndDialog(hDlg, IDC_SETTINGS_CANCEL);
            }
            return TRUE; 
        }
        
        case WM_INITDIALOG: {
            try {
                hTab = GetDlgItem(hDlg, IDC_SETTINGS_TAB);
                TCITEM tie; tie.mask = TCIF_TEXT;
                tie.pszText=(LPWSTR)L"Hotkey";  TabCtrl_InsertItem(hTab,0,&tie);
                tie.pszText=(LPWSTR)L"User";    TabCtrl_InsertItem(hTab,1,&tie);
                tie.pszText=(LPWSTR)L"Program"; TabCtrl_InsertItem(hTab,2,&tie);
                
                hHotkeyPage  = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGS_HOTKEY),  hDlg, HotkeyPageProc,0);
                hUserPage    = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGS_USER),    hDlg, UserPageProc,0);
                hProgramPage = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGS_PROGRAM), hDlg, ProgramPageProc,0);
                
                RECT rc; GetWindowRect(hTab,&rc); 
                MapWindowPoints(NULL,hDlg,(LPPOINT)&rc,2); 
                RECT rcPage=rc; 
                TabCtrl_AdjustRect(hTab,FALSE,&rcPage);
                
                HWND pages[]={hHotkeyPage,hUserPage,hProgramPage};
                for(HWND p:pages) {
                    if (p) {
                        SetWindowPos(p,NULL,rcPage.left,rcPage.top, rcPage.right-rcPage.left, rcPage.bottom-rcPage.top, SWP_SHOWWINDOW);
                    }
                }
                
                if (hUserPage) ShowWindow(hUserPage,SW_HIDE); 
                if (hProgramPage) ShowWindow(hProgramPage,SW_HIDE);
            }
            catch (const std::exception& e) {
                // Dialog initialization failed - close dialog
                EndDialog(hDlg, IDC_SETTINGS_CANCEL);
                return FALSE;
            }
            return TRUE; 
        }
        
        case WM_NOTIFY: {
            try {
                LPNMHDR pnmh = (LPNMHDR)lParam;
                if (pnmh && pnmh->idFrom==IDC_SETTINGS_TAB && pnmh->code==TCN_SELCHANGE) {
                    int sel = TabCtrl_GetCurSel(hTab);
                    if (hHotkeyPage) ShowWindow(hHotkeyPage, sel==0?SW_SHOW:SW_HIDE);
                    if (hUserPage) ShowWindow(hUserPage,   sel==1?SW_SHOW:SW_HIDE);
                    if (hProgramPage) ShowWindow(hProgramPage,sel==2?SW_SHOW:SW_HIDE);
                    if (sel==1 && hUserPage) {
                        try {
                            UpdateUserPageState(hUserPage);
                        }
                        catch (const std::exception& e) {
                            // User page update failed - ignore
                        }
                    }
                }
            }
            catch (const std::exception& e) {
                // Notify handling failed - ignore
            }
            break; 
        }
        
        case WM_AUTH_RESULT: { // async auth completion
            try {
                bool success = (wParam != 0);
                // Re-enable sign in button
                if (hUserPage) {
                    EnableWindow(GetDlgItem(hUserPage, IDC_USER_SIGNIN), !AuthService_IsSignedIn());
                }
                
                if (success) {
                    try {
                        UpdateUserPageState(hUserPage);
                        MessageBoxA(hDlg, "Login successful!", "Info", MB_OK|MB_ICONINFORMATION);
                    }
                    catch (const std::exception& e) {
                        MessageBoxA(hDlg, "Login successful, but UI update failed!", "Warning", MB_OK|MB_ICONWARNING);
                    }
                } else {
                    MessageBoxA(hDlg, "Login failed.", "Error", MB_OK|MB_ICONERROR);
                }
            }
            catch (const std::exception& e) {
                // Auth result handling failed
                MessageBoxA(hDlg, "Auth result processing failed.", "Error", MB_OK|MB_ICONERROR);
            }
            return TRUE; 
        }
        
        case WM_COMMAND: {
            try {
                switch(LOWORD(wParam)) {
                case IDC_SETTINGS_SAVE: {
                    try {
                        if (hHotkeyPage) {
                            int modSel=(int)SendDlgItemMessage(hHotkeyPage,IDC_HOTKEY_MODIFIER,CB_GETCURSEL,0,0);
                            switch(modSel){
                                case 0:g_hotkeyModifiers=MOD_CONTROL|MOD_ALT;break;
                                case 1:g_hotkeyModifiers=MOD_CONTROL;break;
                                case 2:g_hotkeyModifiers=MOD_ALT;break;
                                case 3:g_hotkeyModifiers=0;break;
                            }
                            
                            int keySel=(int)SendDlgItemMessage(hHotkeyPage,IDC_HOTKEY_KEY,CB_GETCURSEL,0,0);
                            if (keySel>=0) g_hotkeyVk = VK_F1 + keySel;
                            
                            SaveHotkeySettings();
                        }
                    }
                    catch (const std::exception& e) {
                        // Settings save failed - show error but still close
                        MessageBoxA(hDlg, "Failed to save some settings.", "Warning", MB_OK|MB_ICONWARNING);
                    }
                    
                    EndDialog(hDlg, IDC_SETTINGS_SAVE);
                    return TRUE; 
                }
                case IDC_SETTINGS_CANCEL:
                    EndDialog(hDlg, IDC_SETTINGS_CANCEL); 
                    return TRUE;
                }
            }
            catch (const std::exception& e) {
                // Command handling failed - force close
                EndDialog(hDlg, IDC_SETTINGS_CANCEL);
                return TRUE;
            }
            break; 
        }
        }
        return FALSE;
    }
    catch (const std::exception& e) {
        // Critical error in dialog proc - force close
        EndDialog(hDlg, IDC_SETTINGS_CANCEL);
        return FALSE;
    }
}
