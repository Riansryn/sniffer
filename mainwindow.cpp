#include "mainwindow.h"
#include <QNetworkInterface>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

MainWindow::MainWindow(QObject *parent) :
    QObject(parent),
    sniffer(new PacketSniffer(this)),
    packetModel(new PacketModel(this)),
    m_statusMessage("Ready")
{
    connect(sniffer, &PacketSniffer::packetReceived, this, &MainWindow::onPacketReceived);
    
    loadNetworkInterfaces();
    updateStatus("Ready");
}

MainWindow::~MainWindow()
{
    if (sniffer->isCapturing()) {
        sniffer->stopCapture();
    }
}

bool MainWindow::isCapturing() const
{
    return sniffer->isCapturing();
}

void MainWindow::loadNetworkInterfaces()
{
    m_networkInterfaces.clear();
    
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    if (interfaces.isEmpty()) {
        QVariantMap item;
        item["display"] = "No interfaces found";
        item["value"] = "";
        m_networkInterfaces.append(item);
        emit networkInterfacesChanged();
        return;
    }
    
#ifdef LINUX_PLATFORM
    QVariantMap anyItem;
    anyItem["display"] = "any";
    anyItem["value"] = "any";
    m_networkInterfaces.append(anyItem);
#endif
    
    for (const QNetworkInterface &iface : interfaces) {
        if (iface.flags().testFlag(QNetworkInterface::IsUp) &&
            !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            
            QString displayName = iface.name();
            
            QList<QNetworkAddressEntry> addresses = iface.addressEntries();
            if (!addresses.isEmpty()) {
                displayName += QString(" (%1)").arg(addresses.first().ip().toString());
            }
            
            QVariantMap item;
            item["display"] = displayName;
            item["value"] = iface.name();
            m_networkInterfaces.append(item);
        }
    }
    
    for (const QNetworkInterface &iface : interfaces) {
        if (iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            QString displayName = iface.name() + " (loopback)";
            
            QVariantMap item;
            item["display"] = displayName;
            item["value"] = iface.name();
            m_networkInterfaces.append(item);
        }
    }
    
    if (m_networkInterfaces.isEmpty()) {
        QVariantMap item;
        item["display"] = "No active interfaces";
        item["value"] = "";
        m_networkInterfaces.append(item);
    }
    
    emit networkInterfacesChanged();
}

void MainWindow::startCapture(const QString &interface)
{
    if (interface.isEmpty() || interface.contains("No")) {
        updateStatus("Error: Please select a valid network interface");
        return;
    }
    
    if (sniffer->startCapture(interface)) {
        emit capturingChanged();
        updateStatus("Capturing packets on " + interface);
    } else {
        updateStatus("Error: Failed to start capture. Check permissions.");
    }
}

void MainWindow::stopCapture()
{
    sniffer->stopCapture();
    emit capturingChanged();
    updateStatus("Capture stopped");
}

void MainWindow::clearPackets()
{
    packetModel->clear();
    m_packetDetails.clear();
    emit packetDetailsChanged();
    updateStatus("Packets cleared");
}

void MainWindow::onPacketReceived(const PacketInfo &packet)
{
    packetModel->addPacket(packet);
}

void MainWindow::selectPacket(int index)
{
    if (index < 0 || index >= packetModel->rowCount()) {
        return;
    }
    
    PacketInfo packet = packetModel->getPacket(index);
    
    m_packetDetails.clear();
    m_packetDetails += "Packet Details:\n";
    m_packetDetails += "================\n\n";
    m_packetDetails += QString("No: %1\n").arg(packet.number);
    m_packetDetails += QString("Time: %1\n").arg(packet.timestamp);
    m_packetDetails += QString("Source: %1:%2\n").arg(packet.source).arg(packet.srcPort);
    m_packetDetails += QString("Destination: %1:%2\n").arg(packet.destination).arg(packet.dstPort);
    m_packetDetails += QString("Protocol: %1\n").arg(packet.protocol);
    m_packetDetails += QString("Length: %1 bytes\n").arg(packet.length);
    m_packetDetails += QString("\nInfo: %1\n").arg(packet.info);
    m_packetDetails += QString("\nRaw Data (hex):\n%1\n").arg(packet.data);
    
    emit packetDetailsChanged();
}

void MainWindow::updateStatus(const QString &message)
{
    m_statusMessage = message;
    emit statusMessageChanged();
}

void MainWindow::saveToPcap()
{
    QList<PacketInfo> packets = packetModel->getAllPackets();
    
    if (packets.isEmpty()) {
        updateStatus("Warning: No packets to save");
        return;
    }
    
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/capture.pcap";
    QString filename = QFileDialog::getSaveFileName(
        nullptr,
        "Save Capture File",
        defaultPath,
        "PCAP Files (*.pcap);;All Files (*)"
    );
    
    if (filename.isEmpty()) {
        return;
    }
    
    if (writePcapFile(filename, packets)) {
        updateStatus(QString("Success: Saved %1 packets to %2")
            .arg(packets.size())
            .arg(QFileInfo(filename).fileName()));
    } else {
        updateStatus("Error: Failed to save capture file");
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
