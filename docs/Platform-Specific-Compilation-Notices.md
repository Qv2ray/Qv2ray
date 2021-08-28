# Linux (libuv, uvw)

- The CMake support provided by `uvw` is not complete and lacks `libdl` linkage when configuring using `USE_SYSTEM_LIBUV` option.

# Linux (Qt)

- As a regression of Qt 6.2 beta2, Vulkan must be installed when configuring a Qt GUI application, so install vulkan and set `VULKAN_SDK` properly. (See [QTBUG-95391](https://bugreports.qt.io/browse/QTBUG-95391))

# macOS (Qt)

- `macdeployqt` is malfunctioning when it sees `libabsl_debugging_internal.dylib` (See [QTBUG-90982](https://bugreports.qt.io/browse/QTBUG-90982))
  - QvPersonal uses static linking (provided by vcpkg) so this won't cause a problem.
  - Pay special attention when using shared(dynamic) gRPC libraries (which depends on libabsl) from homebrew.
- **Qt Network OpenSSL TLS backend plugin** is not shipped Qt, causing TLS 1.3 to be unavailable. (See [QTBUG-95249](https://bugreports.qt.io/browse/QTBUG-95249))

# Windows (Qt)

- `windeployqt` will not copy **Qt Network TLS backend plugins**, so they're copied by CMake, as a result, debugging libraries may be copied.
- As a regression of Qt 6.2 beta2, Vulkan must be installed when configuring a Qt GUI application, so install vulkan and set `VULKAN_SDK` properly. (See [QTBUG-95391](https://bugreports.qt.io/browse/QTBUG-95391))