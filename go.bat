@echo off

call build.bat
if %errorlevel% neq 0 (
    echo Build failed, aborting run
    exit /b %errorlevel%
)

call run.bat
