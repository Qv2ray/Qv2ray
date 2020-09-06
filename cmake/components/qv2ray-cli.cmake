set(QV2RAY_CLI_BASEDIR ${CMAKE_SOURCE_DIR}/src/ui/cli)
add_definitions(-DQAPPLICATION_CLASS=QCoreApplication)

set(QV2RAY_CLI_SOURCES
    ${QV2RAY_CLI_BASEDIR}/Qv2rayCliApplication.hpp
    ${QV2RAY_CLI_BASEDIR}/Qv2rayCliApplication.cpp)
