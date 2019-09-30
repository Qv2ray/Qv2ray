#!/bin/bash
brew install grpc
brew install protobuf
ABSPATH=$(cd "$(dirname "$0")"; pwd)
$ABSPATH/grpc_gen.sh
