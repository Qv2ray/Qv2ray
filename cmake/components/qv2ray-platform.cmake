if(ANDROID)
    find_package(Qt5 COMPONENTS AndroidExtras REQUIRED)
    list(APPEND QV2RAY_PLATFORM_SOURCES
        ${CMAKE_SOURCE_DIR}/src/platforms/android/QvVPNService.hpp
        ${CMAKE_SOURCE_DIR}/src/platforms/android/QvVPNService.cpp
        )
    list(APPEND QV2RAY_PLATFORM_LIBS Qt5::AndroidExtras)
endif()
