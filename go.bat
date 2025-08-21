@echo off

set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

call build.bat
if %errorlevel% neq 0 (
    echo %RED%build failed, aborting run%NC%
    exit /b %errorlevel%
)

echo.
call run.bat
