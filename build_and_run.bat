@echo off
setlocal
cd /d "%~dp0"

echo ==========================================
echo   BUILD AND RUN DEVICE CONTROL DEMO
echo ==========================================
echo.

where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] CMake was not found in PATH.
    echo Install CMake or open a terminal where CMake is available.
    pause
    exit /b 1
)

echo [1/3] Configuring project...
cmake -S . -B build
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

echo.
echo [2/3] Building Debug configuration...
cmake --build build --config Debug
if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

if not exist "build\Debug\device_control.exe" (
    echo [ERROR] Expected executable was not created:
    echo build\Debug\device_control.exe
    pause
    exit /b 1
)

echo.
echo [3/3] Launching server and client...
start "Device Control - Server" cmd /k ""%CD%\build\Debug\device_control.exe" server"
timeout /t 2 /nobreak >nul
start "Device Control - Client" cmd /k ""%CD%\build\Debug\device_control.exe" client"

echo.
echo Demo windows opened.
echo Close the client from its menu when finished.
echo Press Ctrl+C in the server window to stop the server.
echo.
pause
endlocal
