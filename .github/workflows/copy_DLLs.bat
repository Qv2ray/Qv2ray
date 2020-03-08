@echo off
echo Copying DLLs for Windows
forfiles /s /p %GITHUB_WORKSPACE%\libs\x86-windows\bin\ /m "*.dll" /c "cmd.exe /c copy @file %GITHUB_WORKSPACE%\build\release\"
forfiles /s /p %GITHUB_WORKSPACE%\libs\x64-windows\bin\ /m "*.dll" /c "cmd.exe /c copy @file %GITHUB_WORKSPACE%\build\release\"
exit 0
