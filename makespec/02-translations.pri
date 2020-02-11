message(" ")
message("Looking for language support.")
QM_FILES_RESOURCE_PREFIX = "translations"
for(var, $$list($$files("$$PWD/../translations/*.ts", true))) {
    LOCALE_FILENAME = $$basename(var)
    message("  --> Found:" $$LOCALE_FILENAME)
    !equals(LOCALE_FILENAME, "en_US.ts") {
        # ONLY USED IN LRELEASE CONTEXT
        # en_US is not EXTRA...
        EXTRA_TRANSLATIONS += $$PWD/../translations/$$LOCALE_FILENAME
    }
}

TRANSLATIONS += $$PWD/../translations/en_US.ts
message("Qv2ray will build with" $${replace(EXTRA_TRANSLATIONS, "$$PWD/../translations/", "")} and $${replace(TRANSLATIONS, "$$PWD/../translations/", "")})

qmake_lupdate {
    message(" ")
    message("Running lupdate...")
    message("TRANSLATIONS: $$TRANSLATIONS")
    lupdate_output = $$system(lupdate -no-ui-lines $$SOURCES $$HEADERS $$FORMS -ts $$TRANSLATIONS)
    message(" $$lupdate_output")
    message("lupdate finished.")
}
