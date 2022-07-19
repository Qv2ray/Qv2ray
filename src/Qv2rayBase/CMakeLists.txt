cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.14)

project(Qv2rayBase
    VERSION 3.0.0.0
    LANGUAGES CXX
    DESCRIPTION "Base library for Qv2ray")

if (NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/Windows.cmake)
endif()

include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

option(USE_SYSTEM_PLUGIN_INTERFACE "Use system provided plugin interface headers." OFF)

if(USE_SYSTEM_PLUGIN_INTERFACE)
    find_package(QvPluginInterface 5.0 REQUIRED)
else()
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/PluginInterface)
    install(TARGETS QvPluginInterface)
endif()

find_package(Qt6 6.2 COMPONENTS Core Network REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(WASM ON)
else()
    set(WASM OFF)
endif()

if(WASM)
    # Dummy
else()
    include(cmake/libuvw.cmake)
endif()

set(BASELIB_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/Common/HTTPRequestHelper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Common/ProfileHelpers.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Plugin/LatencyTestHost.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Plugin/PluginAPIHost.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Plugin/PluginManagerCore.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Profile/KernelManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Profile/ProfileManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Qv2rayBaseLibrary.cpp
    )

set(BASELIB_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Common/HTTPRequestHelper.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Common/ProfileHelpers.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Common/Settings.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Common/Utils.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Interfaces/IStorageProvider.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Interfaces/IUserInteractionInterface.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Plugin/LatencyTestHost.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Plugin/PluginAPIHost.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Plugin/PluginManagerCore.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Profile/KernelManager.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Profile/ProfileManager.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Qv2rayBaseFeatures.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/Qv2rayBaseLibrary.hpp
    )

set(BASELIB_P_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/private/Common/SettingsUpgrade_p.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/Interfaces/BaseStorageProvider_p.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/Plugin/LatencyTestHost_p.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/Plugin/LatencyTestThread_p.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/Plugin/PluginAPIHost_p.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/Plugin/PluginManagerCore_p.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/Profile/KernelManager_p.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/Profile/ProfileManager_p.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/private/Qv2rayBaseLibrary_p.cpp
    )

set(BASELIB_P_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/private/Common/SettingsUpgrade_p.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/private/Interfaces/BaseStorageProvider_p.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/private/Plugin/LatencyTestHost_p.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/private/Plugin/LatencyTestThread_p.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/private/Plugin/PluginAPIHost_p.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/private/Plugin/PluginManagerCore_p.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/private/Profile/KernelManager_p.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/private/Profile/ProfileManager_p.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Qv2rayBase/private/Qv2rayBaseLibrary_p.hpp
    )

option(BUILD_SHARED_LIBS "Build Qv2rayBase as shared library.")

add_library(Qv2rayBase ${QV2RAYBASE_RC} ${BASELIB_SOURCES} ${BASELIB_P_SOURCES} ${BASELIB_HEADERS} ${BASELIB_P_HEADERS})
target_include_directories(Qv2rayBase
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

set_target_properties(Qv2rayBase PROPERTIES CXX_STANDARD 17)

target_include_directories(Qv2rayBase PRIVATE ${QVPLUGIN_INTERFACE_INCLUDE_PATH})

target_compile_definitions(Qv2rayBase PUBLIC -DQV2RAY_BASELIB_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
target_compile_definitions(Qv2rayBase PUBLIC -DQV2RAY_BASELIB_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_compile_definitions(Qv2rayBase PUBLIC -DQV2RAY_BASELIB_VERSION_PATCH=${PROJECT_VERSION_PATCH})
target_compile_definitions(Qv2rayBase PUBLIC -DQV2RAY_BASELIB_VERSION_TWEAK=${PROJECT_VERSION_TWEAK})
target_compile_definitions(Qv2rayBase PUBLIC -DQV2RAY_BASELIB_VERSION="${PROJECT_VERSION}")

set_target_properties(Qv2rayBase PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 0)
set_target_properties(Qv2rayBase PROPERTIES CXX_VISIBILITY_PRESET hidden)

if(CMAKE_CXX_COMPILER_ID EQUAL Clang OR CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    if(UNIX AND NOT APPLE)
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,-z,defs")
    endif()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/Deployment.cmake)

add_library(Qv2ray::Qv2rayBase ALIAS Qv2rayBase)

target_link_libraries(Qv2rayBase
    PUBLIC
        Qt::Core
        Qt::Network
        Qv2ray::QvPluginInterface
    )

if(WASM)
    target_compile_definitions(Qv2rayBase PUBLIC "-DQV2RAYBASE_NO_LIBUV")
else()
    target_link_libraries(Qv2rayBase PRIVATE Qv2ray::libuvw)
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
