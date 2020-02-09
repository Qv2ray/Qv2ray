message(" ")

# For Linux and macOS
message("Configuring for macOS specific environment")
LIBS += -framework Carbon -framework Cocoa

use_grpc: error("The use of gRPC backend is not supported on macOS platform.")

message("  --> Linking libqvb static library and Security framework, for macOS platform.")
LIBS += -L$$PWD/../libs/ -lqvb-darwin
LIBS += -framework Security
