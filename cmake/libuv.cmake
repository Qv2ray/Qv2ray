add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libuv)
add_library(uv::uv-static ALIAS uv_a)
set_target_properties(uv_a PROPERTIES POSITION_INDEPENDENT_CODE 1)
set(UVW_SOURCES
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/async.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/check.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/dns.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/emitter.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/fs.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/fs_event.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/fs_poll.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/idle.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/lib.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/loop.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/pipe.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/poll.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/prepare.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/process.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/signal.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/stream.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/tcp.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/thread.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/timer.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/tty.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/util.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/work.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw/uvw/udp.cpp
)
add_library(UVW_LIB ${UVW_SOURCES})
target_compile_definitions(UVW_LIB PUBLIC UVW_AS_LIB)
target_link_libraries(UVW_LIB uv::uv-static)
set(LibUV_LIBRARIES uv::uv-static UVW_LIB)
set(LibUV_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libuv/include
       ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/uvw
)
