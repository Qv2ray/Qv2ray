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
        ${QNODEEDITOR_DIR}/include
        ${QNODEEDITOR_DIR}/include/nodes/internal
        )

    set(QNODEEDITOR_HEADERS
        ${QNODEEDITOR_DIR}/include/nodes/internal/Compiler.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/Connection.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/ConnectionGeometry.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/ConnectionGraphicsObject.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/ConnectionState.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/ConnectionStyle.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/DataModelRegistry.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/Export.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/FlowScene.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/FlowView.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/FlowViewStyle.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/memory.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/Node.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/NodeData.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/NodeDataModel.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/NodeGeometry.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/NodeGraphicsObject.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/NodePainterDelegate.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/NodeState.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/NodeStyle.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/OperatingSystem.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/PortType.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/QStringStdHash.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/QUuidStdHash.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/Serializable.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/Style.hpp
        ${QNODEEDITOR_DIR}/include/nodes/internal/TypeConverter.hpp
        )

    #    qt5_wrap_cpp(QNODEEDITOR_SOURCES
    #        ${HEADERS_TO_MOC}
    #        TARGET qv2ray
    #        OPTIONS --no-notes # Don't display a note for the headers which don't produce a moc_*.cpp
    #    )
    set(QNODEEDITOR_LIBRARY qv2ray_nodeeditor)
    add_library(${QNODEEDITOR_LIBRARY} STATIC
        ${QNODEEDITOR_SOURCES}
        ${QNODEEDITOR_HEADERS}
        )
    target_include_directories(${QNODEEDITOR_LIBRARY} PUBLIC
        ${QNODEEDITOR_INCLUDE_PATH}
        )
    target_link_libraries(${QNODEEDITOR_LIBRARY}
        ${QV_QT_LIBNAME}::Core
        ${QV_QT_LIBNAME}::Widgets
        ${QV_QT_LIBNAME}::Gui
        )
    set(QNODEEDITOR_QRC_RESOURCES ${QNODEEDITOR_DIR}/resources/resources.qrc)
elseif(QV2RAY_QNODEEDITOR_PROVIDER STREQUAL "package")
    find_package(NodeEditor REQUIRED CONFIG)
    find_path(QNODEEDITOR_INCLUDE_PATH NAMES Node.hpp PATH_SUFFIXES nodes/internal)
    set(QNODEEDITOR_LIBRARY NodeEditor::nodes)
endif()
