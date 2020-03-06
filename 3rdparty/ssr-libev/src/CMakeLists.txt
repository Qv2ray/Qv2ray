set(SOURCE_FILES_SNI
        http.c
        tls.c
        rule.c)

set(SOURCE_FILES_LOCAL
        utils.c
        jconf.c
        json.c
        encrypt.c
        udprelay.c
        cache.c
        acl.c
        netutils.c
        local.c
        ${SOURCE_FILES_SNI})


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${libev_include_dirs})
include_directories(${libcork_include_dirs})
include_directories(${libipset_include_dirs})
include_directories(${libudns_include_dirs})
#include_directories(${libsodium_include_dirs})


if (WIN32)
    list ( APPEND SOURCE_FILES_LOCAL win32.c )
    list ( APPEND SOURCE_FILES_TUNNEL win32.c )
    list ( APPEND SOURCE_FILES_SERVER win32.c )
endif ()

add_executable(ss_local ${SOURCE_FILES_LOCAL})
add_library(libshadowsocks-libev ${SOURCE_FILES_LOCAL})

set_target_properties(ss_local PROPERTIES
        COMPILE_DEFINITIONS MODULE_LOCAL
        )
target_compile_definitions(libshadowsocks-libev
        PRIVATE MODULE_LOCAL LIB_ONLY
        )
set_target_properties(libshadowsocks-libev PROPERTIES SUFFIX ".la")

set (ss_lib_common
        ev
        m
        libcork)

set (ss_lib_net
        ${ss_lib_common}
        ${LIBCRYPTO}
        libipset
        ${PCRE_LIBRARIES}
        udns
        sodium
        )

if (WIN32)
    list ( APPEND ss_lib_net Ws2_32 )
endif()

target_link_libraries(ss_local
        ${ss_lib_net}
)
