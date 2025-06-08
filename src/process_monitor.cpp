#include "process_monitor.hpp"
#include <tlhelp32.h>
#include <fstream>
#include <codecvt>
#include <locale>
#include <shlwapi.h>
#include <filesystem>
#include <algorithm>

std::vector<std::wstring> ProcessMonitor::blacklistProcesses;
const DWORD ProcessMonitor::CHECK_INTERVAL = 1000;
ProcessMonitor::ProcessCache ProcessMonitor::cache = {false, 0};

std::string ProcessMonitor::SanitizeProcessName(const std::string& processName) {
    std::string sanitized;
    sanitized.reserve(processName.size());

    size_t start = processName.find_first_not_of(" \t\r\n");
    size_t end = processName.find_last_not_of(" \t\r\n");
    
    if (start == std::string::npos) {
        return "";
    }

    for (size_t i = start; i <= end; i++) {
        char c = processName[i];
        if (isprint(c) && c != '<' && c != '>' && c != '|' && c != '/' && c != '\\' && c != ':' && c != '"' && c != '?') {
            sanitized += c;
        }
    }
    
    return sanitized;
}

std::wstring ProcessMonitor::StringToWString(const std::string& str) {
    try {
        if (str.empty()) return L"";
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(str);
    } catch (...) {
        return L"";
    }
}

bool ProcessMonitor::LoadBlacklistFromFile() {
    wchar_t exePath[MAX_PATH];
    if (!GetModuleFileNameW(NULL, exePath, MAX_PATH)) {
        return false;
    }

    std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
    std::filesystem::path blacklistPath = exeDir / "blacklist.txt";
    
    std::ifstream file(blacklistPath);
    if (!file.is_open()) {
        return false;
    }

    blacklistProcesses.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::string sanitized = SanitizeProcessName(line);
        
        if (!sanitized.empty() && sanitized.find(".exe") != std::string::npos) {
            std::wstring wline = StringToWString(sanitized);
            if (!wline.empty()) {
                blacklistProcesses.push_back(wline);
            }
        }
    }

    return !blacklistProcesses.empty();
}

bool ProcessMonitor::Initialize() {
    try {
        return LoadBlacklistFromFile();
    } catch (...) {
        return false;
    }
}

bool ProcessMonitor::IsBlacklistedProcessRunning() {
    try {
        DWORD currentTime = GetTickCount();
        
        // Reduz o intervalo de cache para minimizar falsos negativos
        if (currentTime - cache.lastCheckTime < CHECK_INTERVAL / 2) {
            return cache.isBlacklisted;
        }

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return false;
        }

        bool found = false;
        try {
            PROCESSENTRY32W pe32;
            pe32.dwSize = sizeof(pe32);

            if (Process32FirstW(snapshot, &pe32)) {
                do {
                    for (const auto& process : blacklistProcesses) {
                        if (_wcsicmp(pe32.szExeFile, process.c_str()) == 0) {
                            found = true;
                            break;
                        }
                    }
                    if (found) break;
                } while (Process32NextW(snapshot, &pe32));
            }
        } catch (...) {
            CloseHandle(snapshot);
            return false;
        }

        CloseHandle(snapshot);
        
        cache.isBlacklisted = found;
        cache.lastCheckTime = currentTime;
        
        return found;
    } catch (...) {
        return false;
    }
} 