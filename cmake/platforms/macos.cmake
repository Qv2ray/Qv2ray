find_package(Iconv REQUIRED)
find_library(CARBON NAMES Carbon)
find_library(COCOA NAMES Cocoa)
find_library(SECURITY NAMES Security)

target_link_libraries(qv2ray PUBLIC
    Iconv::Iconv
    ${CARBON}
    ${COCOA}
    ${SECURITY}
    )
target_include_directories(qv2ray PRIVATE
    ${Iconv_INCLUDE_DIR}
    )

set(MACOSX_ICON "${CMAKE_SOURCE_DIR}/assets/icons/qv2ray.icns")
set(MACOSX_PLIST "${CMAKE_SOURCE_DIR}/assets/MacOSXBundleInfo.plist.in")

set_source_files_properties(${QV2RAY_QM_FILES}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/lang
    )

target_sources(qv2ray PRIVATE
    ${MACOSX_ICON}
    )

set_target_properties(qv2ray
    PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${MACOSX_PLIST}
    MACOSX_BUNDLE_BUNDLE_NAME "Qv2ray"
    MACOSX_BUNDLE_BUNDLE_VERSION ${QV2RAY_VERSION_STRING}
    MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2019-2021 Qv2ray Development Group"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.qv2ray"
    MACOSX_BUNDLE_ICON_FILE "qv2ray.icns"
    MACOSX_BUNDLE_INFO_STRING "Created by Qv2ray Workgroup"
    MACOSX_BUNDLE_LONG_VERSION_STRING ${QV2RAY_VERSION_STRING}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${QV2RAY_VERSION_STRING}
    RESOURCE ${MACOSX_ICON}
    )

# Destination paths below are relative to ${CMAKE_INSTALL_PREFIX}
install(TARGETS qv2ray
    BUNDLE  DESTINATION .   COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    )
set(APPS "\${CMAKE_INSTALL_PREFIX}/qv2ray.app")
include(cmake/deployment.cmake)

if(QV2RAY_AUTO_DEPLOY)
    if(QV2RAY_QT6)
        set(QV2RAY_QtX_DIR ${Qt6_DIR})
    else()
        set(QV2RAY_QtX_DIR ${Qt5_DIR})
    endif()
    add_custom_command(TARGET qv2ray POST_BUILD COMMAND ${QV2RAY_QtX_DIR}/../../../bin/macdeployqt ${CMAKE_BINARY_DIR}/qv2ray.app)
endif()
