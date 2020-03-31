# Directories to look for dependencies
set(DIRS "${CMAKE_BINARY_DIR}")

# Path used for searching by FIND_XXX(), with appropriate suffixes added
if(CMAKE_PREFIX_PATH)
    foreach(dir ${CMAKE_PREFIX_PATH})
        list(APPEND DIRS "${dir}/bin" "${dir}/lib")
    endforeach()
endif()

# Append Qt's lib folder which is two levels above Qt5Widgets_DIR
list(APPEND DIRS "${Qt5Widgets_DIR}/../..")
list(APPEND DIRS "/usr/local/lib")
list(APPEND DIRS "/usr/lib")

include(InstallRequiredSystemLibraries)

message(STATUS "APPS: ${APPS}")
message(STATUS "QT_PLUGINS: ${QT_PLUGINS}")
message(STATUS "DIRS: ${DIRS}")

install(CODE "include(BundleUtilities)
      fixup_bundle(\"${APPS}\" \"${QT_PLUGINS}\" \"${DIRS}\")")

# Packaging
set(CPACK_PACKAGE_VENDOR "Qv2ray Development Group")
set(CPACK_PACKAGE_VERSION ${QV2RAY_VERSION_STRING})
set(CPACK_PACKAGE_DESCRIPTION "Cross-platform V2Ray Client written in Qt.")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/Qv2ray/Qv2ray")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/assets/icons\\\\qv2ray.ico")

if(BUILD_NSIS)
    if(MSVC)
        set(CPACK_GENERATOR "NSIS")
        set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/assets/icons\\\\qv2ray.ico")
        set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/assets/icons\\\\qv2ray.ico")
        set(CPACK_NSIS_DISPLAY_NAME "Qv2ray")
        set(CPACK_NSIS_PACKAGE_NAME "qv2ray")
    endif()
endif()

include(CPack)