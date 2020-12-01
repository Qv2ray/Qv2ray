if(WIN32)
    set(CURL_LIBRARIES libcurl libssl libcrypto zlib)
endif()

find_package(CURL REQUIRED)

if(ANDROID)
    find_package(OpenSSL REQUIRED)
    set(CURL_LIBRARIES CURL::libcurl OpenSSL::SSL OpenSSL::Crypto)
endif()
