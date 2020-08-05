cmake_minimum_required(VERSION 3.10.1)

file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSION" QV2RAY_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/BUILDVERSION" QV2RAY_BUILD_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSIONSUFFIX" QV2RAY_VERSION_SUFFIX)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNODE_DEBUG_DRAWING)
endif()

set(QV2RAY_VERSION_STRING "${QV2RAY_VERSION}${QV2RAY_VERSION_SUFFIX}")
project(qv2ray)

set(VERSION_LIST ${QV2RAY_VERSION})
string(REPLACE "." ";" VERSION_LIST ${VERSION_LIST})
separate_arguments(VERSION_LIST)

list(GET VERSION_LIST 0 CMAKE_PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 CMAKE_PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 CPACK_PACKAGE_VERSION_PATCH)

# Tweaks and other defaults
# Setting CMAKE to use loose block and search for find modules in source directory
set ( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
set ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

# For Windows RC file.
add_definitions(-DQV2RAY_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR})
add_definitions(-DQV2RAY_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR})
add_definitions(-DQV2RAY_VERSION_BUGFIX=${CPACK_PACKAGE_VERSION_PATCH})
add_definitions(-DQV2RAY_VERSION_BUILD=${QV2RAY_BUILD_VERSION})

add_definitions(-DQV2RAY_VERSION_STRING="${QV2RAY_VERSION_STRING}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(ANDROID)
    include(${ANDROID_SDK}/android_openssl/CMakeLists.txt)
endif()

set(QV2RAY_ONLY_USE_QML OFF CACHE BOOL "Only use QML when building Qv2ray")

if(QV2RAY_ONLY_USE_QML)
    find_package(Qt5 5.11 COMPONENTS Core Gui Network Qml Quick REQUIRED)
    set(QV2RAY_QT_LIBS Qt5::Core Qt5::Network Qt5::Quick Qt5::Qml Qt5::Gui)
    add_definitions(-DQV2RAY_QML)
else()
    find_package(Qt5 5.11 COMPONENTS Core Gui Widgets Network REQUIRED)
    set(QV2RAY_QT_LIBS Qt5::Core Qt5::Network Qt5::Widgets Qt5::Gui)
    add_definitions(-DQV2RAY_QWIDGETS)
endif()

find_package(Threads REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

cmake_policy(SET CMP0071 NEW)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.17.0")
    cmake_policy(SET CMP0100 NEW)
endif()

message(" ")
message("Qv2ray Version: ${QV2RAY_VERSION_STRING}")
message("Qv2ray Build Version: ${QV2RAY_BUILD_VERSION}")
message("|-------------------------------------------------|")
message("| Qv2ray, A Cross Platform v2ray Qt GUI Client.   |")
message("| Licenced under GPLv3                            |")
message("|                                                 |")
message("| You may only use this program to the extent     |")
message("| permitted by local law.                         |")
message("|                                                 |")
message("| See: https://www.gnu.org/licenses/gpl-3.0.html  |")
message("|-------------------------------------------------|")
message("| Project Homepage: https://github.com/Qv2ray     |")
message("| Welcome to contribute!                          |")
message("|-------------------------------------------------|")
message(" ")

if(WIN32)
    add_compile_options("/std:c++17")
    add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x600 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
    set(GUI_TYPE WIN32)
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        if(CMAKE_CL_64)
            include(${CMAKE_SOURCE_DIR}/libs/x64-windows/scripts/buildsystems/vcpkg.cmake)
        else()
            include(${CMAKE_SOURCE_DIR}/libs/x86-windows/scripts/buildsystems/vcpkg.cmake)
        endif()
    endif()
endif()

# ==================================================================================
# Qv2ray Build Arguments
# ==================================================================================
set(QV2RAY_QNODEEDITOR_PROVIDER "module" CACHE STRING "qnodeeditor provider")
set(QV2RAY_ZXING_PROVIDER "module" CACHE STRING "zxing-cpp provider")
if (ANDROID)
else()
    set(QV2RAY_SINGLEAPPLICATION_PROVIDER "module" CACHE STRING "SingleApplication provider")
endif()
set(QV2RAY_DEFAULT_VASSETS_PATH "unset" CACHE STRING "v2ray assets path")
set(QV2RAY_DEFAULT_VCORE_PATH "unset" CACHE STRING "v2ray core path")
set(QV2RAY_TRANSLATION_PATH "unset" CACHE STRING "Qv2ray translations path")
set(QV2RAY_DISABLE_AUTO_UPDATE OFF CACHE BOOL "Disable update checker")
set(BUILD_TESTING OFF CACHE BOOL "Build test")
set(EMBED_TRANSLATIONS OFF CACHE BOOL "Embed translations")
set(QV2RAY_AUTO_DEPLOY ON CACHE BOOL "Automatically run deploy command after build")
set(QV2RAY_HAS_BUILT_IN_THEMES ON CACHE BOOL "Build with built-in themes")

if(QV2RAY_DEFAULT_VASSETS_PATH AND NOT QV2RAY_DEFAULT_VASSETS_PATH STREQUAL "unset")
    add_definitions(-DQV2RAY_DEFAULT_VASSETS_PATH="${QV2RAY_DEFAULT_VASSETS_PATH}")
endif()

if(QV2RAY_DEFAULT_VCORE_PATH AND NOT QV2RAY_DEFAULT_VCORE_PATH STREQUAL "unset")
    add_definitions(-DQV2RAY_DEFAULT_VCORE_PATH="${QV2RAY_DEFAULT_VCORE_PATH}")
endif()

if(QV2RAY_TRANSLATION_PATH AND NOT QV2RAY_TRANSLATION_PATH STREQUAL "unset")
    add_definitions(-DQV2RAY_TRANSLATION_PATH="${QV2RAY_TRANSLATION_PATH}")
endif()

if(QV2RAY_DISABLE_AUTO_UPDATE)
    add_definitions(-DDISABLE_AUTO_UPDATE)
endif()

if(FALL_BACK_TO_XDG_OPEN)
    add_definitions(-DFALL_BACK_TO_XDG_OPEN)
endif()

if(EMBED_TRANSLATIONS)
    add_definitions(-DEMBED_TRANSLATIONS)
    configure_file(translations/translations.qrc ${CMAKE_BINARY_DIR} COPYONLY)
    set(QV2RAY_EMBED_TRANSLATION_QRC ${CMAKE_BINARY_DIR}/translations.qrc)
endif()


# ==================================================================================
# 3rdparty Sources
# ==================================================================================
include(cmake/libuv.cmake)
include(cmake/libcurl.cmake)
include(cmake/translations.cmake)
include(cmake/qnodeeditor.cmake)
if (ANDROID)
else()
    include(cmake/singleapplication.cmake)
    include(cmake/protobuf.cmake)
    include(cmake/backend.cmake)
endif()
include(cmake/zxing-cpp.cmake)
include(cmake/libsemver.cmake)


# ==================================================================================
# Qv2ray Build Info
# ==================================================================================
if(QV2RAY_BUILD_INFO)
    set(_QV2RAY_BUILD_INFO_STR_ "${QV2RAY_BUILD_INFO}")
elseif(DEFINED ENV{_QV2RAY_BUILD_INFO_})
    set(_QV2RAY_BUILD_INFO_STR_ "$ENV{_QV2RAY_BUILD_INFO_}")
else()
    set(_QV2RAY_BUILD_INFO_STR_ "Qv2ray from manual build")
endif()

if(QV2RAY_BUILD_EXTRA_INFO)
    set(_QV2RAY_BUILD_EXTRA_INFO_STR_ "${QV2RAY_BUILD_EXTRA_INFO}")
elseif(DEFINED ENV{_QV2RAY_BUILD_EXTRA_INFO_})
    set(_QV2RAY_BUILD_EXTRA_INFO_STR_ "$ENV{_QV2RAY_BUILD_EXTRA_INFO_}")
else()
    set(_QV2RAY_BUILD_EXTRA_INFO_STR_ "Qv2ray ${QV2RAY_VERSION_STRING}:${QV2RAY_BUILD_VERSION}")
endif()

set(QV2RAY_BUILD_INFO ${_QV2RAY_BUILD_INFO_STR_})
set(QV2RAY_BUILD_EXTRA_INFO ${_QV2RAY_BUILD_EXTRA_INFO_STR_})

add_definitions(-D_QV2RAY_BUILD_INFO_STR_="${_QV2RAY_BUILD_INFO_STR_}")
add_definitions(-D_QV2RAY_BUILD_EXTRA_INFO_STR_="${_QV2RAY_BUILD_EXTRA_INFO_STR_}")
message("Qv2ray build info: ${_QV2RAY_BUILD_INFO_STR_}")
message("Qv2ray build info ex: ${_QV2RAY_BUILD_EXTRA_INFO_STR_}")


# ==================================================================================
# Plugin Interface Headers
# ==================================================================================
set(QVPLUGIN_INTERFACE_INCLUDE_DIR "src/plugin-interface")
include(src/plugin-interface/QvPluginInterface.cmake)


# ==================================================================================
# Qv2ray Sources and Headers
# ==================================================================================
include(cmake/components/qv2ray-lib.cmake)
include(cmake/components/qv2ray-ui.cmake)

if (QV2RAY_HAS_BUILT_IN_THEMES)
    include(libs/uistyles/uistyles.cmake)
endif()

set(QRC_RESOURCES
    ${UISTYLE_QRCS}
    ${CMAKE_SOURCE_DIR}/resources.qrc)

# Qv2ray baselib
add_library(qv2ray-baselib STATIC
    ${QV2RAY_BASE_HEADERS}
    ${QV2RAY_LIB_SOURCES}
    ${QVPLUGIN_INTERFACE_HEADERS}
    ${LIBSEMVER_SOURCES}
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    ${API_GRPC_SRCS}
    ${API_PROTO_SRCS}
    )

set(QV2RAY_FULL_SOURCES
    src/main.cpp
    assets/qv2ray.rc
    ${QV2RAY_UI_FORMS}
    ${QV2RAY_UI_SOURCES}
    ${SINGLEAPPLICATION_SOURCES}
    ${QNODEEDITOR_QRC_RESOURCES}
    ${QV2RAY_EMBED_TRANSLATION_QRC}
    ${QRC_RESOURCES}
    ${QM_FILES})

if(ANDROID)
    add_library(qv2ray SHARED ${QV2RAY_FULL_SOURCES})
    target_link_libraries(qv2ray -llog -landroid)
else()
    add_executable(qv2ray ${GUI_TYPE} ${QV2RAY_FULL_SOURCES})
endif()


target_link_libraries(qv2ray-baselib
    ${QV2RAY_PROTOBUF_LIBRARY}
    ${QV2RAY_BACKEND_LIBRARIES}
    ${ZXING_LIBRARY}
    Threads::Threads
    ${QV2RAY_QT_LIBS}
    ${LibUV_LIBRARIES}
    ${CURL_LIBRARIES}
    )

target_link_libraries(qv2ray
    qv2ray-baselib
    ${QNODEEDITOR_LIBRARY}
    ${SINGLEAPPLICATION_LIBRARY}
    )

target_include_directories(qv2ray PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
    ${QNODEEDITOR_INCLUDE_PATH}
    ${SINGLEAPPLICATION_DIR}
    ${Protobuf_INCLUDE_DIRS}
    )

target_include_directories(qv2ray-baselib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${ZXING_INCLUDE_PATH}
    ${Protobuf_INCLUDE_DIRS}
    ${LibUV_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    )

if (BUILD_TESTING)
    include(CTest)
    add_subdirectory(test)
endif()

# Qt language translations
add_custom_target(lupdate
    COMMAND lupdate ${QV2RAY_BASE_HEADERS}
    ${QV2RAY_LIB_SOURCES}
    ${QVPLUGIN_INTERFACE_HEADERS}
    ${QV2RAY_UI_SOURCES}
    ${QV2RAY_UI_FORMS}
    ${QNODEEDITOR_QRC_RESOURCES}
    ${SINGLEAPPLICATION_SOURCES} -ts ${TRANSLATIONS_TS} -locations none -noobsolete
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
set_target_properties(lupdate PROPERTIES EXCLUDE_FROM_ALL TRUE)

if(APPLE)
    find_package(Iconv REQUIRED)
    find_library(CARBON NAMES Carbon)
    find_library(COCOA NAMES Cocoa)
    find_library(SECURITY NAMES Security)
    target_link_libraries(qv2ray
        Iconv::Iconv
        ${CARBON}
        ${COCOA}
        ${SECURITY}
        )
    target_include_directories(qv2ray PRIVATE
        ${Iconv_INCLUDE_DIR}
        )

    set(MACOSX_ICON "${CMAKE_SOURCE_DIR}/assets/icons/qv2ray.icns")
    set(MACOSX_PLIST "${CMAKE_SOURCE_DIR}/assets/MacOSXBundleInfo.plist.in")
    set_source_files_properties(${QM_FILES}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources/lang
        )

    target_sources(qv2ray PRIVATE
        ${MACOSX_ICON}
        )

    set_target_properties(qv2ray
        PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${MACOSX_PLIST}
        MACOSX_BUNDLE_BUNDLE_NAME "Qv2ray"
        MACOSX_BUNDLE_BUNDLE_VERSION ${QV2RAY_VERSION_STRING}
        MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2019-2020 Qv2ray Development Group"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.qv2ray"
        MACOSX_BUNDLE_ICON_FILE "qv2ray.icns"
        MACOSX_BUNDLE_INFO_STRING "Created by Qv2ray development team"
        MACOSX_BUNDLE_LONG_VERSION_STRING ${QV2RAY_VERSION_STRING}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${QV2RAY_VERSION_STRING}
        RESOURCE
        ${MACOSX_ICON}
        )

    # Destination paths below are relative to ${CMAKE_INSTALL_PREFIX}

    install(TARGETS qv2ray
        BUNDLE DESTINATION . COMPONENT Runtime
        RUNTIME DESTINATION bin COMPONENT Runtime
        )
    if(QV2RAY_AUTO_DEPLOY)
        add_custom_command(TARGET qv2ray POST_BUILD
            COMMAND ${Qt5_DIR}/../../../bin/macdeployqt ${CMAKE_BINARY_DIR}/qv2ray.app
            )
    endif()
    set(APPS "\${CMAKE_INSTALL_PREFIX}/qv2ray.app")
    include(cmake/deployment.cmake)
endif()

if(UNIX AND NOT APPLE AND NOT WIN32)
    set(QV2RAY_LINUX_ICON_DIMENSIONS 16 22 32 48 64 128 256 512 1024)

    install(TARGETS qv2ray RUNTIME DESTINATION bin)
    install(FILES assets/qv2ray.metainfo.xml DESTINATION share/metainfo)
    install(FILES assets/qv2ray.desktop DESTINATION share/applications)
    install(FILES assets/icons/qv2ray.svg DESTINATION share/icons/hicolor/scalable/apps)
    foreach(QV2RAY_LINUX_ICON_DIMENSION ${QV2RAY_LINUX_ICON_DIMENSIONS})
        install(FILES assets/icons/qv2ray.${QV2RAY_LINUX_ICON_DIMENSION}.png DESTINATION share/icons/hicolor/${QV2RAY_LINUX_ICON_DIMENSION}x${QV2RAY_LINUX_ICON_DIMENSION}/apps RENAME qv2ray.png)
    endforeach(QV2RAY_LINUX_ICON_DIMENSION)
    if(NOT EMBED_TRANSLATIONS)
        install(FILES ${QM_FILES} DESTINATION share/qv2ray/lang)
    endif()
endif()

if(WIN32)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(qv2ray-baselib wininet wsock32 ws2_32 user32 Iphlpapi OpenSSL::SSL OpenSSL::Crypto Dbghelp)
    install(TARGETS qv2ray RUNTIME DESTINATION .)
    if(NOT EMBED_TRANSLATIONS)
        install(FILES ${QM_FILES} DESTINATION lang)
    endif()
    if(QV2RAY_AUTO_DEPLOY)
        add_custom_command(TARGET qv2ray POST_BUILD
            COMMAND ${Qt5_DIR}/../../../bin/windeployqt ${CMAKE_BINARY_DIR}/qv2ray.exe --compiler-runtime --verbose 2 --dir ${CMAKE_BINARY_DIR}/winqt/
            )
    endif()
    install(DIRECTORY ${CMAKE_BINARY_DIR}/winqt/ DESTINATION .)
    set(APPS "\${CMAKE_INSTALL_PREFIX}/qv2ray.exe")
    include(cmake/deployment.cmake)
endif()
