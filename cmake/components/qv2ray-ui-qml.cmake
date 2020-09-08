set(QV2RAY_QML_BASEDIR ${CMAKE_SOURCE_DIR}/src/ui/qml)
set(QV2RAY_QML_SOURCES
    ${QV2RAY_QML_BASEDIR}/qmlresx.qrc
    ${QV2RAY_QML_BASEDIR}/Qv2rayQMLApplication.hpp
    ${QV2RAY_QML_BASEDIR}/Qv2rayQMLApplication.cpp
    ${QV2RAY_QML_BASEDIR}/Qv2rayQMLProperty.cpp
    ${QV2RAY_QML_BASEDIR}/Qv2rayQMLProperty.hpp
    )

if(QV2RAY_QML_LIVE_UPDATE)
    add_definitions(-DQV2RAY_QMLLIVE_DEBUG=1)
    find_library(QMLLIVE_LIBS qmllive)
    link_libraries(-lqmllive)
endif()
