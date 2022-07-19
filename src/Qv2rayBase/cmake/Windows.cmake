include(${CMAKE_CURRENT_LIST_DIR}/versioninfo/generate_product_version.cmake)
generate_product_version(
    QV2RAYBASE_RC
    NAME               "Qv2ray Base Library"
    BUNDLE             "Qv2ray Project Family"
    ICON               "${CMAKE_CURRENT_SOURCE_DIR}/assets/qv2ray.ico"
    VERSION_MAJOR      ${PROJECT_VERSION_MAJOR}
    VERSION_MINOR      ${PROJECT_VERSION_MINOR}
    VERSION_PATCH      ${PROJECT_VERSION_PATCH}
    VERSION_REVISION   ${PROJECT_VERSION_TWEAK}
    COMPANY_NAME       "Moody"
    COMPANY_COPYRIGHT  "Moody 2021"
    FILE_DESCRIPTION   "Qv2ray Base Library"
    )
add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)

if(NOT MINGW)
    set(CMAKE_DEBUG_POSTFIX d)
    add_compile_options("/utf-8")
    add_definitions(-D_WIN32_WINNT=0x600 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
endif()
