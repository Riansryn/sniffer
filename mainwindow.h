#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "packetsniffer.h"
#include "packetmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartCapture();
    void onStopCapture();
    void onClearPackets();
    void onPacketReceived(const PacketInfo &packet);
    void onPacketSelected(const QModelIndex &index);
    void onSaveToPcap();

private:
    Ui::MainWindow *ui;
    PacketSniffer *sniffer;
    PacketModel *packetModel;
    
    void setupUI();
    void loadNetworkInterfaces();
    void updateStatus(const QString &message);
    bool writePcapFile(const QString &filename, const QList<PacketInfo> &packets);
};

#endif // MAINWINDOW_H
