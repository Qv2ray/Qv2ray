if(QV2RAY_ZXING_PROVIDER STREQUAL "module")
    set(ZXING_DIR ${CMAKE_SOURCE_DIR}/3rdparty/zxing-cpp/core)
    set(ZXING_SOURCES
        ${ZXING_DIR}/src/BitArray.cpp
        ${ZXING_DIR}/src/BitMatrix.cpp
        ${ZXING_DIR}/src/BitSource.cpp
        ${ZXING_DIR}/src/CharacterSetECI.cpp
        ${ZXING_DIR}/src/ConcentricFinder.cpp
        ${ZXING_DIR}/src/DecodeHints.cpp
        ${ZXING_DIR}/src/GenericGF.cpp
        ${ZXING_DIR}/src/GenericGFPoly.cpp
        ${ZXING_DIR}/src/GenericLuminanceSource.cpp
        ${ZXING_DIR}/src/GlobalHistogramBinarizer.cpp
        ${ZXING_DIR}/src/GridSampler.cpp
        ${ZXING_DIR}/src/PerspectiveTransform.cpp
        ${ZXING_DIR}/src/qrcode/QRBitMatrixParser.cpp
        ${ZXING_DIR}/src/qrcode/QRCodecMode.cpp
        ${ZXING_DIR}/src/qrcode/QRDataBlock.cpp
        ${ZXING_DIR}/src/qrcode/QRDecoder.cpp
        ${ZXING_DIR}/src/qrcode/QRDetector.cpp
        ${ZXING_DIR}/src/qrcode/QREncoder.cpp
        ${ZXING_DIR}/src/qrcode/QRErrorCorrectionLevel.cpp
        ${ZXING_DIR}/src/qrcode/QRFormatInformation.cpp
        ${ZXING_DIR}/src/qrcode/QRMaskUtil.cpp
        ${ZXING_DIR}/src/qrcode/QRMatrixUtil.cpp
        ${ZXING_DIR}/src/qrcode/QRReader.cpp
        ${ZXING_DIR}/src/qrcode/QRVersion.cpp
        ${ZXING_DIR}/src/qrcode/QRWriter.cpp
        ${ZXING_DIR}/src/ReedSolomonDecoder.cpp
        ${ZXING_DIR}/src/ReedSolomonEncoder.cpp
        ${ZXING_DIR}/src/Result.cpp
        ${ZXING_DIR}/src/ResultMetadata.cpp
        ${ZXING_DIR}/src/ReadBarcode.cpp
        ${ZXING_DIR}/src/textcodec/Big5MapTable.cpp
        ${ZXING_DIR}/src/textcodec/Big5TextDecoder.cpp
        ${ZXING_DIR}/src/textcodec/Big5TextEncoder.cpp
        ${ZXING_DIR}/src/textcodec/GBTextDecoder.cpp
        ${ZXING_DIR}/src/textcodec/GBTextEncoder.cpp
        ${ZXING_DIR}/src/textcodec/JPTextDecoder.cpp
        ${ZXING_DIR}/src/textcodec/JPTextEncoder.cpp
        ${ZXING_DIR}/src/textcodec/KRHangulMapping.cpp
        ${ZXING_DIR}/src/textcodec/KRTextDecoder.cpp
        ${ZXING_DIR}/src/textcodec/KRTextEncoder.cpp
        ${ZXING_DIR}/src/TextDecoder.cpp
        ${ZXING_DIR}/src/TextEncoder.cpp
        ${ZXING_DIR}/src/TextUtfEncoding.cpp
        )
    set(ZXING_INCLUDE_PATH
        ${ZXING_DIR}/src
        )
    set(ZXING_LIBRARY qv2ray_zxing)
    add_library(${ZXING_LIBRARY} STATIC
        ${ZXING_SOURCES}
        )
    target_include_directories(${ZXING_LIBRARY} PUBLIC
        ${ZXING_INCLUDE_PATH}
        )
elseif(QV2RAY_ZXING_PROVIDER STREQUAL "package")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ZXING REQUIRED zxing)
    #set(ZXING_LIBRARY ${ZXING_LIBRARIES})
    set(ZXING_LIBRARY ZXing)
    set(ZXING_INCLUDE_PATH ${ZXING_INCLUDE_DIRS})
endif()
