@echo off
setlocal enabledelayedexpansion

set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

set "BUILD_DIR=build-windows"
set "BUILD_SUCCESS=0"
set "TOTAL_WARNINGS=0"

if "%1"=="clean" (
    call :cleanup_build
    echo %GREEN%build directory cleaned.%NC%
    exit /b 0
)

call :print_header "building for windows"

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"

echo running cmake for windows...
cmake .. -DCMAKE_BUILD_TYPE=Release > cmake_windows.log 2>&1
if %errorlevel% neq 0 (
    echo %RED%cmake configuration failed for windows%NC%
    cd ..
    exit /b 1
) else (
    echo %GREEN%cmake configuration successful for windows%NC%
)

echo building for windows...
cmake --build . --config Release > build_windows.log 2>&1
if %errorlevel% neq 0 (
    echo %RED%windows build failed%NC%
    cd ..
    exit /b 1
) else (
    echo %GREEN%windows build successful%NC%
    set "BUILD_SUCCESS=1"
)

for /f %%i in ('findstr /c:"warning:" build_windows.log ^| findstr /v "_deps" ^| find /c /v ""') do set "TOTAL_WARNINGS=%%i"
if %TOTAL_WARNINGS% gtr 0 (
    echo %YELLOW%windows build completed with %TOTAL_WARNINGS% warnings%NC%
)

cd ..

call :print_header "build summary"
echo platform results:
if %BUILD_SUCCESS%==1 (
    echo %GREEN%  windows: success ^(%TOTAL_WARNINGS% warnings^)%NC%
) else (
    echo %RED%  windows: failed%NC%
)

echo.
echo build artifacts:
if exist "%BUILD_DIR%\factor-e.exe" (
    echo   windows executable: %BUILD_DIR%\factor-e.exe
) else if exist "%BUILD_DIR%\Release\factor-e.exe" (
    echo   windows executable: %BUILD_DIR%\Release\factor-e.exe
)

echo.
echo build logs:
echo   windows build log: %BUILD_DIR%\build_windows.log
echo   windows cmake log: %BUILD_DIR%\cmake_windows.log

if %TOTAL_WARNINGS% gtr 0 (
    echo.
    echo %YELLOW%to view warnings in detail:%NC%
    echo   windows warnings: findstr "warning:" %BUILD_DIR%\build_windows.log ^| findstr /v "_deps"
)

if %BUILD_SUCCESS%==1 (
    echo %GREEN%build completed successfully%NC%
    exit /b 0
) else (
    exit /b 1
)

:print_header
echo %BLUE%========================================%NC%
echo %BLUE% %~1 %NC%
echo %BLUE%========================================%NC%
goto :eof

:cleanup_build
echo cleaning old build directory...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
)
goto :eof