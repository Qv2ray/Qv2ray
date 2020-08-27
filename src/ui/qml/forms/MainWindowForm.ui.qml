import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.11

Item {
    id: element
    property alias button: button

    ColumnLayout {
        id: rowLayout
        anchors.fill: parent

        Button {
            id: button
            text: "Hello"
            property string yay: "value"
        }
        Button {
            text: "2"
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:0.75;height:480;width:640}
}
##^##*/

