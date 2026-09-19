@echo off
set PROJ=%1
set SHA=%3
if %SHA%== set SHA=nosha
set ENGINE=C:\Program Files\Epic Games\UE_5.8
mkdir "%PROJ%\Saved\Logs" 2>nul
"%ENGINE%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "%PROJ%\NAMEC.uproject" -run=%2 -unattended -nullrhi -nosplash -Log="%PROJ%\Saved\Logs\Commandlet-%SHA%.log"
exit /b %ERRORLEVEL%
