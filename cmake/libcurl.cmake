if(ANDROID)
    set(CURL_LIBRARIES -lcurl -lssl -lcrypto -lz)
elseif(WIN32)
    set(CURL_LIBRARIES libcurl libssl libcrypto zlib)
endif()

find_package(CURL REQUIRED)
