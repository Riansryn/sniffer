#include "packetsniffer.h"
#include <QDateTime>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDebug>

#ifdef LINUX_PLATFORM
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#endif

PacketSniffer::PacketSniffer(QObject *parent)
    : QObject(parent)
    , capturing(false)
    , packetCount(0)
    , udpSocket(nullptr)
#ifdef LINUX_PLATFORM
    , captureThread(nullptr)
#endif
{
    qRegisterMetaType<PacketInfo>("PacketInfo");
}

PacketSniffer::~PacketSniffer()
{
    stopCapture();
}

bool PacketSniffer::startCapture(const QString &interface)
{
    if (capturing) {
        return false;
    }

#ifdef LINUX_PLATFORM
    // Use pcap for Linux
    captureThread = new CaptureThread(interface, this);
    connect(captureThread, &CaptureThread::packetCaptured, 
            this, &PacketSniffer::packetReceived);
    captureThread->start();
    capturing = true;
    packetCount = 0;
    return true;
#else
    // Fallback to simple UDP socket for Android
    udpSocket = new QUdpSocket(this);
    
    if (udpSocket->bind(QHostAddress::Any, 0, QUdpSocket::ShareAddress)) {
        connect(udpSocket, &QUdpSocket::readyRead, 
                this, &PacketSniffer::onSocketReadyRead);
        capturing = true;
        packetCount = 0;
        return true;
    }
    
    delete udpSocket;
    udpSocket = nullptr;
    return false;
#endif
}

void PacketSniffer::stopCapture()
{
    if (!capturing) {
        return;
    }

#ifdef LINUX_PLATFORM
    if (captureThread) {
        captureThread->stop();
        captureThread->wait();
        delete captureThread;
        captureThread = nullptr;
    }
#else
    if (udpSocket) {
        udpSocket->close();
        delete udpSocket;
        udpSocket = nullptr;
    }
#endif

    capturing = false;
}

void PacketSniffer::onSocketReadyRead()
{
    while (udpSocket && udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        parsePacket(datagram.data(), datagram.senderAddress(), datagram.senderPort());
    }
}

void PacketSniffer::parsePacket(const QByteArray &data, const QHostAddress &sender, quint16 port)
{
    PacketInfo packet;
    packet.number = ++packetCount;
    packet.timestamp = formatTimestamp();
    packet.timestampMicros = QDateTime::currentDateTime().toMSecsSinceEpoch() * 1000;
    packet.source = sender.toString();
    packet.srcPort = port;
    packet.destination = "Unknown";
    packet.dstPort = 0;
    packet.protocol = "UDP";
    packet.length = data.size();
    packet.info = QString("UDP packet from %1:%2").arg(sender.toString()).arg(port);
    packet.data = byteArrayToHex(data);
    packet.rawData = data;

    emit packetReceived(packet);
}

QString PacketSniffer::formatTimestamp()
{
    return QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

QString PacketSniffer::byteArrayToHex(const QByteArray &data)
{
    QString hex;
    int maxBytes = qMin(data.size(), 256); // Limit to first 256 bytes
    
    for (int i = 0; i < maxBytes; i += 16) {
        hex += QString("%1: ").arg(i, 4, 16, QChar('0'));
        
        // Hex representation
        for (int j = 0; j < 16 && (i + j) < maxBytes; ++j) {
            hex += QString("%1 ").arg((unsigned char)data[i + j], 2, 16, QChar('0'));
        }
        
        hex += "\n";
    }
    
    if (data.size() > maxBytes) {
        hex += QString("\n... (%1 more bytes)").arg(data.size() - maxBytes);
    }
    
    return hex;
}

#ifdef LINUX_PLATFORM
CaptureThread::CaptureThread(const QString &iface, QObject *parent)
    : QThread(parent)
    , interface(iface)
    , stopRequested(false)
{
}

void CaptureThread::stop()
{
    stopRequested = true;
}

void CaptureThread::run()
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    
    // Open device for sniffing
    handle = pcap_open_live(interface.toStdString().c_str(), 
                           BUFSIZ, 1, 1000, errbuf);
    
    if (handle == nullptr) {
        qWarning() << "Could not open device:" << errbuf;
        return;
    }
    
    struct pcap_pkthdr header;
    const unsigned char *packet;
    int packetNumber = 0;
    
    while (!stopRequested) {
        packet = pcap_next(handle, &header);
        
        if (packet != nullptr) {
            PacketInfo info = parseRawPacket(packet, header.len, &header);
            info.number = ++packetNumber;
            emit packetCaptured(info);
        }
    }
    
    pcap_close(handle);
}

PacketInfo CaptureThread::parseRawPacket(const unsigned char *packet, int length, const struct pcap_pkthdr *header)
{
    PacketInfo info;
    info.timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    info.timestampMicros = (quint64)header->ts.tv_sec * 1000000 + header->ts.tv_usec;
    info.length = length;
    info.protocol = "Unknown";
    info.source = "Unknown";
    info.destination = "Unknown";
    info.srcPort = 0;
    info.dstPort = 0;
    info.info = "";
    
    // Store raw packet data for PCAP export
    info.rawData = QByteArray(reinterpret_cast<const char*>(packet), length);
    
    // Parse Ethernet header
    struct ether_header *eth = (struct ether_header *)packet;
    
    if (ntohs(eth->ether_type) == ETHERTYPE_IP) {
        // Parse IP header
        struct ip *iph = (struct ip *)(packet + sizeof(struct ether_header));
        
        char srcIP[INET_ADDRSTRLEN];
        char dstIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(iph->ip_src), srcIP, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(iph->ip_dst), dstIP, INET_ADDRSTRLEN);
        
        info.source = QString(srcIP);
        info.destination = QString(dstIP);
        
        int ipHeaderLen = iph->ip_hl * 4;
        
        if (iph->ip_p == IPPROTO_TCP) {
            info.protocol = "TCP";
            struct tcphdr *tcph = (struct tcphdr *)(packet + sizeof(struct ether_header) + ipHeaderLen);
            info.srcPort = ntohs(tcph->th_sport);
            info.dstPort = ntohs(tcph->th_dport);
            info.info = QString("TCP %1 → %2").arg(info.srcPort).arg(info.dstPort);
        } else if (iph->ip_p == IPPROTO_UDP) {
            info.protocol = "UDP";
            struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct ether_header) + ipHeaderLen);
            info.srcPort = ntohs(udph->uh_sport);
            info.dstPort = ntohs(udph->uh_dport);
            info.info = QString("UDP %1 → %2").arg(info.srcPort).arg(info.dstPort);
        } else if (iph->ip_p == IPPROTO_ICMP) {
            info.protocol = "ICMP";
            info.info = "ICMP packet";
        }
    } else if (ntohs(eth->ether_type) == ETHERTYPE_ARP) {
        info.protocol = "ARP";
        info.info = "ARP packet";
    }
    
    // Store raw data as hex (limited)
    QString hex;
    int maxBytes = qMin(length, 128);
    for (int i = 0; i < maxBytes; ++i) {
        hex += QString("%1 ").arg(packet[i], 2, 16, QChar('0'));
        if ((i + 1) % 16 == 0) hex += "\n";
    }
    if (length > maxBytes) {
        hex += QString("\n... (%1 more bytes)").arg(length - maxBytes);
    }
    info.data = hex;
    
    return info;
}
#endif
