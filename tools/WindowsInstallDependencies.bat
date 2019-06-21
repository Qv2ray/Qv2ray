@echo off
REM Install Python

ECHO Downloading Python using wget...
wget.exe https://www.python.org/ftp/python/3.7.3/python-3.7.3.exe -O %~dp0\python-3.7.3.exe

REM Get install path 
CALL :NORMALIZEPATH "..\python37\"
ECHO INSTALL_PATH=%RETVAL%

REM Installing Python...
python-3.7.3.exe /quiet TargetDir=%RETVAL%

REM From: https://stackoverflow.com/a/8185270/8364323
ECHO Wait for it installing....
:LOOP
PSLIST python-3.7.3 >nul 2>&1
IF ERRORLEVEL 1 (
  GOTO CONTINUE
) ELSE (
  ECHO Waiting for python finish install.
  TIMEOUT /T 5
  GOTO LOOP
)

:CONTINUE
ECHO Python headers and libs are installed!
exit 0

REM ========================================================================================

REM Path resolving using: https://stackoverflow.com/a/33404867/8364323
:: ========== FUNCTIONS ==========
:NORMALIZEPATH
  SET RETVAL=%~dpfn1
  EXIT /B