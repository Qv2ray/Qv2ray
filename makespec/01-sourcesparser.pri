message(" ")
defineTest(Qv2rayAddFile) {
    ext = $$take_last(ARGS)
    filename = $${take_first(ARGS)}.$${ext}
    qmake_debug: message("Qv2rayAddFile: filename: $$filename")
    !exists($$filename) {
        error("File: \"$$filename\" is not found, Qv2ray build preparation cannot continue")
    }
    equals(ext, "cpp") {
        SOURCES += $$filename
    } else {
        equals(ext, "hpp") {
            HEADERS += $$filename
        } else {
            equals(ext, "ui") {
                FORMS += $$filename
            } else {
                error("Unknown extension: $${ext}")
            }
        }
    }
    export(SOURCES)
    export(HEADERS)
    export(FORMS)
}

defineTest(Qv2rayAddSource) {
    # Module Compnent Filename extlist
    module = $$take_first(ARGS)
    component = $$take_first(ARGS)
    filename = $$take_first(ARGS)
    extlist = $$ARGS
    FILEPATH = "$$PWD/src/$${module}"
    qmake_debug: message("Qv2rayAddSource: Adding \"$${filename}\" of module \"$${module}\", component \"$${component}\" to the project")
    equals(component, "_") {
        qmake_debug: message("Qv2rayAddSource: Component is empty, ignore")
        FILEPATH += "/$${filename}"
        FILEPATH=$$join(FILEPATH)
    } else {
        FILEPATH += "/$${component}/$${filename}"
        FILEPATH=$$join(FILEPATH)
    }
    qmake_debug: message("Qv2rayAddSource: filepath: $${FILEPATH}, extlist: $${extlist}")
    for(iterate, extlist) {
        Qv2rayAddFile($$FILEPATH, $$iterate)
    }
    export(SOURCES)
    export(HEADERS)
    export(FORMS)
}
