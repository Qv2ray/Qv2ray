message(" ")
message("Configuring Qv2ray build for Linux")
isEmpty(PREFIX) {
    PREFIX=/usr/local
}

use_grpc {
    # For gRPC and protobuf in linux and macOS
    message("  --> Linking against gRPC and protobuf library.")
    LIBS += -L/usr/local/lib -lgrpc++ -lgrpc
} else {
    message("  --> Linking libqvb static library, for Linux platform.")
    LIBS += -L$$PWD/../libs/ -lqvb-linux64
}

message("  --> Generating desktop dependency.")
desktop.files += $$PWD/../assets/qv2ray.desktop
desktop.path = $$PREFIX/share/applications/

message("  --> Generating icons dependency.")
icon.files += $$PWD/../assets/icons/qv2ray.png
icon.path = $$PREFIX/share/icons/hicolor/256x256/apps/

target.path = $$PREFIX/bin/
INSTALLS += target desktop icon

