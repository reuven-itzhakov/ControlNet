#include "AuthService.h"
#include "DeviceTracker.h"
#include "DeviceInfo.h"
#include "firebase/auth.h"
#include <thread>
#include <atomic>

extern firebase::auth::Auth* g_firebaseAuth; // provided by main app

namespace {
    std::atomic<int> g_pendingOps{0};
}

void AuthService_Init() {}
void AuthService_Shutdown() {}

bool AuthService_IsSignedIn() {
    return g_firebaseAuth && g_firebaseAuth->current_user().is_valid();
}

std::string AuthService_CurrentEmail() {
    if (AuthService_IsSignedIn()) return g_firebaseAuth->current_user().email();
    return {}; 
}

void AuthService_SignInAsync(const std::string& email, const std::string& password, HWND notifyHwnd) {
    if (!g_firebaseAuth || g_pendingOps.load() > 0) {
        // Post failure immediately if no auth or already pending
        PostMessage(notifyHwnd, WM_AUTH_RESULT, 0, 0);
        return;
    }
    g_pendingOps.fetch_add(1);
    std::thread([email, password, notifyHwnd]() {
        auto future = g_firebaseAuth->SignInWithEmailAndPassword(email.c_str(), password.c_str());
        while (future.status() == firebase::kFutureStatusPending) {
            Sleep(15);
        }
        bool success = future.error() == 0;
        
        // If sign-in was successful, add device to Firestore
        if (success) {
            try {
                DeviceTracker_AddDevice(email);
                RegisterDeviceInfo();
            }
            catch (const std::exception& e) {
                // Device tracking failed, but don't fail the sign-in
            }
        }
        
        g_pendingOps.fetch_sub(1);
        PostMessage(notifyHwnd, WM_AUTH_RESULT, success ? 1 : 0, 0);
    }).detach();
}

void AuthService_SignOut() {
    if (g_firebaseAuth) {
        try {
            std::string currentEmail = AuthService_CurrentEmail();
            g_firebaseAuth->SignOut();
            
            // Remove device from Firestore after sign out
            if (!currentEmail.empty()) {
                DeviceTracker_RemoveDevice(currentEmail);
            }
        }
        catch (const std::exception& e) {
            // If device removal fails, still sign out
            g_firebaseAuth->SignOut();
        }
    }
}
