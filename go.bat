@echo off
setlocal EnableDelayedExpansion

REM Exit on any error
echo current step: build

cd build
if not exist build (
    echo Creating build directory...
    mkdir build
    cd build
)

echo current step: cmake
cmake .. 
if !errorlevel! neq 0 (
    echo CMAKE failed
    exit /b 1
) else (
    echo CMAKE successful
)

echo current step: build
cmake --build . --config Release
if !errorlevel! neq 0 (
    echo BUILD failed
    exit /b 1
) else (
    echo BUILD successful
)

echo current step: run
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
