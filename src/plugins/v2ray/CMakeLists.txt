find_package(Protobuf REQUIRED)

add_library(V2RayStatsBackendAPI INTERFACE)
add_library(V2RayPlugin::BackendDeps ALIAS V2RayStatsBackendAPI)

if(QV2RAY_AVOID_GRPC)
    add_compile_definitions(QV2RAY_NO_GRPC)
    set(NANO_PB
            ${CMAKE_CURRENT_LIST_DIR}/core/pb_common.c
            ${CMAKE_CURRENT_LIST_DIR}/core/pb_common.h
            ${CMAKE_CURRENT_LIST_DIR}/core/pb_decode.h
            ${CMAKE_CURRENT_LIST_DIR}/core/pb_decode.c
            ${CMAKE_CURRENT_LIST_DIR}/core/pb_encode.c
            ${CMAKE_CURRENT_LIST_DIR}/core/pb_encode.h
            ${CMAKE_CURRENT_LIST_DIR}/core/pb.h
            ${CMAKE_CURRENT_LIST_DIR}/core/api.pb.h
            ${CMAKE_CURRENT_LIST_DIR}/core/api.pb.c
            )
else()
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
    find_package(gRPC QUIET)
    if(NOT gRPC_FOUND)
        find_package(PkgConfig REQUIRED)
        if(UNIX AND NOT APPLE)
            pkg_check_modules(GRPC REQUIRED grpc++ grpc)
            target_link_libraries(V2RayStatsBackendAPI INTERFACE ${GRPC_LIBRARIES})
        else()
            find_library(UPB_LIBRARY NAMES upb)
            find_library(ADDRESS_SORTING NAMES address_sorting)
            pkg_check_modules(GRPC REQUIRED grpc++ grpc gpr)
            target_link_libraries(V2RayStatsBackendAPI INTERFACE ${GRPC_LINK_LIBRARIES} ${UPB_LIBRARY} ${ADDRESS_SORTING})
        endif()
    else()
        target_link_libraries(V2RayStatsBackendAPI INTERFACE gRPC::grpc++)
    endif()
endif()
target_link_libraries(V2RayStatsBackendAPI INTERFACE protobuf::libprotobuf)

macro(generate_v2ray_grpc_source proto)
    get_filename_component(PROTO_ABS_DIR_PATH ${proto} DIRECTORY)
    file(RELATIVE_PATH _r ${PROTO_SOURCE_DIR} ${proto})
    get_filename_component(PROTO_FILE_DIR ${_r} DIRECTORY)
    get_filename_component(PROTO_FILE_NAME ${_r} NAME_WE)

    set(PROTO_HEADER_FILE "${PROTO_GENERATED_DIR}/${PROTO_FILE_DIR}/${PROTO_FILE_NAME}.pb.h")
    set(PROTO_SOURCE_FILE "${PROTO_GENERATED_DIR}/${PROTO_FILE_DIR}/${PROTO_FILE_NAME}.pb.cc")

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

    list(APPEND PROTO_HEADERS ${PROTO_HEADER_FILE} ${PROTO_GRPC_HEADER_FILE})
    list(APPEND PROTO_SOURCES ${PROTO_SOURCE_FILE} ${PROTO_GRPC_SOURCE_FILE})
endmacro()

function(qv2ray_internal_add_v2ray_plugin name classname classprefix)
    set(PROTO_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/${classprefix}")
    set(PROTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/assets")
    make_directory(${PROTO_GENERATED_DIR})
	if(NOT QV2RAY_AVOID_GRPC)
    generate_v2ray_grpc_source("${CMAKE_CURRENT_LIST_DIR}/assets/${classprefix}-Stats.proto")
    endif()

    qv2ray_add_plugin(${name} Widgets
        INSTALL_PREFIX_MACOS "${CMAKE_INSTALL_PREFIX}/qv2ray.app/Contents/Resources/plugins"
        CLASS_NAME ${classname})

    target_sources(${name} PRIVATE
        ${NANO_PB}
        ${PROTO_SOURCES} ${PROTO_HEADERS}
        ${CMAKE_CURRENT_LIST_DIR}/ui/w_V2RayKernelSettings.hpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/w_V2RayKernelSettings.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/w_V2RayKernelSettings.ui
        ${CMAKE_CURRENT_LIST_DIR}/common/SettingsModels.hpp
        ${CMAKE_CURRENT_LIST_DIR}/common/CommonHelpers.hpp
        ${CMAKE_CURRENT_LIST_DIR}/common/CommonHelpers.cpp
        ${CMAKE_CURRENT_LIST_DIR}/core/V2RayAPIStats.hpp
        ${CMAKE_CURRENT_LIST_DIR}/core/V2RayAPIStats.cpp

        ${CMAKE_CURRENT_LIST_DIR}/V2RayCorePluginTemplate.hpp
        ${CMAKE_CURRENT_LIST_DIR}/V2RayCorePluginTemplate.cpp

        ${CMAKE_CURRENT_LIST_DIR}/core/${classprefix}/Kernel.hpp
        ${CMAKE_CURRENT_LIST_DIR}/core/${classprefix}/Kernel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/core/${classprefix}/ProfileGenerator.hpp
        ${CMAKE_CURRENT_LIST_DIR}/core/${classprefix}/ProfileGenerator.cpp
        )

    if(QV2RAY_V2RAY_PLUGIN_USE_PROTOBUF)
        target_compile_definitions(${name} PRIVATE QV2RAY_V2RAY_PLUGIN_USE_PROTOBUF)
    endif()

    target_compile_definitions(${name} PRIVATE QT_NO_CAST_FROM_ASCII)
    target_compile_definitions(${name} PRIVATE V2RayCorePluginClass=${classname})
    target_compile_definitions(${name} PRIVATE V2RayPluginNamespace=${classprefix})
    target_compile_definitions(${name} PRIVATE V2RayCoreType=CORETYPE_${classprefix})
    target_compile_definitions(${name} PRIVATE V2RayCoreStatsHeaderName="${classprefix}-Stats.grpc.pb.h")

    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../PluginsCommon)
    target_include_directories(${name} PRIVATE ${PROTO_GENERATED_DIR})
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/core/${classprefix})
    target_link_libraries(${name}
        PRIVATE
            Qt::Network
            Qt::Gui
            V2RayPlugin::BackendDeps)
endfunction()

qv2ray_internal_add_v2ray_plugin(QvPlugin-V2RayRustSupport "V2RayRustPlugin" V2RayRust)
qv2ray_internal_add_v2ray_plugin(QvPlugin-V2Ray4Support "V2Ray4Plugin" V2Ray)
qv2ray_internal_add_v2ray_plugin(QvPlugin-V2Ray5Support "V2Ray5Plugin" V2Ray5)
qv2ray_internal_add_v2ray_plugin(QvPlugin-V2RaySNSupport "V2RaySNPlugin" V2RaySN)
