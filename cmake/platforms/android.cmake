set(ANDROID_LIB_ARCH ${CMAKE_ANDROID_ARCH})
if(${ANDROID_LIB_ARCH} STREQUAL "x86_64")
    set(ANDROID_LIB_ARCH "x64")
endif()
set(QV2RAY_PLATFORM_LIBS_BIN_PREFIX ${CMAKE_SOURCE_DIR}/libs/tools)
set(QV2RAY_PLATFORM_LIBS_PREFIX ${CMAKE_SOURCE_DIR}/libs/${ANDROID_LIB_ARCH}-android)

QVLOG(CMAKE_ANDROID_ARCH)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set (SSL_ROOT_PATH "${CMAKE_SOURCE_DIR}/3rdparty/android_openssl/no-asm/latest")
else()
    set (SSL_ROOT_PATH "${CMAKE_SOURCE_DIR}/3rdparty/android_openssl/latest")
endif()

list(APPEND ANDROID_EXTRA_LIBS "${SSL_ROOT_PATH}/${CMAKE_ANDROID_ARCH}/libcrypto_1_1.so")
list(APPEND ANDROID_EXTRA_LIBS "${SSL_ROOT_PATH}/${CMAKE_ANDROID_ARCH}/libssl_1_1.so")

set(APK_PACKAGE_NAME "net.qv2ray.qv2ray")
set(APK_PACKAGE_VERSION "v${QV2RAY_VERSION_STRING}")
set(APK_DISPLAY_NAME "Qv2ray")
set(APK_PACKAGE_VERSIONCODE 0)
configure_file(${CMAKE_SOURCE_DIR}/assets/AndroidManifest.xml.in ${CMAKE_SOURCE_DIR}/assets/android/AndroidManifest.xml @ONLY)
