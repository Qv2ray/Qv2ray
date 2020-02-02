#!/bin/bash
cd tools
mkdir -p ../libs/gen
protoc --cpp_out=../libs/gen ./v2ray_geosite.proto

