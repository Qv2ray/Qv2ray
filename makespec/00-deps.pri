message(" ")
message("Configuring Qv2ray Dependencies...")

defineTest(Qv2rayQMakeError)　{
    message(" ")
    message("-----------------------------------------------")
    message("Cannot continue: ")
    message("  --> Qv2ray is not properly configured yet: ")
    message("      $$ARGS")
    message("  --> Please read the build wiki: https://github.com/Qv2ray/Qv2ray/wiki/Manually-Build-Qv2ray")
    message("-----------------------------------------------")
    message(" ")
    warning("IF YOU THINK IT'S A MISTAKE, PLEASE OPEN AN ISSUE")
    error("! ABORTING THE BUILD !")
    message(" ")
}

CONFIG += enable_decoder_qr_code enable_encoder_qr_code
include($$PWD/../3rdparty/qzxing/src/QZXing-components.pri)
include($$PWD/../3rdparty/SingleApplication/singleapplication.pri)
include($$PWD/../3rdparty/QNodeEditor/QNodeEditor.pri)

#include(3rdparty/x2struct/x2struct.pri)

message("Adding QHttpServer Support")
message("  --> Adding qhttpserver")
HEADERS += \
    $$PWD/../3rdparty/qhttpserver/src/qhttpconnection.h \
    $$PWD/../3rdparty/qhttpserver/src/qhttprequest.h \
    $$PWD/../3rdparty/qhttpserver/src/qhttpresponse.h \
    $$PWD/../3rdparty/qhttpserver/src/qhttpserver.h \
    $$PWD/../3rdparty/qhttpserver/src/qhttpserverapi.h \
    $$PWD/../3rdparty/qhttpserver/src/qhttpserverfwd.h

SOURCES += \
    $$PWD/../3rdparty/qhttpserver/src/qhttpconnection.cpp \
    $$PWD/../3rdparty/qhttpserver/src/qhttprequest.cpp \
    $$PWD/../3rdparty/qhttpserver/src/qhttpresponse.cpp \
    $$PWD/../3rdparty/qhttpserver/src/qhttpserver.cpp

INCLUDEPATH += 3rdparty/qhttpserver/src/

message("  --> Adding http parser")
HEADERS += $$PWD/../3rdparty/qhttpserver/http-parser/http_parser.h
SOURCES += $$PWD/../3rdparty/qhttpserver/http-parser/http_parser.c
INCLUDEPATH += $$PWD/../3rdparty/qhttpserver/http-parser/


