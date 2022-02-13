cmake_minimum_required(VERSION 3.10.1)
project(Qv2ray VERSION 3.0.0)
set(QV2RAY_PROJECT_VERSION_SUFFIX "rc1")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(BUILD_TESTING "Build Testing" OFF)

if(NOT DEFINED BUILD_SHARED_LIBS)
    option(BUILD_SHARED_LIBS "Build Shared Libraries" ON)
endif()
if(NOT DEFINED QV2RAY_STATIC_PLUGINS)
    option(QV2RAY_STATIC_PLUGINS "Build static plugins" OFF)
endif()

message("Build shared library: " ${BUILD_SHARED_LIBS})

if(WIN32)
    include(cmake/versioninfo/generate_product_version.cmake)
    generate_product_version(
        QV2RAY_RC
        NAME               "Qv2ray"
        BUNDLE             "Qv2ray Project Family"
        ICON               "${CMAKE_CURRENT_LIST_DIR}/assets/icons/qv2ray.ico"
        VERSION_MAJOR      ${CMAKE_PROJECT_VERSION_MAJOR}
        VERSION_MINOR      ${CMAKE_PROJECT_VERSION_MINOR}
        VERSION_PATCH      ${CMAKE_PROJECT_VERSION_PATCH}
        VERSION_REVISION   "0"
        COMPANY_NAME       "Moody"
        COMPANY_COPYRIGHT  "Qv2ray Workgroup 2021, Shadowsocks-NET 2021, Moody 2022"
        FILE_DESCRIPTION   "Qv2ray 3.0"
        )
    add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)
    set(GUI_TYPE WIN32)
    if(MSVC)
        add_compile_options("/utf-8")
        add_compile_options("/wo4251")
        add_compile_options("/wo4244")
        add_compile_options("/wo4245")
        add_compile_options("/wo4267")
        add_compile_options("/wo4996")
        add_definitions(-D_WIN32_WINNT=0x600 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
    endif()
endif()

cmake_policy(SET CMP0100 NEW)
cmake_policy(SET CMP0087 NEW)

# ==================================================================================
# Qv2ray compile arguments
# ==================================================================================

set(QV2RAY_DEFAULT_VASSETS_PATH "unset" CACHE STRING "Default V2Ray assets path")
set(QV2RAY_DEFAULT_VCORE_PATH   "unset" CACHE STRING "Default V2Ray core path")

set(QV2RAY_QV2RAYBASE_PROVIDER        "module" CACHE STRING "Qv2rayBase Provider")
set(QV2RAY_SINGLEAPPLICATION_PROVIDER "module" CACHE STRING "SingleApplication Provider")

option(QV2RAY_HAS_BUILTIN_THEMES    "Build with builtin themes" ON)
option(QV2RAY_AUTO_UPDATE           "Has auto update checker"   ON)
if(NOT QV2RAY_AUTO_UPDATE)
    add_definitions(-DQV2RAY_NO_AUTOUPDATE=1)
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
# Qv2rayBase, Qt Libraries, Qv2ray GUI Libraries, Threads
# ==================================================================================
cmake_policy(SET CMP0071 NEW)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(QV2RAY_CHECK_DEPRECATED OFF)

if (QV2RAY_CHECK_DEPRECATED)
    add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x070000)
endif()

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(Qt6 6.2 COMPONENTS Core Widgets Gui Network REQUIRED)

set(QAPPLICATION_CLASS "QApplication")
set(QT_DEFAULT_MAJOR_VERSION 6)

if(QV2RAY_QV2RAYBASE_PROVIDER STREQUAL "package")
    find_package(Qv2rayBase REQUIRED)
else()
    add_subdirectory(src/Qv2rayBase)
endif()

if(QV2RAY_SINGLEAPPLICATION_PROVIDER STREQUAL "package")
    find_library(SINGLEAPPLICATION_LIBRARY NAMES SingleApplication)
    find_path(SINGLEAPPLICATION_DIR NAMES singleapplication.h PATH_SUFFIXES singleapplication)
else()
    add_subdirectory(3rdparty/SingleApplication)
endif()

# ==================================================================================
# Qv2ray Builtin Plugins
# ==================================================================================
add_subdirectory(src/plugins)

# ==================================================================================
# Qv2ray
# ==================================================================================
include(cmake/qrencode.cmake)
include(cmake/qv2ray-sources.cmake)

if(QV2RAY_HAS_BUILTIN_THEMES)
    include(3rdparty/uistyles/uistyles.cmake)
endif()

# ==================================================================================
# Qv2ray Build Info
# ==================================================================================
set(QV2RAY_CODENAME "Qv2ray 3.0, the new era.")

if(QV2RAY_BUILD_INFO)
    set(QV2RAY_BUILD_INFO "${QV2RAY_BUILD_INFO}")
elseif(DEFINED ENV{QV2RAY_BUILD_INFO})
    set(QV2RAY_BUILD_INFO "$ENV{QV2RAY_BUILD_INFO}")
else()
    set(QV2RAY_BUILD_INFO "Qv2ray (Made By Yourself)")
endif()

if(QV2RAY_BUILD_EXTRA_INFO)
    set(QV2RAY_BUILD_EXTRA_INFO "${QV2RAY_BUILD_EXTRA_INFO}")
elseif(DEFINED ENV{QV2RAY_BUILD_EXTRA_INFO})
    set(QV2RAY_BUILD_EXTRA_INFO "$ENV{QV2RAY_BUILD_EXTRA_INFO}")
else()
    set(QV2RAY_BUILD_EXTRA_INFO "${CMAKE_PROJECT_VERSION}")
endif()

add_definitions(-DQV2RAY_BUILD_INFO="${QV2RAY_BUILD_INFO}")
add_definitions(-DQV2RAY_BUILD_EXTRA_INFO="${QV2RAY_BUILD_EXTRA_INFO} - ${QV2RAY_CODENAME}")
message("Qv2ray: ${QV2RAY_BUILD_INFO} - ${QV2RAY_BUILD_EXTRA_INFO}")

message(" ")
message("Qv2ray Version: ${CMAKE_PROJECT_VERSION}")
message("Qv2ray Build Version: ${QV2RAY_BUILD_VERSION}")
message("|-------------------------------------------------|")
message("| Qv2ray, A Cross Platform v2ray Qt GUI Client.   |")
message("| Licenced under GPLv3.                           |")
message("|                                                 |")
message("| You may only use this program to the extent     |")
message("| permitted by local law.                         |")
message("|                                                 |")
message("| See: https://www.gnu.org/licenses/gpl-3.0.html  |")
message("|-------------------------------------------------|")
message("| This is the original Qv2ray project by the      |")
message("| author who contributed over 95% code and other  |")
message("| contributors.                                   |")
message("|-------------------------------------------------|")
message("| Unlike Qv2ray/Qv2ray whose control has been     |")
message("| forcibly taken away from the author by another  |")
message("| organization owner.                             |")
message("|-------------------------------------------------|")
message(" ")

qt6_add_executable(qv2ray
    ${GUI_TYPE} ${QV2RAY_RC}
    ${QV2RAY_FULL_SOURCES}
    ${UISTYLE_QRCS}
    ${CMAKE_CURRENT_LIST_DIR}/assets/icons/resources.qrc)

qv2ray_add_plugin_moc_sources(qv2ray)

target_compile_definitions(qv2ray PRIVATE -DQV2RAY_VERSION_STRING="${CMAKE_PROJECT_VERSION}-${QV2RAY_PROJECT_VERSION_SUFFIX}")
target_include_directories(qv2ray PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(qv2ray PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/components)

target_link_libraries(qv2ray PRIVATE
    Qv2ray::QREncode
    Qv2ray::Qv2rayBase
    Qv2ray::QvPluginInterface
    Qv2ray::AllPlugins
    OpenSSL::SSL
    OpenSSL::Crypto
    SingleApplication::SingleApplication
    )

# BEGIN SPECIAL CASE
if(USE_SYSTEM_LIBUV)
# Qt deferred target finalizer will try looking for "unofficial::libuv::libuv" when USE_SYSTEM_LIBUV is on.
    if(WIN32)
        find_package(unofficial-libuv CONFIG REQUIRED)
        add_library(Qv2ray::libuv ALIAS unofficial::libuv::libuv)
    endif()
endif()
# END SPECIAL CASE

# ==================================================================================
# Platform-specific installation and deployment
# ==================================================================================
if(APPLE)
    include(cmake/deployment-macos.cmake)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include(cmake/deployment-linux.cmake)
elseif(WIN32)
    include(cmake/deployment-windows.cmake)
endif()
