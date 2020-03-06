# Copyright (C) 2007-2013 LuaDist.
# Created by Peter Drahoš, Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( libev C )
cmake_minimum_required ( VERSION 2.8 )
include ( cmake/dist.cmake )
#include ( configure )

#configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )
include_directories( ${CMAKE_BINARY_DIR} )

set ( EV_SRC 
  ev.c
  event.c
)

if (CYGWIN)
  list ( APPEND EV_LIBS Ws2_32 )
endif ()

add_library ( ev STATIC ${EV_SRC} )
target_link_libraries ( ev ${EV_LIBS} )

set(libev_include_dirs
        ${PROJECT_SOURCE_DIR}
        CACHE INTERNAL "libev library" FORCE
        )
