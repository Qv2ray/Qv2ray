import QtQuick 2.12

Item {
    readonly property bool isConnected: _internal.isConnected
    readonly property string connectionId: _internal.connectionId
    readonly property string connectionName: _internal.connectionName
    readonly property string connectionType: _internal.connectionType
    ConnectionItemForm {
        anchors.fill: parent
        id: _internal
        editButton.onClicked: {}
        editJsonButton.onClicked: {}
        deleteButton.onClicked: {}
        toggleConnectivityBtn.onClicked: {
            isConnected = !isConnected
        }
    }
}
