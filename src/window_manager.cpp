#include "window_manager.hpp"

std::unordered_map<HWND, WindowManager::WindowState> WindowManager::modifiedWindows;

BOOL CALLBACK WindowManager::EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    if (!hWnd || !lParam) return TRUE;
    
    auto* data = reinterpret_cast<EnumWindowsData*>(lParam);
    
    if (!IsWindow(hWnd)) return TRUE;

    // Salva o estado original da janela se ainda não foi salvo
    if (data->windowStates->find(hWnd) == data->windowStates->end()) {
        DWMNCRENDERINGPOLICY originalPolicy;
        if (SUCCEEDED(DwmGetWindowAttribute(
            hWnd,
            DWMWA_NCRENDERING_POLICY,
            &originalPolicy,
            sizeof(DWMNCRENDERINGPOLICY)))) {
            (*data->windowStates)[hWnd] = {originalPolicy, false};
        }
    }
    
    HRESULT hr = DwmSetWindowAttribute(
        hWnd,
        DWMWA_NCRENDERING_POLICY,
        &data->policy,
        sizeof(DWMNCRENDERINGPOLICY)
    );

    if (SUCCEEDED(hr)) {
        (*data->windowStates)[hWnd].wasModified = true;
    }

    return TRUE;
}

BOOL CALLBACK WindowManager::RestoreWindowsProc(HWND hWnd, LPARAM lParam) {
    if (!hWnd || !IsWindow(hWnd)) return TRUE;

    auto it = modifiedWindows.find(hWnd);
    if (it != modifiedWindows.end() && it->second.wasModified) {
        DwmSetWindowAttribute(
            hWnd,
            DWMWA_NCRENDERING_POLICY,
            &it->second.originalPolicy,
            sizeof(DWMNCRENDERINGPOLICY)
        );
    }

    return TRUE;
}

bool WindowManager::DisableCompositionOnAllWindows() {
    try {
        EnumWindowsData data = {
            false,
            DWMNCRP_DISABLED,
            &modifiedWindows
        };
        
        if (!EnumDesktopWindows(NULL, EnumWindowsProc, reinterpret_cast<LPARAM>(&data))) {
            return false;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool WindowManager::RestoreAllWindows() {
    try {
        if (!EnumDesktopWindows(NULL, RestoreWindowsProc, 0)) {
            return false;
        }
        return true;
    } catch (...) {
        return false;
    }
}

void WindowManager::Cleanup() {
    RestoreAllWindows();
    modifiedWindows.clear();
} 