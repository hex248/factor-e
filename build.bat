@echo off
setlocal enabledelayedexpansion

set VCPKG_PATH=%USERPROFILE%/vcpkg

if not exist "%USERPROFILE%\vcpkg\scripts\buildsystems\vcpkg.cmake" (
    echo Error: vcpkg not found at %USERPROFILE%\vcpkg
    echo Please make sure vcpkg is installed in your user directory.
    exit /b 1
)

if "%1"=="clean" (
    call :cleanup_build
    echo Build directory cleaned.
    exit /b 0
)

if not exist "build" (
    mkdir build
)

cd build

echo Running CMake...
cmake -A x64 -DCMAKE_TOOLCHAIN_FILE=%VCPKG_PATH%/scripts/buildsystems/vcpkg.cmake ..
if %errorlevel% neq 0 (
    echo CMAKE failed
    exit /b 1
) else (
    echo CMAKE successful
)

echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo BUILD failed
    exit /b 1
) else (
    echo BUILD successful
)

echo.
echo Executable should be in: build\Release\factor-e.exe
goto :eof

:cleanup_build
if exist "build" (
    echo Cleaning old build directory...
    rmdir /s /q build
)
goto :eof