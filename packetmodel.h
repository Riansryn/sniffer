#ifndef PACKETMODEL_H
#define PACKETMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "packetsniffer.h"

class PacketModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PacketModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void addPacket(const PacketInfo &packet);
    void clear();
    PacketInfo getPacket(int row) const;
    QList<PacketInfo> getAllPackets() const;

private:
    QList<PacketInfo> packets;
    QStringList headers;
};

#endif // PACKETMODEL_H
