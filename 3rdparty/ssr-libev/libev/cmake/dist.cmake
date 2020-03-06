# LuaDist CMake utility library.
# Provides sane project defaults and macros common to LuaDist CMake builds.
# 
# Copyright (C) 2007-2012 LuaDist.
# by David Manura, Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.


# Tweaks and other defaults
# Setting CMAKE to use loose block and search for find modules in source directory
set ( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
set ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

# In MSVC, prevent warnings that can occur when using standard libraries.
if ( MSVC )
  add_definitions ( -D_CRT_SECURE_NO_WARNINGS )
endif ()

