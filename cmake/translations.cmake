find_package(Qt5 COMPONENTS LinguistTools)
set(TRANSLATIONS_DIR ${CMAKE_SOURCE_DIR}/translations)
qt5_create_translation(QM_FILES ${TRANSLATIONS_DIR} en_US.ts ja_JP.ts ru_RU.ts zh_CN.ts)