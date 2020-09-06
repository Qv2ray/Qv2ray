import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13

ApplicationWindow {
    visible: true
    MainWindowForm {
        anchors.fill: parent
    }
    onClosing: close.accepted = false

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

