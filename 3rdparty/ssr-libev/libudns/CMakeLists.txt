# Copyright (C) 2007-2013 LuaDist.
# Created by Peter Drahoš, Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( libudns )
cmake_minimum_required ( VERSION 2.8 )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set ( UDNS_SRC
  udns_dn.c udns_dntosp.c udns_parse.c udns_resolver.c udns_init.c
  udns_misc.c udns_XtoX.c
  udns_rr_a.c udns_rr_ptr.c udns_rr_mx.c udns_rr_txt.c udns_bl.c
  udns_rr_srv.c udns_rr_naptr.c udns_codes.c udns_jran.c
)

if (WIN32)
    list ( APPEND UDNS_LIBS Ws2_32 )
endif ()

add_library ( udns STATIC ${UDNS_SRC} )
target_link_libraries ( udns ${UDNS_LIBS} )

set(libudns_include_dirs
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
        CACHE INTERNAL "libudns library" FORCE
        )
