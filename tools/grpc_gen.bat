@echo off
cd tools
echo Extracting files.
%~dp0\7z.exe -y e %~dp0gRPC-win32.tar.gz -o%~dp0 && %~dp0\7z.exe -y x %~dp0gRPC-win32.tar -o%~dp0\..\libs\gRPC-win32
if errorlevel 1 goto errored

del %~dp0gRPC-win32.tar

mkdir %~dp0..\libs\gen

echo.
echo Generate grpc.pb.h using gRPC and protocol buffer
echo. ---^> Generating gRPC file.
%~dp0\..\libs\gRPC-win32\bin\protoc.exe v2ray_api_commands.proto --grpc_out=%~dp0..\libs\gen --plugin=protoc-gen-grpc="%~dp0..\libs\gRPC-win32\bin\grpc_cpp_plugin.exe" 
if errorlevel 1 goto errored

echo. ---^> Generating proto file.
%~dp0\..\libs\gRPC-win32\bin\protoc.exe v2ray_api_commands.proto --cpp_out=%~dp0..\libs\gen
if errorlevel 1 goto errored

echo DONE
exit 0

:errored
echo %errorlevel%
echo SOME PROCESS FAILED!
exit 1