#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QVariantList>
#include "packetsniffer.h"
#include "packetmodel.h"

class MainWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PacketModel* packetModel READ getPacketModel CONSTANT)
    Q_PROPERTY(QVariantList networkInterfaces READ networkInterfaces NOTIFY networkInterfacesChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString packetDetails READ packetDetails NOTIFY packetDetailsChanged)
    Q_PROPERTY(bool capturing READ isCapturing NOTIFY capturingChanged)

public:
    explicit MainWindow(QObject *parent = nullptr);
    ~MainWindow();
    
    PacketModel* getPacketModel() const { return packetModel; }
    QVariantList networkInterfaces() const { return m_networkInterfaces; }
    QString statusMessage() const { return m_statusMessage; }
    QString packetDetails() const { return m_packetDetails; }
    bool isCapturing() const;
    
    Q_INVOKABLE void startCapture(const QString &interface);
    Q_INVOKABLE void stopCapture();
    Q_INVOKABLE void clearPackets();
    Q_INVOKABLE void selectPacket(int index);
    Q_INVOKABLE void saveToPcap();

signals:
    void networkInterfacesChanged();
    void statusMessageChanged();
    void packetDetailsChanged();
    void capturingChanged();

private slots:
    void onPacketReceived(const PacketInfo &packet);

private:
    PacketSniffer *sniffer;
    PacketModel *packetModel;
    QVariantList m_networkInterfaces;
    QString m_statusMessage;
    QString m_packetDetails;
    
    void loadNetworkInterfaces();
    void updateStatus(const QString &message);
    bool writePcapFile(const QString &filename, const QList<PacketInfo> &packets);
};

#endif // MAINWINDOW_H
