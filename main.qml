import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1000
    height: 700
    title: "Network Sniffer"
    
    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            Action {
                text: qsTr("E&xit")
                onTriggered: Qt.quit()
            }
        }
        Menu {
            title: qsTr("&Help")
            Action {
                text: qsTr("&About")
                onTriggered: aboutDialog.open()
            }
        }
    }
    
    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 10
            
            Label {
                text: "Interface:"
                Layout.leftMargin: 10
            }
            
            ComboBox {
                id: interfaceComboBox
                Layout.preferredWidth: 200
                model: controller ? controller.networkInterfaces : []
                textRole: "display"
                valueRole: "value"
                ToolTip.visible: hovered
                ToolTip.text: "Select network interface to capture packets"
            }
            
            Button {
                id: startButton
                text: "Start"
                icon.name: "media-playback-start"
                onClicked: {
                    if (controller) {
                        controller.startCapture(interfaceComboBox.currentValue)
                    }
                }
            }
            
            Button {
                id: stopButton
                text: "Stop"
                icon.name: "media-playback-stop"
                enabled: false
                onClicked: {
                    if (controller) {
                        controller.stopCapture()
                    }
                }
            }
            
            Button {
                text: "Clear"
                icon.name: "edit-clear"
                onClicked: {
                    if (controller) {
                        controller.clearPackets()
                    }
                }
            }
            
            Button {
                text: "Save to PCAP"
                icon.name: "document-save"
                ToolTip.visible: hovered
                ToolTip.text: "Save captured packets to PCAP file"
                onClicked: {
                    if (controller) {
                        controller.saveToPcap()
                    }
                }
            }
            
            Item {
                Layout.fillWidth: true
            }
        }
    }
    
    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical
        
        // Packet list table
        Item {
            SplitView.fillHeight: true
            SplitView.minimumHeight: 200
            
            Rectangle {
                anchors.fill: parent
                color: palette.base
                border.color: palette.mid
                border.width: 1
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 1
                    spacing: 0
                    
                    // Header
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        color: palette.button
                        
                        Row {
                            anchors.fill: parent
                            
                            HeaderCell { text: "No."; width: 60 }
                            HeaderCell { text: "Time (s)"; width: 120 }
                            HeaderCell { text: "Interface"; width: 100 }
                            HeaderCell { text: "Source"; width: 150 }
                            HeaderCell { text: "Destination"; width: 150 }
                            HeaderCell { text: "Protocol"; width: 80 }
                            HeaderCell { text: "Length"; width: 80 }
                            HeaderCell { text: "Info"; width: parent.width - 740 }
                        }
                    }
                    
                    // Table view
                    ListView {
                        id: packetListView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        
                        model: controller ? controller.packetModel : null
                        
                        delegate: Rectangle {
                            width: packetListView.width
                            height: 25
                            color: index % 2 === 0 ? palette.base : palette.alternateBase
                            
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    packetListView.currentIndex = index
                                    if (controller) {
                                        controller.selectPacket(index)
                                    }
                                }
                            }
                            
                            Row {
                                anchors.fill: parent
                                
                                TableCell { text: model.number; width: 60 }
                                TableCell { text: model.timestampSecs ? model.timestampSecs.toFixed(6) : ""; width: 120 }
                                TableCell { text: model.interfaceName; width: 100 }
                                TableCell { text: model.source + ":" + model.srcPort; width: 150 }
                                TableCell { text: model.destination + ":" + model.dstPort; width: 150 }
                                TableCell { text: model.protocol; width: 80 }
                                TableCell { text: model.length; width: 80 }
                                TableCell { text: model.info; width: parent.width - 740 }
                            }
                            
                            Rectangle {
                                anchors.fill: parent
                                color: "lightblue"
                                opacity: 0.3
                                visible: packetListView.currentIndex === index
                            }
                        }
                        
                        ScrollBar.vertical: ScrollBar { }
                    }
                }
            }
        }
        
        // Packet details
        Item {
            SplitView.preferredHeight: 250
            SplitView.minimumHeight: 100
            
            Rectangle {
                anchors.fill: parent
                color: palette.base
                border.color: palette.mid
                border.width: 1
                
                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 5
                    
                    TextArea {
                        id: detailsTextArea
                        readOnly: true
                        selectByMouse: true
                        wrapMode: TextArea.NoWrap
                        font.family: "Monospace"
                        font.pointSize: 9
                        placeholderText: "Select a packet to view details..."
                        text: controller ? controller.packetDetails : ""
                    }
                }
            }
        }
    }
    
    footer: ToolBar {
        Label {
            id: statusLabel
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            text: controller ? controller.statusMessage : "Ready"
        }
    }
    
    Connections {
        target: controller
        function onCapturingChanged() {
            startButton.enabled = !controller.capturing
            stopButton.enabled = controller.capturing
            interfaceComboBox.enabled = !controller.capturing
        }
    }
    
    Dialog {
        id: aboutDialog
        title: "About Network Sniffer"
        modal: true
        anchors.centerIn: parent
        
        standardButtons: Dialog.Ok
        
        ColumnLayout {
            spacing: 10
            
            Label {
                text: "Network Sniffer v1.0"
                font.bold: true
                font.pointSize: 14
            }
            
            Label {
                text: "A Qt Quick network packet capture and analysis tool"
            }
            
            Label {
                text: "Features:"
                font.bold: true
            }
            
            Label {
                text: "• Real-time packet capture\n• Protocol identification\n• PCAP file export\n• Cross-platform support"
            }
            
            Label {
                text: "\n© 2024 - Educational/Personal Use"
                font.italic: true
            }
        }
    }
}