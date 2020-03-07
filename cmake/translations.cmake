find_package(Qt5 COMPONENTS LinguistTools)
set(TRANSLATIONS_DIR ${CMAKE_SOURCE_DIR}/translations)
qt5_create_translation(QM_FILES ${TRANSLATIONS_DIR}/en_US.ts ${TRANSLATIONS_DIR}/ja_JP.ts ${TRANSLATIONS_DIR}/ru_RU.ts ${TRANSLATIONS_DIR}/zh_CN.ts)
