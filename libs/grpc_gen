#!/bin/bash
protoc --grpc_out=./ --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./src/utils/gRPC/command.proto
protoc --cpp_out=./ ./src/utils/gRPC/command.proto

