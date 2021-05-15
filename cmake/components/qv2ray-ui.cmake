set(QV2RAY_UI_COMMON_BASEDIR ${CMAKE_SOURCE_DIR}/src/ui/common)
add_definitions(-DQAPPLICATION_CLASS=QApplication)

set(QV2RAY_UI_COMMON_SOURCES
    # Common Utils
    ${QV2RAY_UI_COMMON_BASEDIR}/QRCodeHelper.cpp
    ${QV2RAY_UI_COMMON_BASEDIR}/QRCodeHelper.hpp
    # Platform Auto Launch
    ${QV2RAY_UI_COMMON_BASEDIR}/autolaunch/QvAutoLaunch.hpp
    ${QV2RAY_UI_COMMON_BASEDIR}/autolaunch/QvAutoLaunch.cpp
    # Darkmode Detector
    ${QV2RAY_UI_COMMON_BASEDIR}/darkmode/DarkmodeDetector.cpp
    ${QV2RAY_UI_COMMON_BASEDIR}/darkmode/DarkmodeDetector.hpp
    # Speed Graph
    ${QV2RAY_UI_COMMON_BASEDIR}/speedchart/speedwidget.cpp
    ${QV2RAY_UI_COMMON_BASEDIR}/speedchart/speedwidget.hpp
    # Log Highlighter
    ${QV2RAY_UI_COMMON_BASEDIR}/LogHighlighter.hpp
    ${QV2RAY_UI_COMMON_BASEDIR}/LogHighlighter.cpp
    # GUI Plugins
    ${QV2RAY_UI_COMMON_BASEDIR}/GuiPluginHost.cpp
    ${QV2RAY_UI_COMMON_BASEDIR}/GuiPluginHost.hpp
    # Message bus
    ${QV2RAY_UI_COMMON_BASEDIR}/QvMessageBus.hpp
    ${QV2RAY_UI_COMMON_BASEDIR}/QvMessageBus.cpp
    )
