target_link_libraries(qv2ray_base wininet wsock32 ws2_32 user32 Rasapi32 Iphlpapi Dbghelp)

install(TARGETS qv2ray RUNTIME DESTINATION .)

set(DIRS "${CMAKE_BINARY_DIR}")
if(CMAKE_PREFIX_PATH)
    foreach(dir ${CMAKE_PREFIX_PATH})
        list(APPEND DIRS "${dir}/bin" "${dir}/lib")
    endforeach()
endif()

list(APPEND DIRS "${Qt6Core_DIR}/../..")

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)

set(APP "${CMAKE_INSTALL_PREFIX}/qv2ray.exe")
#include(InstallRequiredSystemLibraries)
install(CODE "include(BundleUtilities)")
install(CODE "fixup_bundle(\"${APP}\"   \"\"   \"${DIRS}\")")
install(CODE "message(\"\")")
install(CODE "execute_process(COMMAND \"${Qt6_DIR}/../../../bin/windeployqt.exe\" \"${APP}\" --no-compiler-runtime)")
install(CODE "message(\"Completed\")")
