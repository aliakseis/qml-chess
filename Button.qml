import QtQuick 2.0

Rectangle {
    id: container
    property alias caption: txt.text
    signal clicked
    height: 64
    width: txt.width + 24
    radius: 8

    gradient: Gradient {
        GradientStop { position: 0.0; color: ma.pressed ? "#90652C" : "#EFD0A8" }
        GradientStop { position: 1.0; color: "burlywood" }
    }

    Text {
        id: txt
        anchors.centerIn: parent

        style: Text.Raised
        styleColor: "#999BCD"
        color: enabled? "#67689B" : "#DFC098"
    }
    MouseArea{
        id: ma
        anchors.fill: parent
        onClicked: container.clicked()
    }
}
