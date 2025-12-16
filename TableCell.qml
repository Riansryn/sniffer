import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    property alias text: label.text
    height: parent.height
    color: "transparent"
    
    Label {
        id: label
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        elide: Text.ElideRight
        width: parent.width - 10
        font.pointSize: 9
    }
}
