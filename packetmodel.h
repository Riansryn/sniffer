#ifndef PACKETMODEL_H
#define PACKETMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "packetsniffer.h"

class PacketModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum PacketRoles {
        NumberRole = Qt::UserRole + 1,
        TimestampRole,
        TimestampSecsRole,
        SourceRole,
        DestinationRole,
        SrcPortRole,
        DstPortRole,
        ProtocolRole,
        LengthRole,
        InterfaceRole,
        InfoRole
    };

    explicit PacketModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addPacket(const PacketInfo &packet);
    void clear();
    PacketInfo getPacket(int row) const;
    QList<PacketInfo> getAllPackets() const;

private:
    QList<PacketInfo> packets;
};

#endif // PACKETMODEL_H
