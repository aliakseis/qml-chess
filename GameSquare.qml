import QtQuick 2.0

Rectangle {
    property QtObject canvas: null
    property int tileSize: canvas.tileSize
    property int col: 0
    property int row: 0
    width: tileSize
    height: tileSize
    x: tileSize * col
    y: tileSize * row
    color: (col + row) % 2 ? "#90652C" : "#DEB887"
}
