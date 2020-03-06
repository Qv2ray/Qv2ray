# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011-2015, RedJack, LLC.
# All rights reserved.
#
# Please see the COPYING file in this distribution for license details.
# ----------------------------------------------------------------------

set(CMAKE_LEGACY_CYGWIN_WIN32 0)

cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME shadowsocks-libev)
set(RELEASE_DATE 2015-09-03)
project(${PROJECT_NAME})

set(VERSION 2.5.6)


set(with_crypto_library "openssl" CACHE STRING "build with the given crypto library, TYPE=openssl|polarssl|mbedtls default=openssl")



include ( cmake/dist.cmake )
include ( configure )
MESSAGE("fffffffffffffffffff"
        ${LIBCRYPTO})
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

#add_subdirectory(libsodium)
add_subdirectory(libcork)
add_subdirectory(libipset)
add_subdirectory(libev)


include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory(libudns)
add_subdirectory(src)
