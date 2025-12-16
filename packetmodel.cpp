#include "packetmodel.h"

PacketModel::PacketModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int PacketModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return packets.size();
}

QVariant PacketModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= packets.size()) {
        return QVariant();
    }

    const PacketInfo &packet = packets.at(index.row());
    
    switch (role) {
    case NumberRole: return packet.number;
    case TimestampRole: return packet.timestamp;
    case TimestampSecsRole: return packet.timestampSecs;
    case SourceRole: return packet.source;
    case DestinationRole: return packet.destination;
    case SrcPortRole: return packet.srcPort;
    case DstPortRole: return packet.dstPort;
    case ProtocolRole: return packet.protocol;
    case LengthRole: return packet.length;
    case InterfaceRole: return packet.interfaceName;
    case InfoRole: return packet.info;
    default: return QVariant();
    }
}

QHash<int, QByteArray> PacketModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NumberRole] = "number";
    roles[TimestampRole] = "timestamp";
    roles[TimestampSecsRole] = "timestampSecs";
    roles[SourceRole] = "source";
    roles[DestinationRole] = "destination";
    roles[SrcPortRole] = "srcPort";
    roles[DstPortRole] = "dstPort";
    roles[ProtocolRole] = "protocol";
    roles[LengthRole] = "length";
    roles[InterfaceRole] = "interfaceName";
    roles[InfoRole] = "info";
    return roles;
}

void PacketModel::addPacket(const PacketInfo &packet)
{
    beginInsertRows(QModelIndex(), 0, 0);
    packets.prepend(packet);
    endInsertRows();
}

void PacketModel::clear()
{
    beginResetModel();
    packets.clear();
    endResetModel();
}

PacketInfo PacketModel::getPacket(int row) const
{
    if (row >= 0 && row < packets.size()) {
        return packets.at(row);
    }
    return PacketInfo();
}

QList<PacketInfo> PacketModel::getAllPackets() const
{
    return packets;
}
