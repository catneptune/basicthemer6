#include "process_monitor.hpp"
#include "window_manager.hpp"
#include <windows.h>

namespace {
    bool shouldExit = false;

    BOOL WINAPI ConsoleHandler(DWORD signal) {
        if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT) {
            shouldExit = true;
            return TRUE;
        }
        return FALSE;
    }

    void CleanupAndExit() {
        WindowManager::Cleanup();
    }
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    // Registra handler de cleanup para quando o programa for fechado
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    atexit(CleanupAndExit);

    // set priority to idle and adjust dpi
    if (!SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS)) {
        MessageBoxW(NULL, L"Failed to set process priority", L"Warning", MB_ICONWARNING);
    }
    
    SetProcessDPIAware();
    
    // initialize process monitor and load blacklist
    if (!ProcessMonitor::Initialize()) {
        MessageBoxW(NULL, L"failed to load blacklist.txt", L"Error", MB_ICONERROR);
        return 1;
    }
    
    // this is the main loop that checks if are any blacklisted processes running, if not, it disables composition on all windows
    MSG msg;
    while (!shouldExit) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                shouldExit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        try {
            if (!ProcessMonitor::IsBlacklistedProcessRunning()) {
                WindowManager::DisableCompositionOnAllWindows();
            }
            Sleep(100);
        } catch (...) {
            Sleep(1000);
        }
    }

    // Garante que a limpeza seja feita antes de sair
    CleanupAndExit();
    return static_cast<int>(msg.wParam);
}