@echo off
echo Copying Windows Necessary DLLs
forfiles /s /p %GITHUB_WORKSPACE%\libs\ /m "*.dll" /c "cmd.exe /c copy @file %GITHUB_WORKSPACE%\build\release\"
forfiles /s /p %GITHUB_WORKSPACE%\libs\ /m "*.dll" /c "cmd.exe /c copy @file %GITHUB_WORKSPACE%\build\debug\"
exit 0
