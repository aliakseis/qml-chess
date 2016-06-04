import QtQuick 2.0

import "controller.js" as Controller

Item {

    property int tileSize: 80

    property bool playingState: false
    property bool historyState: false

    property bool hasPrevMove: false
    property bool hasNextMove: false

    Component.onCompleted: Controller.initCanvas(canvas)

Rectangle {
    anchors.fill: parent
    anchors.margins: -5
    color: "#A79176"
}

}
