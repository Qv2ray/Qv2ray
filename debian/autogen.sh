#!/bin/bash
tools/grpc_gen.sh
git submodule update --init --recursive
source /opt/qt512/bin/qt512-env.sh