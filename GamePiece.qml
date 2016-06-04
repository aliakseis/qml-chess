import QtQuick 2.0

import "controller.js" as Controller

Item {
    id: container
    property QtObject canvas: null
    property int tileSize: canvas.tileSize
    property int col: 0
    property int row: 0

    property bool white: false

    property int piece: 0

    width: tileSize
    height: tileSize
    x: tileSize * col
    y: tileSize * row

    Text {
        text: String.fromCharCode(9811 + (1 - white) * 6 + piece);
        anchors.centerIn: parent
        font.pointSize : tileSize * 0.6
    }

    MouseArea {
        anchors.fill: parent
        enabled: canvas.playingState
        drag.axis: Drag.XandYAxis
        drag.target: container
        onPressed: Controller.startMove(container);
        onReleased: Controller.finishMove(container);
    }
}
