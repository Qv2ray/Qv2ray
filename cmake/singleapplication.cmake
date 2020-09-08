if(QV2RAY_SINGLEAPPLICATION_PROVIDER STREQUAL "module")
    set(SINGLEAPPLICATION_DIR ${CMAKE_SOURCE_DIR}/3rdparty/SingleApplication)
    set(SINGLEAPPLICATION_SOURCES
        ${SINGLEAPPLICATION_DIR}/singleapplication.cpp
        ${SINGLEAPPLICATION_DIR}/singleapplication_p.cpp
        )
elseif(QV2RAY_SINGLEAPPLICATION_PROVIDER STREQUAL "package")
    find_library(SINGLEAPPLICATION_LIBRARY NAMES SingleApplication)
    find_path(SINGLEAPPLICATION_DIR NAMES singleapplication.h PATH_SUFFIXES singleapplication)
endif()
