@echo off
setlocal enabledelayedexpansion

if "%1"=="clean" (
    call :cleanup_build
    echo build directory cleaned.
    exit /b 0
)

if not exist "build" (
    mkdir build
)

cd build

echo running CMake...
cmake -A x64 ..
if %errorlevel% neq 0 (
    echo CMAKE failed
    exit /b 1
) else (
    echo CMAKE successful
)

echo building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo BUILD failed
    exit /b 1
) else (
    echo BUILD successful
)

echo.
echo executable in: build\Release\factor-e.exe
goto :eof

:cleanup_build
if exist "build" (
    echo cleaning old build directory...
    rmdir /s /q build
)
goto :eof