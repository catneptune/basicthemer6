@echo off
setlocal enabledelayedexpansion

:: create output directory if it doesn't exist
if not exist ".\out" mkdir ".\out"

:: compile all source files with the following flags
g++ -O2 ^
    .\src\main.cpp ^
    .\src\process_monitor.cpp ^
    .\src\window_manager.cpp ^
    -o .\out\basicthemer.exe ^
    -I.\src ^
    -std=c++17 ^
    -ldwmapi ^
    -luser32 ^
    -lgdi32 ^
    -lshlwapi ^
    -Wl,--subsystem,windows

:: verify if the compilation was successful
if %errorlevel% neq 0 (
    echo error compiling
    exit /b %errorlevel%
)

:: copy the example blacklist.txt to the output directory
copy /Y ".\src\blacklist.txt" ".\out\blacklist.txt" >nul 2>&1

echo build successful
echo executable generated in .\out\basicthemer.exe