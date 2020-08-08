#
# BackwardMacros.cmake
# Copyright 2013 Google Inc. All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

###############################################################################
# OPTIONS
###############################################################################

set(STACK_WALKING_UNWIND TRUE CACHE BOOL
	"Use compiler's unwind API")
set(STACK_WALKING_BACKTRACE FALSE CACHE BOOL
	"Use backtrace from (e)glibc for stack walking")

set(STACK_DETAILS_AUTO_DETECT TRUE CACHE BOOL
	"Auto detect backward's stack details dependencies")

set(STACK_DETAILS_BACKTRACE_SYMBOL FALSE CACHE BOOL
	"Use backtrace from (e)glibc for symbols resolution")
set(STACK_DETAILS_DW FALSE CACHE BOOL
	"Use libdw to read debug info")
set(STACK_DETAILS_BFD FALSE CACHE BOOL
	"Use libbfd to read debug info")
set(STACK_DETAILS_DWARF FALSE CACHE BOOL
	"Use libdwarf/libelf to read debug info")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND NOT DEFINED BACKWARD_TESTS)
	# If this is a top level CMake project, we most lixely want the tests
	set(BACKWARD_TESTS ON CACHE BOOL "Enable tests")
else()
	set(BACKWARD_TESTS OFF CACHE BOOL "Enable tests")
endif()
###############################################################################
# CONFIGS
###############################################################################
if (${STACK_DETAILS_AUTO_DETECT})
	include(FindPackageHandleStandardArgs)

	# find libdw
	find_path(LIBDW_INCLUDE_DIR NAMES "elfutils/libdw.h" "elfutils/libdwfl.h")
	find_library(LIBDW_LIBRARY dw)
	set(LIBDW_INCLUDE_DIRS ${LIBDW_INCLUDE_DIR} )
	set(LIBDW_LIBRARIES ${LIBDW_LIBRARY} )
	find_package_handle_standard_args(libdw DEFAULT_MSG
		LIBDW_LIBRARY LIBDW_INCLUDE_DIR)
	mark_as_advanced(LIBDW_INCLUDE_DIR LIBDW_LIBRARY)

	# find libbfd
	find_path(LIBBFD_INCLUDE_DIR NAMES "bfd.h")
	find_path(LIBDL_INCLUDE_DIR NAMES "dlfcn.h")
	find_library(LIBBFD_LIBRARY bfd)
	find_library(LIBDL_LIBRARY dl)
	set(LIBBFD_INCLUDE_DIRS ${LIBBFD_INCLUDE_DIR} ${LIBDL_INCLUDE_DIR})
	set(LIBBFD_LIBRARIES ${LIBBFD_LIBRARY} ${LIBDL_LIBRARY})
	find_package_handle_standard_args(libbfd DEFAULT_MSG
		LIBBFD_LIBRARY LIBBFD_INCLUDE_DIR
		LIBDL_LIBRARY LIBDL_INCLUDE_DIR)
	mark_as_advanced(LIBBFD_INCLUDE_DIR LIBBFD_LIBRARY
		LIBDL_INCLUDE_DIR LIBDL_LIBRARY)

	# find libdwarf
	find_path(LIBDWARF_INCLUDE_DIR NAMES "libdwarf.h" PATH_SUFFIXES libdwarf)
	find_path(LIBELF_INCLUDE_DIR NAMES "libelf.h")
	find_path(LIBDL_INCLUDE_DIR NAMES "dlfcn.h")
	find_library(LIBDWARF_LIBRARY dwarf)
	find_library(LIBELF_LIBRARY elf)
	find_library(LIBDL_LIBRARY dl)
	set(LIBDWARF_INCLUDE_DIRS ${LIBDWARF_INCLUDE_DIR} ${LIBELF_INCLUDE_DIR} ${LIBDL_INCLUDE_DIR})
	set(LIBDWARF_LIBRARIES ${LIBDWARF_LIBRARY} ${LIBELF_LIBRARY} ${LIBDL_LIBRARY})
	find_package_handle_standard_args(libdwarf DEFAULT_MSG
		LIBDWARF_LIBRARY LIBDWARF_INCLUDE_DIR
		LIBELF_LIBRARY LIBELF_INCLUDE_DIR
		LIBDL_LIBRARY LIBDL_INCLUDE_DIR)
	mark_as_advanced(LIBDWARF_INCLUDE_DIR LIBDWARF_LIBRARY
		LIBELF_INCLUDE_DIR LIBELF_LIBRARY
		LIBDL_INCLUDE_DIR LIBDL_LIBRARY)

	if (LIBDW_FOUND)
		LIST(APPEND _BACKWARD_INCLUDE_DIRS ${LIBDW_INCLUDE_DIRS})
		LIST(APPEND _BACKWARD_LIBRARIES ${LIBDW_LIBRARIES})
		set(STACK_DETAILS_DW TRUE)
		set(STACK_DETAILS_BFD FALSE)
		set(STACK_DETAILS_DWARF FALSE)
		set(STACK_DETAILS_BACKTRACE_SYMBOL FALSE)
	elseif(LIBBFD_FOUND)
		LIST(APPEND _BACKWARD_INCLUDE_DIRS ${LIBBFD_INCLUDE_DIRS})
		LIST(APPEND _BACKWARD_LIBRARIES ${LIBBFD_LIBRARIES})

		# If we attempt to link against static bfd, make sure to link its dependencies, too
		get_filename_component(bfd_lib_ext "${LIBBFD_LIBRARY}" EXT)
		if (bfd_lib_ext STREQUAL "${CMAKE_STATIC_LIBRARY_SUFFIX}")
			list(APPEND _BACKWARD_LIBRARIES iberty z)
		endif()

		set(STACK_DETAILS_DW FALSE)
		set(STACK_DETAILS_BFD TRUE)
		set(STACK_DETAILS_DWARF FALSE)
		set(STACK_DETAILS_BACKTRACE_SYMBOL FALSE)
	elseif(LIBDWARF_FOUND)
		LIST(APPEND _BACKWARD_INCLUDE_DIRS ${LIBDWARF_INCLUDE_DIRS})
		LIST(APPEND _BACKWARD_LIBRARIES ${LIBDWARF_LIBRARIES})

		set(STACK_DETAILS_DW FALSE)
		set(STACK_DETAILS_BFD FALSE)
		set(STACK_DETAILS_DWARF TRUE)
		set(STACK_DETAILS_BACKTRACE_SYMBOL FALSE)
	else()
		set(STACK_DETAILS_DW FALSE)
		set(STACK_DETAILS_BFD FALSE)
		set(STACK_DETAILS_DWARF FALSE)
		set(STACK_DETAILS_BACKTRACE_SYMBOL TRUE)
	endif()
else()
	if (STACK_DETAILS_DW)
		LIST(APPEND _BACKWARD_LIBRARIES dw)
	endif()

	if (STACK_DETAILS_BFD)
		LIST(APPEND _BACKWARD_LIBRARIES bfd dl)
	endif()

	if (STACK_DETAILS_DWARF)
		LIST(APPEND _BACKWARD_LIBRARIES dwarf elf)
	endif()
endif()

macro(map_definitions var_prefix define_prefix)
	foreach(def ${ARGN})
		if (${${var_prefix}${def}})
			LIST(APPEND _BACKWARD_DEFINITIONS "${define_prefix}${def}=1")
		else()
			LIST(APPEND _BACKWARD_DEFINITIONS "${define_prefix}${def}=0")
		endif()
	endforeach()
endmacro()

if (NOT _BACKWARD_DEFINITIONS)
	map_definitions("STACK_WALKING_" "BACKWARD_HAS_" UNWIND BACKTRACE)
	map_definitions("STACK_DETAILS_" "BACKWARD_HAS_" BACKTRACE_SYMBOL DW BFD DWARF)
endif()

set(BACKWARD_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}")

set(BACKWARD_HAS_EXTERNAL_LIBRARIES FALSE)
set(FIND_PACKAGE_REQUIRED_VARS BACKWARD_INCLUDE_DIR)
if(DEFINED _BACKWARD_LIBRARIES)
	set(BACKWARD_HAS_EXTERNAL_LIBRARIES TRUE)
	list(APPEND FIND_PACKAGE_REQUIRED_VARS _BACKWARD_LIBRARIES)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Backward
    REQUIRED_VARS ${FIND_PACKAGE_REQUIRED_VARS}
)
list(APPEND _BACKWARD_INCLUDE_DIRS ${BACKWARD_INCLUDE_DIR})

macro(add_backward target)
	target_include_directories(${target} PRIVATE ${BACKWARD_INCLUDE_DIRS})
	set_property(TARGET ${target} APPEND PROPERTY COMPILE_DEFINITIONS ${BACKWARD_DEFINITIONS})
	set_property(TARGET ${target} APPEND PROPERTY LINK_LIBRARIES ${BACKWARD_LIBRARIES})
endmacro()

set(BACKWARD_INCLUDE_DIRS ${_BACKWARD_INCLUDE_DIRS} CACHE INTERNAL "_BACKWARD_INCLUDE_DIRS")
set(BACKWARD_DEFINITIONS ${_BACKWARD_DEFINITIONS} CACHE INTERNAL "BACKWARD_DEFINITIONS")
set(BACKWARD_LIBRARIES ${_BACKWARD_LIBRARIES} CACHE INTERNAL "BACKWARD_LIBRARIES")
mark_as_advanced(BACKWARD_INCLUDE_DIRS BACKWARD_DEFINITIONS BACKWARD_LIBRARIES)

# Expand each definition in BACKWARD_DEFINITIONS to its own cmake var and export
# to outer scope
foreach(var ${BACKWARD_DEFINITIONS})
  string(REPLACE "=" ";" var_as_list ${var})
  list(GET var_as_list 0 var_name)
  list(GET var_as_list 1 var_value)
  set(${var_name} ${var_value})
  mark_as_advanced(${var_name})
endforeach()

if (NOT TARGET Backward::Backward)
	add_library(Backward::Backward INTERFACE IMPORTED)
	set_target_properties(Backward::Backward PROPERTIES
	    INTERFACE_INCLUDE_DIRECTORIES "${BACKWARD_INCLUDE_DIRS}"
	    INTERFACE_COMPILE_DEFINITIONS "${BACKWARD_DEFINITIONS}"
	)
	if(BACKWARD_HAS_EXTERNAL_LIBRARIES)
		set_target_properties(Backward::Backward PROPERTIES
			INTERFACE_LINK_LIBRARIES "${BACKWARD_LIBRARIES}" 
		)
	endif()
endif()
