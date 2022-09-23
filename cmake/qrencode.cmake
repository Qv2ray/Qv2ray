set(QT_QRENCODE_BASEDIR ${CMAKE_SOURCE_DIR}/3rdparty/qt-qrcode/lib)

include_directories(${QT_QRENCODE_BASEDIR})

set(QT_QRENCODE_SOURCES
    ${QT_QRENCODE_BASEDIR}/libqrencode/bitstream.c
    ${QT_QRENCODE_BASEDIR}/libqrencode/qrencode.c
    ${QT_QRENCODE_BASEDIR}/libqrencode/mqrspec.c
    ${QT_QRENCODE_BASEDIR}/libqrencode/qrinput.c
    ${QT_QRENCODE_BASEDIR}/libqrencode/qrspec.c
    ${QT_QRENCODE_BASEDIR}/libqrencode/split.c
    ${QT_QRENCODE_BASEDIR}/libqrencode/rsecc.c
    ${QT_QRENCODE_BASEDIR}/libqrencode/mmask.c
    ${QT_QRENCODE_BASEDIR}/libqrencode/mask.c
    ${QT_QRENCODE_BASEDIR}/libqrencode/qrencode_inner.h
    ${QT_QRENCODE_BASEDIR}/libqrencode/bitstream.h
    ${QT_QRENCODE_BASEDIR}/libqrencode/qrencode.h
    ${QT_QRENCODE_BASEDIR}/libqrencode/mqrspec.h
    ${QT_QRENCODE_BASEDIR}/libqrencode/qrinput.h
    ${QT_QRENCODE_BASEDIR}/libqrencode/qrspec.h
    ${QT_QRENCODE_BASEDIR}/libqrencode/split.h
    ${QT_QRENCODE_BASEDIR}/libqrencode/rsecc.h
    ${QT_QRENCODE_BASEDIR}/libqrencode/mmask.h
    ${QT_QRENCODE_BASEDIR}/libqrencode/mask.h
    ${QT_QRENCODE_BASEDIR}/QtQrCode
    ${QT_QRENCODE_BASEDIR}/qtqrcode_global.h
    ${QT_QRENCODE_BASEDIR}/QtQrCodePainter
    ${QT_QRENCODE_BASEDIR}/qtqrcodepainter.h
    ${QT_QRENCODE_BASEDIR}/qtqrcode.cpp
    ${QT_QRENCODE_BASEDIR}/qtqrcode.h
    ${QT_QRENCODE_BASEDIR}/qtqrcodepainter.cpp
    )

find_package(${QV_QT_LIBNAME} COMPONENTS Core Gui Svg REQUIRED)

set(QV2RAY_QRENCODE_LIBRARY qv2ray_qrencode)
add_library(qv2ray_qrencode STATIC ${QT_QRENCODE_SOURCES})
target_compile_definitions(${QV2RAY_QRENCODE_LIBRARY} PRIVATE
    -D__STATIC=static
    -DMAJOR_VERSION=3
    -DMINOR_VERSION=9
    -DMICRO_VERSION=0
    -DVERSION="0.0.0")

if(WITH_TESTS)
  enable_testing()
  add_definitions(-DWITH_TESTS=)
  add_definitions(-DSTATIC_IN_RELEASE=)
  add_subdirectory(tests)
else()
  add_definitions(-DSTATIC_IN_RELEASE=static)
endif()

target_link_libraries(${QV2RAY_QRENCODE_LIBRARY}
    ${QV_QT_LIBNAME}::Core
    ${QV_QT_LIBNAME}::Gui
    ${QV_QT_LIBNAME}::Svg)
