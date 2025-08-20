@echo off

if exist "build\Release\factor-e.exe" (
    echo running "Release" version...
    cd build\Release
    factor-e.exe
    cd ../..
) else if exist "build\Debug\factor-e.exe" (
    echo running "Debug" version...
    cd build\Debug
    factor-e.exe
    cd ../..
) else (
    echo error: factor-e.exe not found in "build\Release" or "build\Debug"
    echo make sure you have built the project first with: "build.bat"
    exit /b 1
)