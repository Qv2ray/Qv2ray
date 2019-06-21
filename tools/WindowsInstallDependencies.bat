@echo off
REM Install Python

ECHO Downloading Python using wget...
%~dp0\wget.exe https://www.python.org/ftp/python/3.7.3/python-3.7.3.exe -O %~dp0\python-3.7.3.exe

REM Get install path 
CALL :NORMALIZEPATH "\python37\"
ECHO INSTALL_PATH=%RETVAL%

REM Installing Python...
start /w "" "%~dp0\python-3.7.3.exe" /quiet TargetDir=%RETVAL%

:CONTINUE
ECHO Python headers and libs are installed!
exit 0

REM ========================================================================================

REM Path resolving using: https://stackoverflow.com/a/33404867/8364323
:: ========== FUNCTIONS ==========
:NORMALIZEPATH
  SET RETVAL=%~dpfn1
  EXIT /B