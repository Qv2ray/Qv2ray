# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011-2013, RedJack, LLC.
# All rights reserved.
#
# Please see the LICENSE.txt file in this distribution for license
# details.
# ----------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME ipset)
set(RELEASE_DATE 2013-12-11)
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(CTargets)

#-----------------------------------------------------------------------
# Retrieve the current version number

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
# Check for prerequisite libraries

include_directories(${libcork_include_dirs})

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

#-----------------------------------------------------------------------
# Include our subdirectories

#add_subdirectory(src)

add_c_library(
        libipset
        OUTPUT_NAME ipset
        PKGCONFIG_NAME libipset
        VERSION 1.1.0
        SOURCES
        general.c
        bdd/assignments.c
        bdd/basics.c
        bdd/bdd-iterator.c
        bdd/expanded.c
        bdd/reachable.c
        bdd/read.c
        bdd/write.c
        map/allocation.c
        map/inspection.c
        map/ipv4_map.c
        map/ipv6_map.c
        map/storage.c
        set/allocation.c
        set/inspection.c
        set/ipv4_set.c
        set/ipv6_set.c
        set/iterator.c
        set/storage.c

        LOCAL_LIBRARIES
        libcork
)

set(libipset_include_dirs
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/include
        CACHE INTERNAL "libipset library" FORCE
        )
