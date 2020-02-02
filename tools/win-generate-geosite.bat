@echo off
cd tools
mkdir %~dp0..\libs\gen

echo. ---^> Generating proto file.
%~dp0\..\libs\gRPC-win32\bin\protoc.exe v2ray_geosite.proto --cpp_out=%~dp0..\libs\gen
if errorlevel 1 goto errored

echo DONE
exit 0

:errored
echo %errorlevel%
echo SOME PROCESS FAILED!
exit 1
