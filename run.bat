@echo off

cd build

if exist factor-e.exe (
    factor-e.exe
) else if exist Release\factor-e.exe (
    Release\factor-e.exe
) else if exist Debug\factor-e.exe (
    Debug\factor-e.exe
) else (
    echo ERROR: Could not find factor-e.exe
    exit /b 1
)
