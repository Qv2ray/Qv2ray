if(NOT USE_LIBQVB)
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
    
    find_library(GRPC_LIBRARY NAMES grpc)
    find_library(GRPCPP_LIBRARY NAMES grpc++)

    if(UNIX AND NOT APPLE)
        set(QV2RAY_BACKEND_LIBRARIES ${GRPCPP_LIBRARY} ${GRPC_LIBRARY})
    elseif(APPLE)
        find_library(UPB_LIBRARY NAMES upb)
        set(QV2RAY_BACKEND_LIBRARIES ${GRPCPP_LIBRARY} ${GRPC_LIBRARY} ${UPB_LIBRARY})
    elseif(WIN32)
        find_library(ABSL_BASE NAMES absl_base)
        find_library(ABSL_STR NAMES absl_strings)
        find_library(ABSL_THROW_DELEGATE NAMES absl_throw_delegate)
        set(QV2RAY_BACKEND_LIBRARIES ${GRPCPP_LIBRARY} ${GRPC_LIBRARY} ${ABSL_BASE} ${ABSL_STR} ${ABSL_THROW_DELEGATE})
    endif()

    set(API_PROTO "${CMAKE_SOURCE_DIR}/tools/v2ray_api.proto")
    set(API_PROTO_PATH "${CMAKE_SOURCE_DIR}/tools")
    set(API_GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/v2ray_api.pb.cc")
    set(API_GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/v2ray_api.pb.h")
    add_custom_command(
        OUTPUT "${API_GRPC_SRCS}" "${API_GRPC_HDRS}"
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
            --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
            -I "${API_PROTO_PATH}"
            --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}"
            "${API_PROTO}"
        DEPENDS "${API_PROTO}"
    )
else()
    add_definitions(-DBACKEND_LIBQVB)
    if(UNIX AND NOT APPLE)
        set(QV2RAY_BACKEND_LIBRARIES ${CMAKE_SOURCE_DIR}/libs/libqvb-linux64.a)
    elseif(APPLE)
        set(QV2RAY_BACKEND_LIBRARIES ${CMAKE_SOURCE_DIR}/libs/libqvb-darwin.a)
    endif()
endif()
