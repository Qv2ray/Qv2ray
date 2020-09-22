import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "../widgets"

Item {
    property bool isConnected: false
    property string connectionId: "null"
    property string connectionName: "连接名称"
    property string connectionType: "Shadowsocks + mKCP"

    property alias editButton: _editBtn
    property alias editJsonButton: _editJsonBtn
    property alias deleteButton: _deleteBtn
    property alias toggleConnectivityBtn: _toggleConnectivityBtn

    Rectangle {
        id: base
        anchors.fill: parent
        radius: 16
        color: "#fcfcfc"
        border.width: 2
        border.color: "#bdbdbd"
    }

    Rectangle {
        id: connectedIndicator
        readonly property int xsize: parent.width / 5
        readonly property int ysize: parent.width / 5
        readonly property Gradient connectedGradient: Gradient {
            GradientStop {
                position: 0.00
                color: "#00ff00"
            }
            GradientStop {
                position: 1.00
                color: "#00cf00"
            }
        }
        readonly property Gradient disconnectedGradient: Gradient {
            GradientStop {
                position: 0.00
                color: "#ff0000"
            }
            GradientStop {
                position: 1.00
                color: "#cf0000"
            }
        }
        x: xsize / 4
        y: ysize / 4
        width: xsize / 4
        height: ysize / 4
        radius: xsize
        gradient: isConnected ? connectedGradient : disconnectedGradient
        rotation: 325
        border.width: 1
        border.color: "#000"
    }

    ColumnLayout {
        anchors.fill: base
        anchors.leftMargin: connectedIndicator.x + 2 * connectedIndicator.width
        anchors.topMargin: connectedIndicator.ysize / 4 - 2
        anchors.rightMargin: 0.05 * base.width
        anchors.bottomMargin: actionButtonGroup.height + 12

        Text {
            Layout.fillWidth: true
            text: connectionName
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.pixelSize: base.width / 18
            elide: Text.ElideRight
        }
        Text {
            Layout.fillWidth: true
            text: connectionType
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.pixelSize: base.width / 30
            font.capitalization: Font.AllUppercase
            elide: Text.ElideRight
        }
        Item {
            Layout.fillHeight: true
        }
    }

    RowLayout {
        id: actionButtonGroup
        anchors.bottom: base.bottom
        anchors.right: base.right
        anchors.rightMargin: base.width * 0.1
        anchors.bottomMargin: base.height * 0.05
        IconButton {
            id: _deleteBtn
            visible: !isConnected
            imageSource: qv2ray.assetsPrefix + "/ashbin.svg"
        }
        IconButton {
            id: _editBtn
            visible: !isConnected
            imageSource: qv2ray.assetsPrefix + "/edit.svg"
        }
        IconButton {
            id: _editJsonBtn
            visible: !isConnected
            imageSource: qv2ray.assetsPrefix + "/code.svg"
        }
        IconButton {
            id: _toggleConnectivityBtn
            imageSource: qv2ray.assetsPrefix + (!isConnected ? "/start.svg" : "/stop.svg")
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
