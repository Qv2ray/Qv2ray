
include($$PWD/../libudns/libudns.pri)
include($$PWD/../libcork/libcork.pri)
include($$PWD/../libev/libev.pri)
include($$PWD/../libipset/libipset.pri)
INCLUDEPATH += $$PWD
LIBS += -lsodium
HEADERS +=$$PWD/common.h\
            $$PWD/protocol.h\
            $$PWD/obfs/obfs.h\
    $$PWD/obfs/auth_chain.h\
    $$PWD/obfs/tls1.2_ticket.h\
    $$PWD/obfs/crc32.h\
    $$PWD/obfs/obfsutil.h\
    $$PWD/obfs/verify.h\
    $$PWD/obfs/auth.h\
    $$PWD/obfs/base64.h\
    $$PWD/obfs/obfs.h\
    $$PWD/obfs/http_simple.h\
            $$PWD/cache.h\
            $$PWD/local.h\
    $$PWD/qt_ui_log.h \
    $$PWD/speed.h \
            $$PWD/uthash.h\
            $$PWD/rule.h\
            $$PWD/http.h\
            $$PWD/tls.h\
            $$PWD/utils.h\
            $$PWD/jconf.h\
            $$PWD/json.h\
            $$PWD/resolv.h\
            $$PWD/acl.h\
            $$PWD/netutils.h\
            $$PWD/shadowsocks.h\
            $$PWD/encrypt.h\
            $$PWD/udprelay.h\
            $$PWD/socks5.h
               

SOURCES +=$$PWD/http.c\
        $$PWD/local.c \
        $$PWD/qt_ui_log.cpp \
        $$PWD/tls.c\
        $$PWD/rule.c\
        $$PWD/utils.c\
        $$PWD/jconf.c\
        $$PWD/json.c\
        $$PWD/encrypt.c\
        $$PWD/udprelay.c\
        $$PWD/cache.c\
        $$PWD/acl.c\
        $$PWD/netutils.c\
        $$PWD/obfs/obfs.c\
        $$PWD/obfs/crc32.c\
        $$PWD/obfs/auth.c\
        $$PWD/obfs/auth_chain.c\
        $$PWD/obfs/obfsutil.c\
        $$PWD/obfs/base64.c\
        $$PWD/obfs/verify.c\
        $$PWD/obfs/tls1.2_ticket.c\
        $$PWD/obfs/http_simple.c

