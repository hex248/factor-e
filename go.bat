@echo off

call build.bat
if %errorlevel% neq 0 (
    echo build failed, aborting run
    exit /b %errorlevel%
)

call run.bat
