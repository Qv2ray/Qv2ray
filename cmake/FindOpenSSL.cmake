set(CMAKE_FIND_LIBRARY_SUFFIXES .dll ${CMAKE_FIND_LIBRARY_SUFFIXES})
file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _programfiles)
list(APPEND CMAKE_SYSTEM_IGNORE_PATH "${_programfiles}/Git/mingw64")
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