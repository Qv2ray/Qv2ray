# Building Qv2ray

Qv2ray is a cross platform v2ray GUI, so you can run your build on all desktop platforms.

## Obtaining Source Code

- 

## Dependencies 

- Qt version >= 5
  - Qt version 5.12 and 5.13 is recommended
  - However, you may build it against 5.11 even 5.9.
- gRPC & protobuf
  - It's now required if you want to use the `dev` version. There's no such dependencies in the `version-v1` branch.
- OpenSSL
  - The build may **not** fail if you don't have it since it's a **runtime dependency**.

## Build instructions

```bash
#！/bin/bash

# Recursively clone the project since we have a submodule. 
git clone --recursive https://github.com/lhy0403/Qv2ray
cd Qv2ray
./tools/gen_grpc.sh
mkdir build && cd build
qmake ..
make -j4
```

