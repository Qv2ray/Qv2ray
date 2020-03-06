# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011-2015, RedJack, LLC.
# All rights reserved.
#
# Please see the COPYING file in this distribution for license details.
# ----------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME libcork)
set(RELEASE_DATE 2015-09-03)
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(CTargets)

#-----------------------------------------------------------------------
# Retrieve the current version number

set(VERSION_MAJOR "0")
set(VERSION_MINOR "15")
set(VERSION_PATCH "0")
set(VERSION "0.15.0")
set(GIT_SHA1 "d6ecc2cfbcdf5013038a72b4544f7d9e6eb8f92d")

#-----------------------------------------------------------------------
# Check for building on Tilera
# If the Tilera environment is installed, then $TILERA_ROOT is defined
# as the path to the active MDE.

if(DEFINED ENV{TILERA_ROOT})
    set(TILERA TRUE)
    set(TILERA_ROOT $ENV{TILERA_ROOT})
    message("-- Configuring for Tilera MDE ${TILERA_ROOT}")
    set(ENV{PKG_CONFIG_PATH}
        "${TILERA_ROOT}/tile/usr/lib/pkgconfig:${TILERA_ROOT}/tile/usr/local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}"
    )
    set(CMAKE_SYSTEM_NAME "Tilera")
    set(CMAKE_SYSTEM_PROCESSOR "tilegx")
    set(CMAKE_C_COMPILER "${TILERA_ROOT}/bin/tile-gcc")
    set(CMAKE_LINKER "${TILERA_ROOT}/bin/tile-ld")
    set(TILERA_MONITOR "${TILERA_ROOT}/bin/tile-monitor")
    #add_definitions(-std=gnu99)
    set(CMAKE_FIND_ROOT_PATH "${TILERA_ROOT}/tile")
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

#-----------------------------------------------------------------------
# Set some options

if(APPLE)
    if (NOT CMAKE_INSTALL_NAME_DIR)
        set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
    endif (NOT CMAKE_INSTALL_NAME_DIR)
endif(APPLE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_definitions(-Wall -Werror)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -Werror)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Intel")
    add_definitions(-Wall -Werror)
endif(CMAKE_C_COMPILER_ID STREQUAL "GNU")

include(GNUInstallDirs)

#-----------------------------------------------------------------------
# Check for prerequisite libraries

find_package(Threads)
set(THREADS_LDFLAGS "${CMAKE_THREAD_LIBS_INIT}")
if(WIN32)
    if(CMAKE_THREAD_LIBS_INIT)
        set(THREADS_STATIC_LDFLAGS "-static ${CMAKE_THREAD_LIBS_INIT}")
    else()
        set(THREADS_STATIC_LDFLAGS "-static")
    endif()
else()
    set(THREADS_STATIC_LDFLAGS "${CMAKE_THREAD_LIBS_INIT}")
endif()



#-----------------------------------------------------------------------
# Include our subdirectories

#add_subdirectory(include)
#add_subdirectory(src)

add_c_library(
        libcork
        OUTPUT_NAME cork
        PKGCONFIG_NAME libcork
        VERSION 16.0.1
        SOURCES
        cli/commands.c
        core/allocator.c
        core/error.c
        core/gc.c
        core/hash.c
        core/ip-address.c
        core/mempool.c
        core/timestamp.c
        core/u128.c
        core/version.c
        ds/array.c
        ds/bitset.c
        ds/buffer.c
        ds/dllist.c
        ds/file-stream.c
        ds/hash-table.c
        ds/managed-buffer.c
        ds/ring-buffer.c
        ds/slice.c
#        posix/directory-walker.c
#        posix/env.c
#        posix/exec.c
#        posix/files.c
        posix/process.c
#        posix/subprocess.c
        pthreads/thread.c
        LIBRARIES
        threads
)

set_target_properties(libcork PROPERTIES
        COMPILE_DEFINITIONS CORK_API=CORK_LOCAL
        )

set(libcork_include_dirs
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/include
        CACHE INTERNAL "libcork library" FORCE
        )