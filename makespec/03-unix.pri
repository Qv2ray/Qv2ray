message(" ")
win32: Qv2rayQMakeError("Do not include this file in Windows platform.")

# For Linux and macOS
message("Configuring for unix-like environment")

message("  --> Setting up QMAKE_CXXFLAGS")
QMAKE_CXXFLAGS += -Wno-missing-field-initializers -Wno-unused-parameter -Wno-unused-variable

# macOS homebrew include path
message("  --> Adding local include folder to search path")
INCLUDEPATH += /usr/local/include/

# For protobuf in linux and macOS
message("  --> Linking against protobuf library.")
LIBS += -L/usr/local/lib -lprotobuf

!no_generate_headers {
    message("  --> Generating geosite headers for Unix")
    system("$$PWD/../tools/unix-generate-geosite.sh $$PWD")
}

use_grpc {
    no_generate_headers {
        message("  --> Skipped generation of protobuf and/or gRPC header files")
    } else {
        message("  --> Generating gRPC and protobuf headers for Unix")
        system("$$PWD/../tools/unix-generate-api.sh $$PWD")
    }
}
