cmake_minimum_required(VERSION 3.10.1)

file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSION" QV2RAY_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/BUILDVERSION" QV2RAY_BUILD_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSIONSUFFIX" QV2RAY_VERSION_SUFFIX)

set(QV2RAY_VERSION_STRING "${QV2RAY_VERSION}${QV2RAY_VERSION_SUFFIX}")
project(qv2ray)

macro(QVLOG MSG)
    set(QV2RAY_BUILD_STATS "${QV2RAY_BUILD_STATS}${MSG}: ${${MSG}}\r\n")
endmacro()

set(VERSION_LIST ${QV2RAY_VERSION})
string(REPLACE "." ";" VERSION_LIST ${VERSION_LIST})
separate_arguments(VERSION_LIST)

list(GET VERSION_LIST 0 CMAKE_PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 CMAKE_PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 CPACK_PACKAGE_VERSION_PATCH)

add_definitions(-DQV2RAY_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR})
add_definitions(-DQV2RAY_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR})
add_definitions(-DQV2RAY_VERSION_BUGFIX=${CPACK_PACKAGE_VERSION_PATCH})
add_definitions(-DQV2RAY_VERSION_BUILD=${QV2RAY_BUILD_VERSION})
add_definitions(-DQV2RAY_VERSION_STRING="${QV2RAY_VERSION_STRING}")

# Tweaks and other defaults
# Setting CMAKE to use loose block and search for find modules in source directory
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(ANDROID)
    message("Android ABI: ${CMAKE_ANDROID_ARCH}")
    message("Android SDK: ${ANDROID_SDK}")
    include(cmake/platforms/android.cmake)
    include(cmake/platforms/prefixes.cmake)
endif()

option(USE_MINGW "Use MinGW on Windows" OFF)

if(WIN32)
    include(cmake/versioninfo/generate_product_version.cmake)
    generate_product_version(
        QV2RAY_RC
        NAME               "Qv2ray"
        BUNDLE             "Qv2ray Project Family"
        ICON               "${CMAKE_SOURCE_DIR}/assets/icons/qv2ray.ico"
        VERSION_MAJOR      ${CMAKE_PROJECT_VERSION_MAJOR}
        VERSION_MINOR      ${CMAKE_PROJECT_VERSION_MINOR}
        VERSION_PATCH      ${CPACK_PACKAGE_VERSION_PATCH}
        VERSION_REVISION   ${QV2RAY_BUILD_VERSION}
        COMPANY_NAME       "Qv2ray Workgroup"
        COMPANY_COPYRIGHT  "Qv2ray Workgroup 2021"
        FILE_DESCRIPTION   "Qv2ray Main Application"
        )
    add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)
    set(GUI_TYPE WIN32)
    if(USE_MINGW)
        if(NOT DEFINED MinGW_ROOT)
            set(MinGW_ROOT "C:/msys64/mingw64")
        endif()
        set(QV2RAY_PLATFORM_LIBS_BIN_PREFIX ${MinGW_ROOT}/bin)
        set(QV2RAY_PLATFORM_LIBS_PREFIX ${MinGW_ROOT})
        set(QV2RAY_PLATFORM_LIBS_NO_DEBUG_SUBDIR ON)
    else()
        add_compile_options("/utf-8")
        add_compile_options("/std:c++17")
        add_definitions(-D_WIN32_WINNT=0x600 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
        if(CMAKE_CL_64)
            set(QV2RAY_PLATFORM_LIBS_PREFIX ${CMAKE_SOURCE_DIR}/libs/x64-windows/)
        else()
            set(QV2RAY_PLATFORM_LIBS_PREFIX ${CMAKE_SOURCE_DIR}/libs/x86-windows/)
        endif()
    endif()
    include(cmake/platforms/prefixes.cmake)
endif()

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

# ==================================================================================
# Qv2ray compile arguments
# ==================================================================================
option(QV2RAY_AUTO_DEPLOY "Automatically run deploy command after build" ON)
QVLOG(QV2RAY_AUTO_DEPLOY)

option(BUILD_TESTING "Build Tests" OFF)
QVLOG(BUILD_TESTING)

set(QV2RAY_DEFAULT_VASSETS_PATH "unset" CACHE STRING "Default V2Ray assets path")
QVLOG(QV2RAY_DEFAULT_VASSETS_PATH)

set(QV2RAY_DEFAULT_VCORE_PATH "unset" CACHE STRING "Default V2Ray core path")
QVLOG(QV2RAY_DEFAULT_VCORE_PATH)

option(QV2RAY_DISABLE_AUTO_UPDATE "Disable Update Checker" OFF)
QVLOG(QV2RAY_DISABLE_AUTO_UPDATE)

option(QV2RAY_HAS_BUILTIN_PLUGINS "Build with builtin plugins" ON)
QVLOG(QV2RAY_HAS_BUILTIN_PLUGINS)

option(QV2RAY_EMBED_TRANSLATIONS "Embed Translations" OFF)
option(QV2RAY_HAS_SINGLEAPPLICATION "Build With SingleApplication" ON)

set(QV2RAY_UI_TYPE "QWidget" CACHE STRING "Qv2ray GUI Component")
QVLOG(QV2RAY_UI_TYPE)

option(QV2RAY_USE_V5_CORE "Use V5 Core" ON)
QVLOG(QV2RAY_USE_V5_CORE)

option(QV2RAY_QT6 "Use Qt6" OFF)

if(QV2RAY_UI_TYPE STREQUAL "QWidget")
    set(QV2RAY_USE_QWIDGET ON)
    set(QV2RAY_USE_QML OFF)
elseif(QV2RAY_UI_TYPE STREQUAL "QML")
    set(QV2RAY_USE_QWIDGET OFF)
    set(QV2RAY_USE_QML ON)
elseif(QV2RAY_UI_TYPE STREQUAL "CLI")
    set(QV2RAY_USE_QWIDGET OFF)
    set(QV2RAY_USE_QML OFF)
else()
    message(FATAL_ERROR "The given QV2RAY_UI_TYPE: ${QV2RAY_UI_TYPE} is invalid, expected: QWidget, QML, CLI")
endif()

if(ANDROID)
    set(QV2RAY_EMBED_TRANSLATIONS ON)
    set(QV2RAY_USE_QML ON)
    set(QV2RAY_USE_QWIDGET OFF)
    set(QV2RAY_HAS_SINGLEAPPLICATION OFF)
    set(QV2RAY_QT6 ON)
    message("-- Use Qt6 for Android build.")
endif()

QVLOG(QV2RAY_EMBED_TRANSLATIONS)
QVLOG(QV2RAY_USE_QML)
QVLOG(QV2RAY_USE_QWIDGET)
QVLOG(QV2RAY_HAS_SINGLEAPPLICATION)

if(QV2RAY_HAS_SINGLEAPPLICATION)
    set(QV2RAY_SINGLEAPPLICATION_PROVIDER "module" CACHE STRING "SingleApplication Provider")
    QVLOG(QV2RAY_SINGLEAPPLICATION_PROVIDER)
endif()

if(QV2RAY_USE_QML)
    set(QV2RAY_QT6 ON)
    message("-- Use Qt6 for QML build.")
    option(QV2RAY_QML_LIVE_UPDATE "Use QMLLive for live updated QML" OFF)
    QVLOG(QV2RAY_QML_LIVE_UPDATE)
elseif(QV2RAY_USE_QWIDGET)
    option(QV2RAY_HAS_BUILTIN_THEMES "Build with built-in themes" ON)
    set(QV2RAY_QNODEEDITOR_PROVIDER "module" CACHE STRING "QNodeEditor Provider")
    QVLOG(QV2RAY_HAS_BUILTIN_THEMES)
    QVLOG(QV2RAY_QNODEEDITOR_PROVIDER)
endif()

QVLOG(QV2RAY_QT6)

if(QV2RAY_QT6)
    cmake_policy(SET CMP0072 NEW)
    set(QV_QT_MAJOR_VERSION 6)
    set(QV_QT_MINOR_VERSION 0)
    set(QV_QT_LIBNAME Qt6)
    add_definitions(-DQV2RAY_QT6=1)
else()
    set(QV_QT_MAJOR_VERSION 5)
    set(QV_QT_MINOR_VERSION 11)
    set(QV_QT_LIBNAME Qt5)
endif()


# ==================================================================================
# Default Core/Assets Path
# ==================================================================================
if(QV2RAY_DEFAULT_VCORE_PATH AND NOT QV2RAY_DEFAULT_VCORE_PATH STREQUAL "unset")
    add_definitions(-DQV2RAY_DEFAULT_VCORE_PATH="${QV2RAY_DEFAULT_VCORE_PATH}")
endif()
if(QV2RAY_DEFAULT_VASSETS_PATH AND NOT QV2RAY_DEFAULT_VASSETS_PATH STREQUAL "unset")
    add_definitions(-DQV2RAY_DEFAULT_VASSETS_PATH="${QV2RAY_DEFAULT_VASSETS_PATH}")
endif()

# ==================================================================================
# Embed Translations, Translation Search Path
# ==================================================================================
set(QV2RAY_TRANSLATION_PATH "unset" CACHE STRING "Qv2ray translations path")
if(QV2RAY_TRANSLATION_PATH AND NOT QV2RAY_TRANSLATION_PATH STREQUAL "unset")
    add_definitions(-DQV2RAY_TRANSLATION_PATH="${QV2RAY_TRANSLATION_PATH}")
endif()
if(QV2RAY_EMBED_TRANSLATIONS)
    add_definitions(-DQV2RAY_EMBED_TRANSLATIONS)
    configure_file(translations/translations.qrc ${CMAKE_BINARY_DIR} COPYONLY)
    set(QV2RAY_EMBED_TRANSLATION_QRC ${CMAKE_BINARY_DIR}/translations.qrc)
endif()

# ==================================================================================
# Disable Auto Update
# ==================================================================================
if(QV2RAY_DISABLE_AUTO_UPDATE)
    add_definitions(-DDISABLE_AUTO_UPDATE)
endif()

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
    set(_QV2RAY_BUILD_EXTRA_INFO_STR_ "${QV2RAY_VERSION_STRING}:${QV2RAY_BUILD_VERSION}")
endif()

set(QV2RAY_BUILD_INFO ${_QV2RAY_BUILD_INFO_STR_})
set(QV2RAY_BUILD_EXTRA_INFO ${_QV2RAY_BUILD_EXTRA_INFO_STR_})

add_definitions(-D_QV2RAY_BUILD_INFO_STR_="${_QV2RAY_BUILD_INFO_STR_}")
add_definitions(-D_QV2RAY_BUILD_EXTRA_INFO_STR_="${_QV2RAY_BUILD_EXTRA_INFO_STR_}")
message("Qv2ray Version: ${_QV2RAY_BUILD_INFO_STR_} - ${_QV2RAY_BUILD_EXTRA_INFO_STR_}")

# ==================================================================================
# 3rdparty Sources
# ==================================================================================
include(cmake/libuv.cmake)
include(cmake/libcurl.cmake)
include(cmake/libsemver.cmake)
include(cmake/protobuf.cmake)
include(cmake/backend.cmake)
include(3rdparty/QJsonStruct/QJsonStruct.cmake)

# ==================================================================================
# Qv2ray Base, Qt Libraries, Qv2ray GUI Libraries, libThreads
# ==================================================================================
find_package(Threads REQUIRED)
find_package(${QV_QT_LIBNAME} ${QV_QT_MAJOR_VERSION}.${QV_QT_MINOR_VERSION} COMPONENTS Core Network REQUIRED)
list(APPEND QV2RAY_QT_LIBS ${QV_QT_LIBNAME}::Core ${QV_QT_LIBNAME}::Network)

cmake_policy(SET CMP0071 NEW)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(QV2RAY_USE_QML)
    find_package(${QV_QT_LIBNAME} ${QV_QT_MAJOR_VERSION}.${QV_QT_MINOR_VERSION} COMPONENTS Qml Quick Widgets Svg QuickControls2 Gui REQUIRED)
    list(APPEND QV2RAY_QT_LIBS
        ${QV_QT_LIBNAME}::Quick
        ${QV_QT_LIBNAME}::Qml
        ${QV_QT_LIBNAME}::Widgets
        ${QV_QT_LIBNAME}::Svg
        ${QV_QT_LIBNAME}::QuickControls2
        ${QV_QT_LIBNAME}::Gui)
    set(_QV2RAY_HAS_GUI_INTERNAL_ ON)
elseif(QV2RAY_USE_QWIDGET)
    find_package(${QV_QT_LIBNAME} ${QV_QT_MAJOR_VERSION}.${QV_QT_MINOR_VERSION} COMPONENTS Widgets Svg Gui REQUIRED)
    list(APPEND QV2RAY_QT_LIBS
        ${QV_QT_LIBNAME}::Widgets
        ${QV_QT_LIBNAME}::Svg
        ${QV_QT_LIBNAME}::Gui)
    set(_QV2RAY_HAS_GUI_INTERNAL_ ON)
else()
endif()

# Qv2ray baselib sources
include(cmake/components/qv2ray-base.cmake)
include(src/plugin-interface/QvPluginInterface.cmake)

if(QV2RAY_HAS_SINGLEAPPLICATION)
    include(cmake/singleapplication.cmake)
else()
    add_definitions(-DQV2RAY_NO_SINGLEAPPLICATON)
endif()

add_library(qv2ray_baselib STATIC
    ${QV2RAY_BASE_SOURCES}
    ${SINGLEAPPLICATION_SOURCES}
    ${QVPLUGIN_INTERFACE_HEADERS}
    ${LIBSEMVER_SOURCES}
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    ${API_GRPC_SRCS}
    ${API_PROTO_SRCS}
    )

target_link_libraries(qv2ray_baselib
    ${QV2RAY_QT_LIBS}
    ${QV2RAY_PROTOBUF_LIBRARY}
    ${QV2RAY_BACKEND_LIBRARY}
    ${LibUV_LIBRARIES}
    ${CURL_LIBRARIES}
    ${SINGLEAPPLICATION_LIBRARY}
    Threads::Threads
    )

target_include_directories(qv2ray_baselib PUBLIC
    ${CMAKE_BINARY_DIR}
    ${QV2RAY_BASEDIR_COMPONENTS}
    ${LibUV_INCLUDE_DIR}
    ${SINGLEAPPLICATION_DIR}
    ${CURL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Protobuf_INCLUDE_DIRS}
    )

if(QV2RAY_USE_V5_CORE)
    message("-- Use V2Ray Core V5")
    target_compile_definitions(qv2ray_baselib PUBLIC QV2RAY_USE_V5_CORE)
endif()

# ==================================================================================
# Qv2ray Builtin Plugins
# ==================================================================================
if(QV2RAY_HAS_BUILTIN_PLUGINS)
    include(src/plugins/protocols/QvPlugin-BuiltinProtocolSupport.cmake)
    include(src/plugins/subscription-adapters/QvPlugin-BuiltinSubscriptionAdapters.cmake)
    #include(src/plugins/utils/QvPlugin-BuiltinUtils.cmake)
endif()

# ==================================================================================
# Qv2ray UI Frontend
# ==================================================================================
if(_QV2RAY_HAS_GUI_INTERNAL_)
    add_definitions(-DQV2RAY_GUI)
    include(cmake/qrencode.cmake)

    include(cmake/components/qv2ray-ui.cmake)
    list(APPEND QV2RAY_UI_SOURCES ${QV2RAY_UI_COMMON_SOURCES})

    include(src/plugin-interface/QvGUIPluginInterface.cmake)
    list(APPEND QV2RAY_UI_SOURCES ${QVGUIPLUGIN_INTERFACE_HEADERS})

    if(QV2RAY_USE_QML)
        add_definitions(-DQV2RAY_GUI_QML)
        include(cmake/components/qv2ray-ui-qml.cmake)
        list(APPEND QV2RAY_UI_SOURCES ${QV2RAY_QML_SOURCES})
    elseif(QV2RAY_USE_QWIDGET)
        add_definitions(-DQV2RAY_GUI_QWIDGETS)
        include(cmake/components/qv2ray-ui-widget.cmake)
        list(APPEND QV2RAY_UI_SOURCES ${QV2RAY_UI_WIDGET_SOURCES})
        #
        if (QV2RAY_HAS_BUILTIN_THEMES)
            include(3rdparty/uistyles/uistyles.cmake)
            list(APPEND QV2RAY_QRC_RESOURCES ${UISTYLE_QRCS})
        endif()
        #
        include(cmake/qnodeeditor.cmake)
        list(APPEND QV2RAY_QRC_RESOURCES ${QNODEEDITOR_QRC_RESOURCES})
    endif()
else()
    add_definitions(-DQV2RAY_CLI)
    include(cmake/components/qv2ray-cli.cmake)
    list(APPEND QV2RAY_UI_SOURCES ${QV2RAY_CLI_SOURCES})
endif()

# Platform Dependent Sources
include(cmake/components/qv2ray-platform.cmake)
include(cmake/translations.cmake)

list(APPEND QV2RAY_QRC_RESOURCES
    ${CMAKE_SOURCE_DIR}/resources.qrc
    ${CMAKE_SOURCE_DIR}/resources.new.qrc
    )

set(QV2RAY_FULL_SOURCES
    ${QV2RAY_RC}
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${QV2RAY_UI_SOURCES}
    ${QV2RAY_PLATFORM_SOURCES}
    ${QV2RAY_EMBED_TRANSLATION_QRC}
    ${QV2RAY_QRC_RESOURCES}
    ${QV2RAY_QM_FILES}
    )

QVLOG(QV2RAY_QM_FILES)
QVLOG(QV2RAY_QRC_RESOURCES)

if(QV2RAY_QT6)
    qt6_add_executable(qv2ray ${GUI_TYPE} ${QV2RAY_FULL_SOURCES})
    # For Qt5, Android
    # add_library(qv2ray SHARED ${QV2RAY_FULL_SOURCES})
else()
    add_executable(qv2ray ${GUI_TYPE} ${QV2RAY_FULL_SOURCES})
endif()

# set_target_properties(MAIN PROPERTIES OUTPUT_NAME "qv2ray")

if(ANDROID)
    qvlog(ANDROID_EXTRA_LIBS)
    set_property(TARGET qv2ray PROPERTY QT_ANDROID_EXTRA_LIBS ${ANDROID_EXTRA_LIBS})
    set_property(TARGET qv2ray PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/assets/android)
endif()

target_include_directories(qv2ray PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
    ${QNODEEDITOR_INCLUDE_PATH}
    ${Protobuf_INCLUDE_DIRS}
    )

target_link_libraries(qv2ray PUBLIC
    qv2ray_baselib
    ${QNODEEDITOR_LIBRARY}
    ${QV2RAY_PLATFORM_LIBS}
    ${QV2RAY_QRENCODE_LIBRARY}
    )


# ==================================================================================
# Qv2ray Unit Tests
# ==================================================================================
if (BUILD_TESTING)
    include(CTest)
    add_subdirectory(test)
endif()

# ==================================================================================
# Platform-dependent installation process and deployment
# ==================================================================================
if(APPLE)
    include(cmake/platforms/macos.cmake)
elseif(UNIX AND NOT APPLE AND NOT WIN32 AND NOT ANDROID)
    include(cmake/platforms/linux.cmake)
elseif(WIN32)
    include(cmake/platforms/windows.cmake)
endif()


# ==================================================================================
# Print Qv2ray Build Statistics
# ==================================================================================
get_target_property(QV2RAY_BASE_LINK_LIBS qv2ray_baselib LINK_LIBRARIES)
get_target_property(QV2RAY_EXEC_LINK_LIBS qv2ray LINK_LIBRARIES)
QVLOG(QV2RAY_BUILD_VERSION)
QVLOG(QV2RAY_VERSION_STRING)
QVLOG(CMAKE_INSTALL_PREFIX)
QVLOG(CMAKE_BINARY_DIR)
QVLOG(QV2RAY_BASE_LINK_LIBS)
QVLOG(QV2RAY_EXEC_LINK_LIBS)
message("")
message("======== Qv2ray Build Statistics ========")
message("${QV2RAY_BUILD_STATS}")
message("=========================================")
message("")
