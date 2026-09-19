@echo off
set PROJ=%1
set SHA=%3
if %SHA%== set SHA=nosha
set ENGINE=C:\Program Files\Epic Games\UE_5.8
mkdir "%PROJ%\Saved\Automation\%SHA%" 2>nul
"%ENGINE%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "%PROJ%\NAMEC.uproject" -ExecCmds="Automation RunTests %~2;Quit" -nosound -unattended -nullrhi -nosplash -ReportExportPath="%PROJ%\Saved\Automation\%SHA%"
exit /b %ERRORLEVEL%
