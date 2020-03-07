add_definitions(-DQAPPLICATION_CLASS=QApplication)
set(SINGLEAPPLICATION_DIR ${CMAKE_SOURCE_DIR}/3rdparty/SingleApplication)
set(SINGLEAPPLICATION_SOURCES
    ${SINGLEAPPLICATION_DIR}/singleapplication.cpp
    ${SINGLEAPPLICATION_DIR}/singleapplication_p.cpp
)