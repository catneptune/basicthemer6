#pragma once
#include <windows.h>
#include <dwmapi.h>
#include <unordered_map>

class WindowManager {
public:
    static bool DisableCompositionOnAllWindows();
    static bool RestoreAllWindows();
    static void Cleanup();

private:
    struct WindowState {
        DWMNCRENDERINGPOLICY originalPolicy;
        bool wasModified;
    };

    struct EnumWindowsData {
        bool hasBlacklisted;
        DWMNCRENDERINGPOLICY policy;
        std::unordered_map<HWND, WindowState>* windowStates;
    };

    static std::unordered_map<HWND, WindowState> modifiedWindows;
    static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
    static BOOL CALLBACK RestoreWindowsProc(HWND hWnd, LPARAM lParam);
}; 