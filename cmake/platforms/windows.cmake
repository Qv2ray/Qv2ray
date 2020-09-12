find_package(OpenSSL REQUIRED)
target_link_libraries(qv2ray-baselib wininet wsock32 ws2_32 user32 Iphlpapi OpenSSL::SSL OpenSSL::Crypto Dbghelp)
install(TARGETS qv2ray RUNTIME DESTINATION .)
if(NOT QV2RAY_EMBED_TRANSLATIONS)
    install(FILES ${QV2RAY_QM_FILES} DESTINATION lang)
endif()
install(DIRECTORY ${CMAKE_BINARY_DIR}/winqt/ DESTINATION .)
set(APPS "\${CMAKE_INSTALL_PREFIX}/qv2ray.exe")
include(cmake/deployment.cmake)

if(QV2RAY_AUTO_DEPLOY)
    add_custom_command(TARGET qv2ray POST_BUILD
        COMMAND ${Qt5_DIR}/../../../bin/windeployqt ${CMAKE_BINARY_DIR}/qv2ray.exe --compiler-runtime --verbose 2 --dir ${CMAKE_BINARY_DIR}/winqt/
        )
endif()
