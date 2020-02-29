message(" ")

CONFIG += use_grpc

# For Linux and macOS
message("Configuring for macOS specific environment")
LIBS += -framework Carbon -framework Cocoa

use_grpc {
    message("  --> Linking libgpr and libupb.")
    LIBS += -lgpr -lupb
} else {
    message("  --> Linking libqvb static library and Security framework, for macOS platform.")
    LIBS += -L$$PWD/../libs/ -lqvb-darwin
    LIBS += -framework Security
}
