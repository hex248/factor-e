@echo off

setlocal enabledelayedexpansion

if "%1"=="clean" (
    call :cleanup_build
    echo build directory cleaned.
    exit /b 0
)

if not exist build mkdir build
cd build

cmake ..
if !errorlevel! neq 0 (
    echo CMAKE failed
    exit /b 1
) else (
    echo CMAKE successful
)

cmake --build .
if !errorlevel! neq 0 (
    echo BUILD failed
    exit /b 1
) else (
    echo BUILD successful
)

exit /b 0

:cleanup_build
if exist build (
    echo cleaning old build directory...
    rmdir /s /q build
)
exit /b 0
