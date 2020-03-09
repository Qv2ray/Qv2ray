set(CMAKE_FIND_LIBRARY_SUFFIXES .dll ${CMAKE_FIND_LIBRARY_SUFFIXES})
find_library(SSL_LIBRARY
            NAMES
              libssl-1_1-x64
              libssl-1_1
            PATHS
                ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}
            PATH_SUFFIXES
              bin
            NO_DEFAULT_PATH
)
find_library(CRYPTO_LIBRARY
            NAMES
              libcrypto-1_1-x64
              libcrypto-1_1
            PATHS
                ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}
            PATH_SUFFIXES
              bin
            NO_DEFAULT_PATH
)

message(${SSL_LIBRARY})
message(${CRYPTO_LIBRARY})