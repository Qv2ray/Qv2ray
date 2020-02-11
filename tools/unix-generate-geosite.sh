#!/bin/bash
cd $1/../tools
mkdir -p ../libs/gen
protoc --cpp_out=../libs/gen ./v2ray_geosite.proto
