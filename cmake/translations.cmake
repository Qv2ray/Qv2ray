# ==================================================================================
# Qv2ray Translations
# ==================================================================================

set(TRANSLATIONS_DIR ${CMAKE_SOURCE_DIR}/translations)
file(GLOB TRANSLATIONS_TS ${TRANSLATIONS_DIR}/*.ts)

if(QV2RAY_QT6)
    find_package(${QV_QT_LIBNAME} COMPONENTS Linguist REQUIRED)
    qt_add_translation(QV2RAY_QM_FILES ${TRANSLATIONS_TS})
else()
    find_package(${QV_QT_LIBNAME} COMPONENTS LinguistTools REQUIRED)
    qt5_add_translation(QV2RAY_QM_FILES ${TRANSLATIONS_TS})
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]$" OR NOT DEFINED CMAKE_BUILD_TYPE)
    # Only do this in Debug Build
    add_custom_target(lupdate
        COMMENT "Generating Translation Sources"
        COMMAND Qt::lupdate
        ${CMAKE_SOURCE_DIR}/src
        -ts translations/en_US.ts
        -locations none
        -no-obsolete
        -no-sort
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_dependencies(qv2ray_baselib lupdate)
endif()
