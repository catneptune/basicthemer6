#pragma once
#include <windows.h>
#include <vector>
#include <string>

class ProcessMonitor {
public:
    static bool IsBlacklistedProcessRunning();
    static bool Initialize();

private:
    static std::vector<std::wstring> blacklistProcesses;
    static const DWORD CHECK_INTERVAL;
    
    struct ProcessCache {
        bool isBlacklisted;
        DWORD lastCheckTime;
    };
    
    static ProcessCache cache;
    static bool LoadBlacklistFromFile();
    static std::wstring StringToWString(const std::string& str);
    static std::string SanitizeProcessName(const std::string& processName);
}; 