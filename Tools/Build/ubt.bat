@echo off
set PROJ=%1
set SHA=%5
if %SHA%== set SHA=nosha
set ENGINE=C:\Program Files\Epic Games\UE_5.8
mkdir "%PROJ%\Saved\Logs" 2>nul
"%ENGINE%\Engine\Build\BatchFiles\Build.bat" %2 %3 %4 -Project="%PROJ%\NAMEC.uproject" -WaitMutex -Log="%PROJ%\Saved\Logs\UBT-%SHA%.log"
exit /b %ERRORLEVEL%
