find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

if(MSVC)
    find_package(gRPC CONFIG REQUIRED)
    set(QV2RAY_BACKEND_LIBRARIES c-ares::cares gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts)
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    if(UNIX AND NOT APPLE)
        pkg_check_modules(GRPC REQUIRED grpc++ grpc)
        set(QV2RAY_BACKEND_LIBRARIES ${GRPC_LIBRARIES})
    else()
        find_library(UPB_LIBRARY NAMES upb)
        find_library(ADDRESS_SORTING NAMES address_sorting)
        pkg_check_modules(GRPC REQUIRED grpc++ grpc gpr)
        set(QV2RAY_BACKEND_LIBRARIES ${GRPC_LINK_LIBRARIES} ${UPB_LIBRARY} ${ADDRESS_SORTING})
    endif()
endif()

set(API_PROTO "${CMAKE_SOURCE_DIR}/assets/v2ray_api.proto")
set(API_PROTO_PATH "${CMAKE_SOURCE_DIR}/assets")
set(API_PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/v2ray_api.pb.cc")
set(API_PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/v2ray_api.pb.h")
set(API_GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/v2ray_api.grpc.pb.cc")
set(API_GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/v2ray_api.grpc.pb.h")
add_custom_command(
    OUTPUT "${API_GRPC_SRCS}" "${API_GRPC_HDRS}" "${API_PROTO_HDRS}" "${API_PROTO_SRCS}"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${API_PROTO_PATH}"
    --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}"
    "${API_PROTO}"
    DEPENDS "${API_PROTO}"
    )
