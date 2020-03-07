find_package(Qt5 COMPONENTS LinguistTools)
set(TRANSLATIONS_DIR ${CMAKE_SOURCE_DIR}/translations)
file(GLOB TRANSLATIONS_TS ${TRANSLATIONS_DIR}/**.ts)
qt5_create_translation(QM_FILES ${TRANSLATIONS_TS})
