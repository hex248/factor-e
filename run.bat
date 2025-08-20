@echo off

if exist "build\Release\factor-e.exe" (
    echo Running Release version...
    cd build\Release
    factor-e.exe
) else if exist "build\Debug\factor-e.exe" (
    echo Running Debug version...
    cd build\Debug
    factor-e.exe
) else (
    echo Error: factor-e.exe not found in build\Release or build\Debug
    echo Make sure you have built the project first with: build.bat
    exit /b 1
)