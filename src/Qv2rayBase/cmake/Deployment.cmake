generate_export_header(Qv2rayBase
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/Qv2rayBase/Qv2rayBase_export.h)

configure_package_config_file(cmake/Qv2rayBase.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Qv2rayBaseConfig.cmake
    INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/Qv2rayBase)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/Qv2rayBaseConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

install(TARGETS Qv2rayBase
    EXPORT Qv2rayBaseTargets
    COMPONENT "development"
    EXCLUDE_FROM_ALL
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Some development required files.

# BEGIN Include Headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    COMPONENT "development"
    EXCLUDE_FROM_ALL
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Qv2rayBase/Qv2rayBase_export.h
    COMPONENT "development"
    EXCLUDE_FROM_ALL
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Qv2rayBase)
# END Include Headers


# BEGIN Target Support Files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Qv2rayBaseConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/Qv2rayBaseConfigVersion.cmake
    COMPONENT "development"
    EXCLUDE_FROM_ALL
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Qv2rayBase)

export(EXPORT Qv2rayBaseTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/Qv2rayBaseTargets.cmake"
    NAMESPACE Qv2ray::
)

install(EXPORT Qv2rayBaseTargets
    COMPONENT "development"
    EXCLUDE_FROM_ALL
    FILE Qv2rayBaseTargets.cmake
    NAMESPACE Qv2ray::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Qv2rayBase
)
# END
