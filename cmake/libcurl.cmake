if(ANDROID)
    set(CURL_LIBRARIES -lcurl)
endif()

if(WIN32)
    find_package(CURL CONFIG REQUIRED)
else()
    find_package(CURL REQUIRED)
endif()
