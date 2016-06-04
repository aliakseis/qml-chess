import QtQuick 2.1
import QtQuick.Window 2.1

import "controller.js" as Controller

Window {
    visible: true
    width: 360
    height: 480

    Component.onCompleted: Controller.initContext(applicationContext)

    GameCanvas {
        id: canvas
        width: 320
        height: 320
        tileSize: 320/8
        anchors.centerIn: parent
        z: 1
    }

    Row {
        height: 32
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 8
        spacing: 8

        Button {
            height: parent.height
            caption: qsTr("Start")
            onClicked: Controller.start();
            visible: !canvas.playingState
        }

        Button {
            height: parent.height
            caption: qsTr("Load")
            onClicked: Controller.load();
            visible: !canvas.playingState
        }

        Button {
            height: parent.height
            caption: qsTr("Stop")
            onClicked: Controller.stop();
            visible: canvas.playingState
        }

        Button {
            height: parent.height
            caption: qsTr("Save")
            onClicked: Controller.save();
            visible: canvas.playingState
        }


        Button {
            height: parent.height
            caption: qsTr("Prev")
            onClicked: Controller.prev();
            visible: canvas.historyState
            enabled: canvas.hasPrevMove
        }

        Button {
            height: parent.height
            caption: qsTr("Next")
            onClicked: Controller.next();
            visible: canvas.historyState
            enabled: canvas.hasNextMove
        }
    }

}
