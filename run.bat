@echo off

set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

set "BUILD_DIR=build-windows"

if exist "%BUILD_DIR%\factor-e.exe" (
    echo %GREEN%running windows version...%NC%
    cd "%BUILD_DIR%"
    factor-e.exe
    cd ..
) else if exist "%BUILD_DIR%\Release\factor-e.exe" (
    echo %GREEN%running windows version...%NC%
    cd "%BUILD_DIR%\Release"
    factor-e.exe
    cd ..\..
) else (
    echo %RED%error: factor-e.exe not found in "%BUILD_DIR%"%NC%
    echo %YELLOW%make sure you have built the project first with: "build.bat"%NC%
    exit /b 1
)