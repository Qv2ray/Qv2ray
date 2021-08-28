# Read Carefully
:warning: **Please read [Platform-specific Compilation Notices](Platform-specific-Compilation-Notices) before trying to build.**

# Dependencies

- Qt 6.2
- OpenSSL
- gRPC
- protobuf
- libuv, uvw (required by Qv2rayBase)
- Qv2rayBase / Qv2rayBase-PluginInterface
- A working C++ compiler with C++17 support

# Compilation Steps

## 1. Obtaining dependencies 

  - Qt
    - May be installed via Qt Online Installer, distribution packages or [`aqtinstall`](https://github.com/miurahr/aqtinstall/)
  - OpenSSL, gRPC, protobuf
    - You can install them via `vcpkg`, `brew` or your Linux distribution's package manager.
  - libuv, uvw
    - You can install them via `vcpkg`, or Linux distribution's package manager.
  - Qv2rayBase, Qv2rayBase-PluginInterface
    - Installing from Linux distribution packages (if exist) is suggested.
    - Otherwise, the git submodule works fine.

## 2. Configuring QvPersonal with CMake

Suppose that:
  - the current working directory contains `CMakeLists.txt`,
  - the build output directory will be at `./build`,
  - the desired installation path is `$INSTALLED`,

So, firstly `mkdir build; cd build` and the initial CMake argument will be `.. -DCMAKE_INSTALL_PREFIX=$INSTALLED`

Then:

1. If your Qt installation is not in a standard search path:
    - append `-DCMAKE_PREFIX_PATH=/path/to/your/qt/installation/`
2. If you are using `vcpkg`
    - append `-DCMAKE_TOOLCHAIN_FILE=/path/to/your/vcpkg/scripts/buildsystems/vcpkg.cmake`
3. If you are using package-manager-provided `libuv` and `uvw`:
    - append `-DUSE_SYSTEM_LIBUV=ON -DUSE_SYSTEM_UVW=ON`
4. If you are using package-manager-provided `Qv2rayBase` and `Qv2rayBase-PluginInterface`:
    - append `-DQV2RAY_QV2RAYBASE_PROVIDER="package"`
5. If you are using package-manager-provided `SingleApplication`:
    - append `-DQV2RAY_SINGLEAPPLICATION_PROVIDER="package"`
6. If you are using `Ninja`
    - append `-GNinja`

## 3. Starting the build

Run: `cmake --build .` in the build directory

- If you want to speed up the compilation using multiple cores, 
    - Add `--parallel CORES` (*where `CORES` is the number of your CPU cores (or `$(nproc)`)*)

## 4. Installing (deployment)

- Simply run `cmake --install .` in the build directory.