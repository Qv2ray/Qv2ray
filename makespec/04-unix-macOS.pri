message(" ")

# For Linux and macOS
message("Configuring for macOS specific environment")
LIBS += -framework Carbon -framework Cocoa
message("  --> Linking Security framework, for macOS platform.")
LIBS += -framework Security

use_grpc {
    # For gRPC and protobuf in macOS
    message("  --> Linking against gRPC library.")
    LIBS += -L/usr/local/lib -lgrpc++ -lgrpc -lgpr -lupb
} else {
    message("  --> Linking libqvb static library, for macOS platform.")
    LIBS += -L$$PWD/../libs/ -lqvb-darwin
}
