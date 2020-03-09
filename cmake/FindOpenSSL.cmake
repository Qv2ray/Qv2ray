set(CMAKE_FIND_LIBRARY_SUFFIXES .dll ${CMAKE_FIND_LIBRARY_SUFFIXES})
find_library(SSL_LIBRARY
            NAMES
              libssl-1_1-x64
              libssl-1_1
            PATH_SUFFIXES
              bin
)
find_library(CRYPTO_LIBRARY
            NAMES
              libcrypto-1_1-x64
              libcrypto-1_1
            PATH_SUFFIXES
              bin
)

message(${SSL_LIBRARY})
message(${CRYPTO_LIBRARY})