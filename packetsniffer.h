#ifndef PACKETSNIFFER_H
#define PACKETSNIFFER_H

#include <QObject>
#include <QThread>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QNetworkDatagram>

struct PacketInfo {
    int number;
    QString timestamp;
    QString source;
    QString destination;
    int srcPort;
    int dstPort;
    QString protocol;
    int length;
    QString info;
    QString data;
    QByteArray rawData;
    quint64 timestampMicros;
};

Q_DECLARE_METATYPE(PacketInfo)

class PacketSniffer : public QObject
{
    Q_OBJECT

public:
    explicit PacketSniffer(QObject *parent = nullptr);
    ~PacketSniffer();

    bool startCapture(const QString &interface);
    void stopCapture();
    bool isCapturing() const { return capturing; }

signals:
    void packetReceived(const PacketInfo &packet);
    void errorOccurred(const QString &error);

private slots:
    void onSocketReadyRead();

private:
    bool capturing;
    int packetCount;
    QUdpSocket *udpSocket;
    
    void parsePacket(const QByteArray &data, const QHostAddress &sender, quint16 port);
    QString formatTimestamp();
    QString byteArrayToHex(const QByteArray &data);
    
#ifdef LINUX_PLATFORM
    void captureWithPcap(const QString &interface);
    class CaptureThread *captureThread;
#endif
};

#ifdef LINUX_PLATFORM
struct pcap_pkthdr;

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    CaptureThread(const QString &interface, QObject *parent = nullptr);
    void stop();

protected:
    void run() override;

signals:
    void packetCaptured(const PacketInfo &packet);

private:
    QString interface;
    bool stopRequested;
    
    PacketInfo parseRawPacket(const unsigned char *packet, int length, const struct pcap_pkthdr *header);
};
#endif

#endif // PACKETSNIFFER_H
