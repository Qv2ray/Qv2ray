if(ANDROID)
    # find_package(${QV_QT_LIBNAME} COMPONENTS AndroidExtras REQUIRED)
    # list(APPEND QV2RAY_PLATFORM_SOURCES
    #     ${CMAKE_SOURCE_DIR}/src/platforms/android/QvVPNService.hpp
    #     ${CMAKE_SOURCE_DIR}/src/platforms/android/QvVPNService.cpp
    #     )
    # list(APPEND QV2RAY_PLATFORM_LIBS ${QV_QT_LIBNAME}::AndroidExtras -llog -landroid)
endif()
