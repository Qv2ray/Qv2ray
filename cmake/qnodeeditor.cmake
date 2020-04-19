add_definitions(-DNODE_EDITOR_SHARED -DNODE_EDITOR_EXPORTS)
if(QV2RAY_QNODEEDITOR_PROVIDER STREQUAL "module")
    set(QNODEEDITOR_DIR ${CMAKE_SOURCE_DIR}/3rdparty/QNodeEditor)
    set(QNODEEDITOR_SOURCES
        ${QNODEEDITOR_DIR}/src/Connection.cpp
        ${QNODEEDITOR_DIR}/src/ConnectionBlurEffect.cpp
        ${QNODEEDITOR_DIR}/src/ConnectionGeometry.cpp
        ${QNODEEDITOR_DIR}/src/ConnectionGraphicsObject.cpp
        ${QNODEEDITOR_DIR}/src/ConnectionPainter.cpp
        ${QNODEEDITOR_DIR}/src/ConnectionState.cpp
        ${QNODEEDITOR_DIR}/src/ConnectionStyle.cpp
        ${QNODEEDITOR_DIR}/src/DataModelRegistry.cpp
        ${QNODEEDITOR_DIR}/src/FlowScene.cpp
        ${QNODEEDITOR_DIR}/src/FlowView.cpp
        ${QNODEEDITOR_DIR}/src/FlowViewStyle.cpp
        ${QNODEEDITOR_DIR}/src/Node.cpp
        ${QNODEEDITOR_DIR}/src/NodeConnectionInteraction.cpp
        ${QNODEEDITOR_DIR}/src/NodeDataModel.cpp
        ${QNODEEDITOR_DIR}/src/NodeGeometry.cpp
        ${QNODEEDITOR_DIR}/src/NodeGraphicsObject.cpp
        ${QNODEEDITOR_DIR}/src/NodePainter.cpp
        ${QNODEEDITOR_DIR}/src/NodeState.cpp
        ${QNODEEDITOR_DIR}/src/NodeStyle.cpp
        ${QNODEEDITOR_DIR}/src/Properties.cpp
        ${QNODEEDITOR_DIR}/src/StyleCollection.cpp
    )
    set(QNODEEDITOR_INCLUDE_PATH
        ${QNODEEDITOR_DIR}/include/nodes/internal
    )

    set(QNODEEDITOR_QRC_RESOURCES ${QNODEEDITOR_DIR}/resources/resources.qrc)
elseif(QV2RAY_QNODEEDITOR_PROVIDER STREQUAL "package")
    find_package(NodeEditor REQUIRED CONFIG)
    set(QNODEEDITOR_LIBRARY NodeEditor::nodes)
    find_path(QNODEEDITOR_INCLUDE_PATH Node.hpp PATH_SUFFIXES nodes/internal)
endif()