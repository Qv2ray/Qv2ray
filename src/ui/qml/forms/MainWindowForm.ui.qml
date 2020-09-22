import QtQuick 2.13
import QtQuick.Controls 2.13

import "components"
import QtQuick.Layouts 1.11
import "widgets"

Item {
    id: element
    anchors.fill: parent
    Frame {
        id: frame
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        Row {
            id: row
            x: 0
            y: 24
            width: 200
            height: 400

            IconButton {
                id: iconButton
            }

            Text {
                id: element1
                text: qsTr("Qv2ray")
                font.pixelSize: frame.width / 15
            }
        }
    }

    ScrollView {
        anchors.fill: parent
        font.kerning: false
        id: rowLayout
        ConnectionItem {
            id: connectionItem
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        ConnectionItem {
            id: connectionItem1
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        ConnectionItem {
            id: connectionItem2
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:0.75;height:480;width:640}
}
##^##*/

