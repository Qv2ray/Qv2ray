#!/bin/bash
cd tools
mkdir -p ../libs/gen
protoc --grpc_out=../libs/gen --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./v2ray_api_commands.proto
protoc --cpp_out=../libs/gen ./v2ray_api_commands.proto

