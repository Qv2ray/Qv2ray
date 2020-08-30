import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.0

Item {
    id: element
    anchors.fill: parent

    ColumnLayout {
        id: rowLayout
        anchors.fill: parent

        Button {
            id: button
            text: "Hello"
            Layout.fillHeight: false
            Layout.fillWidth: true
            property string yay: "value"
        }
        ListView {
            id: listView
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillHeight: true
            Layout.fillWidth: true
            Text {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                text: qsTr("text")
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:0.75;height:480;width:640}
}
##^##*/

