find_package(Protobuf REQUIRED)
if(MSVC)
    set(Protobuf_USE_STATIC_LIBS ON)
endif()
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/tools/v2ray_geosite.proto)
set(QV2RAY_PROTOBUF_LIBRARY 
    protobuf::libprotobuf
)