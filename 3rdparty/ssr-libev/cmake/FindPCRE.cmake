#.rst:
# FindPCRE
# --------
#
# Find the native PCRE includes and library.
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines :prop_tgt:`IMPORTED` target ``PCRE::PCRE``, if
# PCRE has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   PCRE_INCLUDE_DIRS   - where to find pcre.h, etc.
#   PCRE_LIBRARIES      - List of libraries when using pcre.
#   PCRE_FOUND          - True if pcre found.
#
# ::
#
#   PCRE_VERSION_STRING - The version of pcre found (x.y.z)
#   PCRE_VERSION_MAJOR  - The major version of zlib
#   PCRE_VERSION_MINOR  - The minor version of zlib
#   PCRE_VERSION_PATCH  - The patch version of zlib
#   PCRE_VERSION_TWEAK  - The tweak version of zlib
#
# Backward Compatibility
# ^^^^^^^^^^^^^^^^^^^^^^
#
# The following variable are provided for backward compatibility
#
# ::
#
#   PCRE_MAJOR_VERSION  - The major version of zlib
#   PCRE_MINOR_VERSION  - The minor version of zlib
#   PCRE_PATCH_VERSION  - The patch version of zlib
#
# Hints
# ^^^^^
#
# A user may set ``PCRE_ROOT`` to a zlib installation root to tell this
# module where to look.

#=============================================================================
# Copyright 2001-2011 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

set(_PCRE_SEARCHES)

# Search PCRE_ROOT first if it is set.
if(PCRE_ROOT)
    set(_PCRE_SEARCH_ROOT PATHS ${PCRE_ROOT} NO_DEFAULT_PATH)
    list(APPEND _PCRE_SEARCHES _PCRE_SEARCH_ROOT)
endif()

# Normal search.
set(_PCRE_SEARCH_NORMAL
        PATHS "[HKEY_LOCAL_MACHINE\\SOFTWARE\\GnuWin32\\Pcre;InstallPath]"
        "$ENV{PROGRAMFILES}/pcre"
        )
list(APPEND _PCRE_SEARCHES _PCRE_SEARCH_NORMAL)

set(PCRE_NAMES pcre pcredll)
set(PCRE_NAMES_DEBUG pcred)

# Try each search configuration.
foreach(search ${_PCRE_SEARCHES})
    find_path(PCRE_INCLUDE_DIR NAMES pcre.h ${${search}} PATH_SUFFIXES include)
endforeach()

# Allow PCRE_LIBRARY to be set manually, as the location of the pcre library
if(NOT PCRE_LIBRARY)
    foreach(search ${_PCRE_SEARCHES})
        find_library(PCRE_LIBRARY_RELEASE NAMES ${PCRE_NAMES} ${${search}} PATH_SUFFIXES lib)
        find_library(PCRE_LIBRARY_DEBUG NAMES ${PCRE_NAMES_DEBUG} ${${search}} PATH_SUFFIXES lib)
    endforeach()

    include(SelectLibraryConfigurations)
    select_library_configurations(PCRE)
endif()

unset(PCRE_NAMES)
unset(PCRE_NAMES_DEBUG)

mark_as_advanced(PCRE_LIBRARY PCRE_INCLUDE_DIR)

if(PCRE_INCLUDE_DIR AND EXISTS "${PCRE_INCLUDE_DIR}/pcre.h")
    file(STRINGS "${PCRE_INCLUDE_DIR}/pcre.h" PCRE_H REGEX "^#define PCRE_VERSION \"[^\"]*\"$")

    string(REGEX REPLACE "^.*PCRE_VERSION \"([0-9]+).*$" "\\1" PCRE_VERSION_MAJOR "${PCRE_H}")
    string(REGEX REPLACE "^.*PCRE_VERSION \"[0-9]+\\.([0-9]+).*$" "\\1" PCRE_VERSION_MINOR  "${PCRE_H}")
    string(REGEX REPLACE "^.*PCRE_VERSION \"[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" PCRE_VERSION_PATCH "${PCRE_H}")
    set(PCRE_VERSION_STRING "${PCRE_VERSION_MAJOR}.${PCRE_VERSION_MINOR}.${PCRE_VERSION_PATCH}")

    # only append a TWEAK version if it exists:
    set(PCRE_VERSION_TWEAK "")
    if( "${PCRE_H}" MATCHES "PCRE_VERSION \"[0-9]+\\.[0-9]+\\.[0-9]+\\.([0-9]+)")
        set(PCRE_VERSION_TWEAK "${CMAKE_MATCH_1}")
        set(PCRE_VERSION_STRING "${PCRE_VERSION_STRING}.${PCRE_VERSION_TWEAK}")
    endif()

    set(PCRE_MAJOR_VERSION "${PCRE_VERSION_MAJOR}")
    set(PCRE_MINOR_VERSION "${PCRE_VERSION_MINOR}")
    set(PCRE_PATCH_VERSION "${PCRE_VERSION_PATCH}")
endif()

# handle the QUIETLY and REQUIRED arguments and set PCRE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PCRE REQUIRED_VARS PCRE_LIBRARY PCRE_INCLUDE_DIR
        VERSION_VAR PCRE_VERSION_STRING)

if(PCRE_FOUND)
    set(PCRE_INCLUDE_DIRS ${PCRE_INCLUDE_DIR})

    if(NOT PCRE_LIBRARIES)
        set(PCRE_LIBRARIES ${PCRE_LIBRARY})
    endif()

    if(NOT TARGET PCRE::PCRE)
        add_library(PCRE::PCRE UNKNOWN IMPORTED)
        set_target_properties(PCRE::PCRE PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${PCRE_INCLUDE_DIRS}")

        if(PCRE_LIBRARY_RELEASE)
            set_property(TARGET PCRE::PCRE APPEND PROPERTY
                    IMPORTED_CONFIGURATIONS RELEASE)
            set_target_properties(PCRE::PCRE PROPERTIES
                    IMPORTED_LOCATION_RELEASE "${PCRE_LIBRARY_RELEASE}")
        endif()

        if(PCRE_LIBRARY_DEBUG)
            set_property(TARGET PCRE::PCRE APPEND PROPERTY
                    IMPORTED_CONFIGURATIONS DEBUG)
            set_target_properties(PCRE::PCRE PROPERTIES
                    IMPORTED_LOCATION_DEBUG "${PCRE_LIBRARY_DEBUG}")
        endif()

        if(NOT PCRE_LIBRARY_RELEASE AND NOT PCRE_LIBRARY_DEBUG)
            set_property(TARGET PCRE::PCRE APPEND PROPERTY
                    IMPORTED_LOCATION "${PCRE_LIBRARY}")
        endif()
    endif()
endif()
