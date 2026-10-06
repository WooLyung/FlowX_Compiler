@echo off
setlocal
set "INPUT=%~dp0input.csv"
set "OUTPUT=%~dp0output.csv"
if not "%~1"=="" set "INPUT=%~f1"
if not "%~2"=="" set "OUTPUT=%~f2"
if not exist "%~dp0FlowXProgram.exe" (
    echo Run Build.bat first.
    if not defined FLOWX_NO_PAUSE pause
    exit /b 1
)
"%~dp0FlowXProgram.exe" "%INPUT%" "%OUTPUT%"
set "RESULT=%ERRORLEVEL%"
if "%RESULT%"=="0" echo Output: %OUTPUT%
if not "%RESULT%"=="0" echo Execution failed with exit code %RESULT%.
if not defined FLOWX_NO_PAUSE pause
exit /b %RESULT%
