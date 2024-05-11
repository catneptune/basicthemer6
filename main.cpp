#include <windows.h>
#include <dwmapi.h>

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWMNCRENDERINGPOLICY pol = DWMNCRP_DISABLED;
    DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &pol, sizeof(DWMNCRENDERINGPOLICY));
    return TRUE;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    for (;;) {
        Sleep(100);
        EnumDesktopWindows(NULL, EnumWindowsProc, NULL);
    }
    return 0;
}
