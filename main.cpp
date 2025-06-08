#include <windows.h>
#include <dwmapi.h>

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWMNCRENDERINGPOLICY policy = DWMNCRP_DISABLED;
    HRESULT hr = DwmSetWindowAttribute(
        hWnd,
        DWMWA_NCRENDERING_POLICY,
        &policy,
        sizeof(DWMNCRENDERINGPOLICY)
    );

    return TRUE;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    for (;;) {
        Sleep(2);
        SetProcessDPIAware(); 
        EnumDesktopWindows(NULL, EnumWindowsProc, (LPARAM)0);
        EnumDesktopWindows(NULL, EnumWindowsProc, NULL);
    }
    return 0; 
}