find_package(Iconv REQUIRED)
set(ICONV_LIBRARY
    Iconv::Iconv
)
set(ICONV_INCLUDE_PATH
    ${Iconv_INCLUDE_DIR}
)