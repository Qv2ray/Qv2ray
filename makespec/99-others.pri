message(" ")
with_metainfo {
    message("  --> Generating metainfo dependency.")
    appdataXml.files += ./assets/qv2ray.metainfo.xml
    appdataXml.path = $$PREFIX/share/metainfo/
    INSTALLS += appdataXml
    DEFINES += WITH_FLATHUB_CONFIG_PATH
}

# Automatically increase build number.
no_increase_build_number | qmake_lupdate {
    message("QV2RAY_BUILD_VERSION will not be increased")
} else {
    QV2RAY_BUILD_VERSION = $$num_add($$QV2RAY_BUILD_VERSION, 1)
    write_file($$PWD/BUILDVERSION, QV2RAY_BUILD_VERSION)
}

qmake_lupdate {
    message(" ")
    message("Running lupdate...")
    message(" ")
    lupdate_output = $$system(lupdate $$SOURCES $$HEADERS $$FORMS -ts $$PWD/$$TRANSLATIONS -no-ui-lines)
    message(" $$lupdate_output")
    message("lupdate finished.")
}

# Qv2ray manual build info
!no_build_info {
    _QV2RAY_BUILD_INFO_STR_=$$getenv(_QV2RAY_BUILD_INFO_)
    _QV2RAY_BUILD_EXTRA_INFO_STR_=$$getenv(_QV2RAY_BUILD_EXTRA_INFO_)

    isEmpty(_QV2RAY_BUILD_INFO_STR_) {
        _QV2RAY_BUILD_INFO_STR_ = "Qv2ray from manual build"
    }

    isEmpty(_QV2RAY_BUILD_EXTRA_INFO_STR_) {
        _QV2RAY_BUILD_EXTRA_INFO_STR_ = "Qv2ray $$VERSION"
    }
} else {
    _QV2RAY_BUILD_INFO_STR_ = "No Info"
    _QV2RAY_BUILD_EXTRA_INFO_STR_ = "No Extra Info"
}

message("Qv2ray build info:")
message("  --> $$_QV2RAY_BUILD_INFO_STR_")
message("  --> $$_QV2RAY_BUILD_EXTRA_INFO_STR_")

DEFINES += _QV2RAY_BUILD_INFO_STR_=\"\\\"$${_QV2RAY_BUILD_INFO_STR_}\\\"\" _QV2RAY_BUILD_EXTRA_INFO_STR_=\"\\\"$${_QV2RAY_BUILD_EXTRA_INFO_STR_}\\\"\"
