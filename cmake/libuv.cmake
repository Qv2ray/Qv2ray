add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libuv EXCLUDE_FROM_ALL)
set_target_properties(uv PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_library(uv::uv-static ALIAS uv_a)
set_target_properties(uv_a PROPERTIES POSITION_INDEPENDENT_CODE 1)
set(UVW_SOURCES
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/async.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/check.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/dns.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/emitter.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/fs.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/fs_event.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/fs_poll.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/idle.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/lib.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/loop.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/pipe.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/poll.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/prepare.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/process.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/signal.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/stream.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/tcp.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/thread.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/timer.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/tty.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/util.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/work.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src/uvw/udp.cpp
)
add_library(UVW_LIB ${UVW_SOURCES})
target_compile_definitions(UVW_LIB PUBLIC UVW_AS_LIB)
target_link_libraries(UVW_LIB uv::uv-static)
set(LibUV_LIBRARIES uv::uv-static UVW_LIB)
set(LibUV_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libuv/include
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/src
)
