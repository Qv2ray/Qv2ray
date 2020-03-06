# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2015, RedJack, LLC.
# All rights reserved.
#
# Please see the COPYING file in this distribution for license details.
# ----------------------------------------------------------------------


#-----------------------------------------------------------------------
# Library, with options to build both shared and static versions

function(target_add_static_libraries TARGET_NAME LIBRARIES LOCAL_LIBRARIES)
    foreach(lib ${LIBRARIES})
        string(REPLACE "-" "_" lib ${lib})
        string(TOUPPER ${lib} upperlib)
        target_link_libraries(
            ${TARGET_NAME}
            ${${upperlib}_STATIC_LDFLAGS}
        )
    endforeach(lib)
    foreach(lib ${LOCAL_LIBRARIES})
        target_link_libraries(${TARGET_NAME} ${lib})
    endforeach(lib)
endfunction(target_add_static_libraries)

set_property(GLOBAL PROPERTY ALL_LOCAL_LIBRARIES "")

function(add_c_library __TARGET_NAME)
    set(options)
    set(one_args OUTPUT_NAME PKGCONFIG_NAME VERSION)
    set(multi_args LIBRARIES LOCAL_LIBRARIES SOURCES)
    cmake_parse_arguments(_ "${options}" "${one_args}" "${multi_args}" ${ARGN})

    if (__VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-dev)?$")
        set(__VERSION_CURRENT  "${CMAKE_MATCH_1}")
        set(__VERSION_REVISION "${CMAKE_MATCH_2}")
        set(__VERSION_AGE      "${CMAKE_MATCH_3}")
    else (__VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-dev)?$")
        message(FATAL_ERROR "Invalid library version number: ${__VERSION}")
    endif (__VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-dev)?$")

    math(EXPR __SOVERSION "${__VERSION_CURRENT} - ${__VERSION_AGE}")

    get_property(ALL_LOCAL_LIBRARIES GLOBAL PROPERTY ALL_LOCAL_LIBRARIES)
    list(APPEND ALL_LOCAL_LIBRARIES ${__TARGET_NAME})
    set_property(GLOBAL PROPERTY ALL_LOCAL_LIBRARIES "${ALL_LOCAL_LIBRARIES}")

    include_directories(
            ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_BINARY_DIR}/include
    )
    add_library(${__TARGET_NAME} STATIC ${__SOURCES})

    set_target_properties(
        ${__TARGET_NAME} PROPERTIES
        OUTPUT_NAME ${__OUTPUT_NAME}
        CLEAN_DIRECT_OUTPUT 1
    )

    target_include_directories(
        ${__TARGET_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
    )

    target_add_static_libraries(
        ${__TARGET_NAME}
        "${__LIBRARIES}"
        "${__LOCAL_LIBRARIES}"
    )
endfunction(add_c_library)
