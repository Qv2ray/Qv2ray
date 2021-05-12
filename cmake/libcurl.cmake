if(WIN32)
    set(CURL_LIBRARIES libcurl libssl libcrypto zlib)
endif()

find_package(CURL REQUIRED)
