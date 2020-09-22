import QtQuick 2.4
import QtQuick.Controls 2.15

RoundButton {
    property alias imageSource: image.source
    id: roundButton
    Image {
        id: image
        anchors.fill: roundButton
        scale: 0.5
        source: imageSource
        fillMode: Image.PreserveAspectFit
    }
}
