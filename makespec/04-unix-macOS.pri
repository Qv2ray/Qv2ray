message(" ")

# For Linux and macOS
message("Configuring for macOS specific environment")
LIBS += -framework Carbon -framework Cocoa

use_libqvb {
    message("  --> Linking libqvb static library and Security framework, for macOS platform.")
    LIBS += -L$$PWD/../libs/ -lqvb-darwin
    LIBS += -framework Security
} else {
    message("  --> Linking libgpr and libupb.")
    LIBS += -lgpr -lupb
}
