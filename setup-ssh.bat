@echo off
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo Relaunching as Administrator...
    powershell -Command "Start-Process cmd -ArgumentList '/c cd /d \"%~dp0\" && \"%~f0\"' -Verb RunAs"
    exit /b
)
cd /d "%~dp0"
powershell -ExecutionPolicy Bypass -File "Tools\Dex\setup-ssh-for-dex.ps1"
echo.
pause
