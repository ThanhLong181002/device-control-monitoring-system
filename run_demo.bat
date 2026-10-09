@echo off
setlocal
cd /d "%~dp0"

set "APP=build\Debug\device_control.exe"

if not exist "%APP%" (
    echo [ERROR] Cannot find "%APP%".
    echo Please build the project first, or run build_and_run.bat.
    pause
    exit /b 1
)

echo ==========================================
echo   DEVICE CONTROL AND MONITORING - DEMO
echo ==========================================
echo Starting server and client in separate windows...
echo.

start "Device Control - Server" cmd /k ""%CD%\%APP%" server"
timeout /t 2 /nobreak >nul
start "Device Control - Client" cmd /k ""%CD%\%APP%" client"

echo.
echo Demo windows opened.
echo Close the client from its menu when finished.
echo Press Ctrl+C in the server window to stop the server.
echo.
pause
endlocal
