@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "ROOT=%~dp0"
set "ROM=%~1"
if not defined ROM set "ROM=%USERPROFILE%\Decomp\warioland4-2\baserom_us.gba"
if not exist "%ROM%" (
    echo Supported US ROM not found. Drag your baserom_us.gba onto this launcher.
    pause
    exit /b 2
)
if not exist "%ROOT%test-data" mkdir "%ROOT%test-data"
if errorlevel 1 (
    echo Could not create the isolated test-data folder.
    pause
    exit /b 1
)
if not exist "%ROOT%test-data\wl4.sav" if exist "%APPDATA%\wl4\wl4.sav" copy "%APPDATA%\wl4\wl4.sav" "%ROOT%test-data\wl4.sav" >nul
if not exist "%ROOT%test-data\settings.cfg" if exist "%APPDATA%\wl4\settings.cfg" copy "%APPDATA%\wl4\settings.cfg" "%ROOT%test-data\settings.cfg" >nul
set "WL4_SETTINGS_FILE=%ROOT%test-data\settings.cfg"
set "LOG=%ROOT%test-data\run-%RANDOM%-%RANDOM%.log"
"%ROOT%wl4.exe" "%ROM%" --save "%ROOT%test-data\wl4.sav" >"%LOG%" 2>&1
set "STATUS=%ERRORLEVEL%"
echo Game exit code: %STATUS%>>"%LOG%"
echo Game exit code: %STATUS%
echo Log: "%LOG%"
pause
exit /b %STATUS%
