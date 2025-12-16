#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QScrollBar>
#include <QNetworkInterface>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    sniffer(new PacketSniffer(this)),
    packetModel(new PacketModel(this))
{
    ui->setupUi(this);
    setupUI();
    
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartCapture);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopCapture);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::onClearPackets);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::onSaveToPcap);
    
    connect(sniffer, &PacketSniffer::packetReceived, this, &MainWindow::onPacketReceived);
    connect(ui->packetTableView, &QTableView::clicked, this, &MainWindow::onPacketSelected);
    
    updateStatus("Ready");
}

MainWindow::~MainWindow()
{
    if (sniffer->isCapturing()) {
        sniffer->stopCapture();
    }
    delete ui;
}

void MainWindow::setupUI()
{
    ui->packetTableView->setModel(packetModel);
    ui->packetTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->packetTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->packetTableView->horizontalHeader()->setStretchLastSection(true);
    
    ui->stopButton->setEnabled(false);
    
    loadNetworkInterfaces();
}

void MainWindow::loadNetworkInterfaces()
{
    ui->interfaceComboBox->clear();
    
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    if (interfaces.isEmpty()) {
        ui->interfaceComboBox->addItem("No interfaces found");
        ui->startButton->setEnabled(false);
        return;
    }
    
#ifdef LINUX_PLATFORM
    ui->interfaceComboBox->addItem("any");
#endif
    
    for (const QNetworkInterface &iface : interfaces) {
        if (iface.flags().testFlag(QNetworkInterface::IsUp) &&
            !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            
            QString displayName = iface.name();
            
            QList<QNetworkAddressEntry> addresses = iface.addressEntries();
            if (!addresses.isEmpty()) {
                displayName += QString(" (%1)").arg(addresses.first().ip().toString());
            }
            
            ui->interfaceComboBox->addItem(displayName, iface.name());
        }
    }
    
    for (const QNetworkInterface &iface : interfaces) {
        if (iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            QString displayName = iface.name() + " (loopback)";
            ui->interfaceComboBox->addItem(displayName, iface.name());
        }
    }
    
    if (ui->interfaceComboBox->count() == 0) {
        ui->interfaceComboBox->addItem("No active interfaces");
        ui->startButton->setEnabled(false);
    }
}

void MainWindow::onStartCapture()
{
    QString interface = ui->interfaceComboBox->currentData().toString();
    
    if (interface.isEmpty()) {
        interface = ui->interfaceComboBox->currentText();
    }
    
    if (interface.isEmpty() || interface.contains("No")) {
        QMessageBox::warning(this, "Warning", "Please select a valid network interface");
        return;
    }
    
    if (sniffer->startCapture(interface)) {
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
        ui->interfaceComboBox->setEnabled(false);
        updateStatus("Capturing packets on " + interface);
    } else {
        QMessageBox::critical(this, "Error", 
            "Failed to start capture. Make sure you have the required permissions.");
    }
}

void MainWindow::onStopCapture()
{
    sniffer->stopCapture();
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->interfaceComboBox->setEnabled(true);
    updateStatus("Capture stopped");
}

void MainWindow::onClearPackets()
{
    packetModel->clear();
    ui->detailsTextEdit->clear();
    updateStatus("Packets cleared");
}

void MainWindow::onPacketReceived(const PacketInfo &packet)
{
    packetModel->addPacket(packet);
    
    // Auto-scroll to bottom
    ui->packetTableView->scrollToBottom();
}

void MainWindow::onPacketSelected(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    
    PacketInfo packet = packetModel->getPacket(index.row());
    
    QString details;
    details += "Packet Details:\n";
    details += "================\n\n";
    details += QString("No: %1\n").arg(packet.number);
    details += QString("Time: %1\n").arg(packet.timestamp);
    details += QString("Source: %1:%2\n").arg(packet.source).arg(packet.srcPort);
    details += QString("Destination: %1:%2\n").arg(packet.destination).arg(packet.dstPort);
    details += QString("Protocol: %1\n").arg(packet.protocol);
    details += QString("Length: %1 bytes\n").arg(packet.length);
    details += QString("\nInfo: %1\n").arg(packet.info);
    details += QString("\nRaw Data (hex):\n%1\n").arg(packet.data);
    
    ui->detailsTextEdit->setPlainText(details);
}

void MainWindow::updateStatus(const QString &message)
{
    ui->statusBar->showMessage(message);
}

void MainWindow::onSaveToPcap()
{
    QList<PacketInfo> packets = packetModel->getAllPackets();
    
    if (packets.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No packets to save");
        return;
    }
    
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Save Capture File",
        QDir::homePath() + "/capture.pcap",
        "PCAP Files (*.pcap);;All Files (*)"
    );
    
    if (filename.isEmpty()) {
        return;
    }
    
    if (writePcapFile(filename, packets)) {
        QMessageBox::information(this, "Success", 
            QString("Successfully saved %1 packets to %2")
                .arg(packets.size())
                .arg(QFileInfo(filename).fileName()));
        updateStatus(QString("Saved %1 packets to file").arg(packets.size()));
    } else {
        QMessageBox::critical(this, "Error", "Failed to save capture file");
        updateStatus("Failed to save capture file");
    }
}

bool MainWindow::writePcapFile(const QString &filename, const QList<PacketInfo> &packets)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    
    // PCAP Global Header
    out << quint32(0xa1b2c3d4);  // Magic number
    out << quint16(2);            // Major version
    out << quint16(4);            // Minor version
    out << qint32(0);             // Timezone offset (GMT)
    out << quint32(0);            // Timestamp accuracy
    out << quint32(65535);        // Snapshot length
    out << quint32(1);            // Link-layer type (Ethernet)
    
    // Write packet records
    for (const PacketInfo &packet : packets) {
        if (packet.rawData.isEmpty()) {
            continue;
        }
        
        quint32 ts_sec = packet.timestampMicros / 1000000;
        quint32 ts_usec = packet.timestampMicros % 1000000;
        quint32 incl_len = packet.rawData.size();
        quint32 orig_len = packet.length;
        
        // PCAP Packet Header
        out << ts_sec;      // Timestamp seconds
        out << ts_usec;     // Timestamp microseconds
        out << incl_len;    // Number of bytes captured
        out << orig_len;    // Actual length of packet
        
        // Packet data
        out.writeRawData(packet.rawData.constData(), packet.rawData.size());
    }
    
    file.close();
    return true;
}
