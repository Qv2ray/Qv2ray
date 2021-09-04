find_package(Protobuf REQUIRED)


if(QV2RAY_AVOID_GRPC)
    add_compile_definitions(QV2RAY_NO_GRPC)
else()
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
    find_package(gRPC QUIET)
    if(NOT gRPC_FOUND)
        find_package(PkgConfig REQUIRED)
        if(UNIX AND NOT APPLE)
            pkg_check_modules(GRPC REQUIRED grpc++ grpc)
            set(QV2RAY_BACKEND_LIBRARY ${GRPC_LIBRARIES})
        else()
            find_library(UPB_LIBRARY NAMES upb)
            find_library(ADDRESS_SORTING NAMES address_sorting)
            pkg_check_modules(GRPC REQUIRED grpc++ grpc gpr)
            set(QV2RAY_BACKEND_LIBRARY ${GRPC_LINK_LIBRARIES} ${UPB_LIBRARY} ${ADDRESS_SORTING})
        endif()
    else()
        set(QV2RAY_BACKEND_LIBRARY gRPC::grpc++)
    endif()
endif()

macro(proto_gen_v2ray_files proto)
    get_filename_component(PROTO_ABS_DIR_PATH ${proto} DIRECTORY)
    file(RELATIVE_PATH _r ${PROTO_SOURCE_DIR} ${proto})
    get_filename_component(PROTO_FILE_DIR ${_r} DIRECTORY)
    get_filename_component(PROTO_FILE_NAME ${_r} NAME_WE)

    set(PROTO_HEADER_FILE "${PROTO_GENERATED_DIR}/${PROTO_FILE_DIR}/${PROTO_FILE_NAME}.pb.h")
    set(PROTO_SOURCE_FILE "${PROTO_GENERATED_DIR}/${PROTO_FILE_DIR}/${PROTO_FILE_NAME}.pb.cc")

    if(PROTO_FILE_DIR MATCHES "/command$" AND NOT QV2RAY_AVOID_GRPC)
        message("Additional gRPC generation required: ${PROTO_FILE_DIR}/${PROTO_FILE_NAME}.proto")
        set(PROTO_GRPC_HEADER_FILE "${PROTO_GENERATED_DIR}/${PROTO_FILE_DIR}/${PROTO_FILE_NAME}.grpc.pb.h")
        set(PROTO_GRPC_SOURCE_FILE "${PROTO_GENERATED_DIR}/${PROTO_FILE_DIR}/${PROTO_FILE_NAME}.grpc.pb.cc")
        add_custom_command(
            OUTPUT "${PROTO_HEADER_FILE}" "${PROTO_SOURCE_FILE}" "${PROTO_GRPC_HEADER_FILE}" "${PROTO_GRPC_SOURCE_FILE}"
            COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
            ARGS
            --grpc_out "${PROTO_GENERATED_DIR}"
            --cpp_out "${PROTO_GENERATED_DIR}"
            -I "${PROTO_SOURCE_DIR}"
            --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}"
            "${proto}"
            DEPENDS "${proto}"
            )
    else()
        add_custom_command(
            COMMENT "Generate protobuf files for ${proto}"
            OUTPUT "${PROTO_HEADER_FILE}" "${PROTO_SOURCE_FILE}"
            COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
            ARGS
            --cpp_out "${PROTO_GENERATED_DIR}"
            -I "${PROTO_SOURCE_DIR}"
            "${proto}"
            DEPENDS "${proto}"
            )
    endif()

    list(APPEND PROTO_HEADERS ${PROTO_HEADER_FILE} ${PROTO_GRPC_HEADER_FILE})
    list(APPEND PROTO_SOURCES ${PROTO_SOURCE_FILE} ${PROTO_GRPC_SOURCE_FILE})
endmacro()

option(QV2RAY_V2RAY_PLUGIN_USE_PROTOBUF "Use protobuf as V2Ray configuration API" OFF)

set(PROTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/3rdparty/v2ray-core/")
if(QV2RAY_V2RAY_PLUGIN_USE_PROTOBUF)
    file(GLOB_RECURSE PROTO_FILES "${PROTO_SOURCE_DIR}/*.proto")
else()
    set(PROTO_FILES "${PROTO_SOURCE_DIR}/app/stats/command/command.proto")
endif()

set(PROTO_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/v2ray")
make_directory(${PROTO_GENERATED_DIR})
foreach(proto ${PROTO_FILES})
    proto_gen_v2ray_files(${proto})
endforeach()
set_source_files_properties(FILES ${PROTO_HEADERS} ${PROTO_SOURCES} PROPERTIES SKIP_AUTOGEN TRUE)

qv2ray_add_plugin(QvPlugin-BuiltinV2RaySupport Widgets
    INSTALL_PREFIX_MACOS "${CMAKE_INSTALL_PREFIX}/qv2ray.app/Contents/Resources/plugins"
    CLASS_NAME "BuiltinV2RayCorePlugin")

target_sources(QvPlugin-BuiltinV2RaySupport PRIVATE
    ${PROTO_SOURCES} ${PROTO_HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}/ui/w_V2RayKernelSettings.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/w_V2RayKernelSettings.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/w_V2RayKernelSettings.ui
    ${CMAKE_CURRENT_LIST_DIR}/common/SettingsModels.hpp
    ${CMAKE_CURRENT_LIST_DIR}/common/CommonHelpers.hpp
    ${CMAKE_CURRENT_LIST_DIR}/common/CommonHelpers.cpp
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinV2RayCorePlugin.hpp
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinV2RayCorePlugin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/V2RayAPIStats.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/V2RayAPIStats.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/V2RayKernel.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/V2RayKernel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/V2RayGoKernel.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/V2RayGoKernel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/V2RayProfileGenerator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/V2RayProfileGenerator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/V2RayGoProfileGenerator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/V2RayGoProfileGenerator.cpp
    )

if(QV2RAY_V2RAY_PLUGIN_USE_PROTOBUF)
    target_compile_definitions(QvPlugin-BuiltinV2RaySupport PRIVATE QV2RAY_V2RAY_PLUGIN_USE_PROTOBUF)
endif()

target_compile_definitions(QvPlugin-BuiltinV2RaySupport PRIVATE QT_NO_CAST_FROM_ASCII)

target_include_directories(QvPlugin-BuiltinV2RaySupport PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../PluginsCommon)
target_include_directories(QvPlugin-BuiltinV2RaySupport PRIVATE ${PROTO_GENERATED_DIR})
target_link_libraries(QvPlugin-BuiltinV2RaySupport
    PRIVATE
    Qt::Network
    Qt::Gui
    protobuf::libprotobuf
    ${QV2RAY_BACKEND_LIBRARY})
