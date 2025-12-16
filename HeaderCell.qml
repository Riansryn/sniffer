import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    property alias text: label.text
    height: parent.height
    color: "transparent"
    border.color: palette.mid
    border.width: 1
    
    Label {
        id: label
        anchors.centerIn: parent
        font.bold: true
        font.pointSize: 9
    }
}
