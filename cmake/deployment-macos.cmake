find_package(Iconv REQUIRED)
find_library(CARBON NAMES Carbon)
find_library(COCOA NAMES Cocoa)
find_library(SECURITY NAMES Security)

target_link_libraries(qv2ray PRIVATE Iconv::Iconv ${CARBON} ${COCOA} ${SECURITY})
target_include_directories(qv2ray PRIVATE ${Iconv_INCLUDE_DIR})

set(MACOSX_ICON "${CMAKE_SOURCE_DIR}/assets/icons/qv2ray.icns")
set(MACOSX_PLIST "${CMAKE_SOURCE_DIR}/assets/MacOSXBundleInfo.plist.in")

set_source_files_properties(${QV2RAY_QM_FILES}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/lang
    )

target_sources(qv2ray PRIVATE ${MACOSX_ICON})

set_target_properties(qv2ray
    PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${MACOSX_PLIST}
    MACOSX_BUNDLE_BUNDLE_NAME "Qv2ray"
    MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
    MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2019-2021 Qv2ray Development Group, 2021 Moody"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.qv2ray"
    MACOSX_BUNDLE_ICON_FILE "qv2ray.icns"
    MACOSX_BUNDLE_INFO_STRING "Created by Moody"
    MACOSX_BUNDLE_LONG_VERSION_STRING ${CMAKE_PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION}
    RESOURCE ${MACOSX_ICON}
    )

## Destination paths below are relative to ${CMAKE_INSTALL_PREFIX}
install(TARGETS qv2ray BUNDLE DESTINATION .)

# For Global Target
list(APPEND DIRS "${CMAKE_BINARY_DIR}")

# For Qv2rayBase
list(APPEND DIRS "${CMAKE_BINARY_DIR}/src/Qv2rayBase")
list(APPEND DIRS "${CMAKE_CURRENT_BINARY_DIR}")
list(APPEND DIRS "${Qt6Core_DIR}/../../")
list(APPEND DIRS "${Qt6Core_DIR}/../../../bin/")

if(CMAKE_PREFIX_PATH)
    foreach(dir ${CMAKE_PREFIX_PATH})
        list(APPEND DIRS "${dir}/bin" "${dir}/lib")
    endforeach()
endif()

list(APPEND DIRS "${Qt6Core_DIR}/../..")
list(APPEND DIRS "/usr/lib")
list(APPEND DIRS "/usr/local/lib")

set(APP "${CMAKE_INSTALL_PREFIX}/qv2ray.app")
install(CODE "execute_process(COMMAND \"${Qt6_DIR}/../../../bin/macdeployqt\" \"${APP}\")")
install(CODE "include(BundleUtilities)")
install(CODE "fixup_bundle(\"${APP}\"   \"\"   \"${DIRS}\")")
install(CODE "message(\"Run install_name_tool\")")
install(CODE "execute_process(COMMAND \"install_name_tool\" -add_rpath \"@executable_path/../Frameworks\" \"${APP}/Contents/MacOS/qv2ray\")")
install(CODE "message(\"Completed\")")
