message(" ")
message("Configuring for Windows environment")
QMAKE_CXXFLAGS += /MP /wo4251
DEFINES += QHTTPSERVER_EXPORT

CONFIG += Qv2ray_Windows use_grpc
!contains(QMAKE_TARGET.arch, x86_64) {
    CONFIG+=Qv2ray_win32
    GRPC_DEPS_PATH=$$PWD/../libs/x86-windows
    #
    !no_generate_headers: message("  --> Generating gRPC and protobuf headers for Windows x86")
    !no_generate_headers: system("$$PWD/../tools/win32-generate-pb.bat")
} else {
    CONFIG+=Qv2ray_win64
    GRPC_DEPS_PATH=$$PWD/../libs/x64-windows
    #
    !no_generate_headers: message("  --> Generating gRPC and protobuf headers for Windows x64")
    !no_generate_headers: system("$$PWD/../tools/win64-generate-pb.bat")
}

message("  --> Applying a hack for protobuf header")
DEFINES += _WIN32_WINNT=0x600

DEPENDPATH  += $$GRPC_DEPS_PATH/include
INCLUDEPATH += $$GRPC_DEPS_PATH/include

message("  --> Setting up target descriptions")
QMAKE_TARGET_DESCRIPTION = "Qv2ray, a cross-platform v2ray GUI client."
QMAKE_TARGET_PRODUCT = "Qv2ray"

Qv2ray_debug: GRPC_LIB_PATH=$$GRPC_DEPS_PATH/debug
Qv2ray_release: GRPC_LIB_PATH=$$GRPC_DEPS_PATH

message("  --> WIN32: Linking against gRPC library: $$GRPC_LIB_PATH")
Qv2ray_debug: LIBS += -L$$GRPC_LIB_PATH/lib/ -laddress_sorting -lcares -lgrpc++_unsecure -lupb -lzlibd -lgrpc_unsecure -lgpr
Qv2ray_release: LIBS += -L$$GRPC_LIB_PATH/lib/ -laddress_sorting -lcares -lgrpc++_unsecure -lupb -lzlib -lgrpc_unsecure -lgpr

message("  --> WIN32: Linking against protobuf library: $$GRPC_LIB_PATH")
Qv2ray_release: LIBS += -lmsvcrt -L$$GRPC_LIB_PATH/lib/ -llibprotobuf
Qv2ray_debug: LIBS += -lmsvcrtd -L$$GRPC_LIB_PATH/lib/ -llibprotobufd

message("  --> Linking against winHTTP and winSock2.")
LIBS += -lwinhttp -lwininet -lws2_32 -luser32
