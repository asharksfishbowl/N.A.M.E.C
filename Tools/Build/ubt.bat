@echo off
set PROJ=%1
set SHA=%5
if %SHA%== set SHA=nosha
set ENGINE=C:\Program Files\Epic Games\UE_5.8
mkdir "%PROJ%\Saved\Logs" 2>nul
rem ⛔ Never run two SSH builds concurrently — cross-account mutex does not queue.
"%ENGINE%\Engine\Build\BatchFiles\Build.bat" %2 %3 %4 -Project="%PROJ%\NAMEC.uproject" -WaitMutex -NoHotReloadFromIDE -Log="%PROJ%\Saved\Logs\UBT-%SHA%.log"
exit /b %ERRORLEVEL%
