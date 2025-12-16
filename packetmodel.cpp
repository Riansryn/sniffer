#include "packetmodel.h"

PacketModel::PacketModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    headers << "No." << "Time" << "Source" << "Destination" << "Protocol" << "Length" << "Info";
}

int PacketModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return packets.size();
}

int PacketModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return headers.size();
}

QVariant PacketModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= packets.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        const PacketInfo &packet = packets.at(index.row());
        
        switch (index.column()) {
        case 0: return packet.number;
        case 1: return packet.timestamp;
        case 2: return QString("%1:%2").arg(packet.source).arg(packet.srcPort);
        case 3: return QString("%1:%2").arg(packet.destination).arg(packet.dstPort);
        case 4: return packet.protocol;
        case 5: return packet.length;
        case 6: return packet.info;
        default: return QVariant();
        }
    }
    
    return QVariant();
}

QVariant PacketModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section >= 0 && section < headers.size()) {
            return headers.at(section);
        }
    }
    
    return QVariant();
}

void PacketModel::addPacket(const PacketInfo &packet)
{
    beginInsertRows(QModelIndex(), packets.size(), packets.size());
    packets.append(packet);
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
