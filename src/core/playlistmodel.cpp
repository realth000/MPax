#include "playlistmodel.h"

PlaylistModelHeader::PlaylistModelHeader(const QList<PlaylistHeaderItem> &headerList)
    : m_header(headerList)
{

}

int PlaylistModelHeader::headerCount() const
{
    return m_header.count();
}

int PlaylistModelHeader::usedHeaderCount() const
{
    int c = 0;
    for (const auto &h : m_header) {
        if (h.second) {
            c++;
        }
    }
    return c;
}

bool PlaylistModelHeader::setHeaderUsed(const PlaylistHeaderItem &headerItem)
{
    for (auto &&h : m_header) {
        if (h.first == headerItem.first) {
            h.second = headerItem.second;
            return true;
        }
    }
    return false;
}

QString PlaylistModelHeader::header(const int &index) const
{
    return m_header[index].first;
}

QString PlaylistModelHeader::usedHeader(const int &index) const
{
    int i = index;
    if (m_header.length() <= i) {
        return QString();
    }
    for (const auto &h : m_header) {
        if (!h.second) {
           continue;
        }
        if (i == 0) {
            return h.first;
        } else {
            i--;
        }
    }
    return QString();
}

QList<PlaylistHeaderItem> PlaylistModelHeader::defaultHeaderList()
{
    return QList<PlaylistHeaderItem>{PlaylistHeaderItem("ContentPath", true),PlaylistHeaderItem("ContentName", true)};
}

PlaylistModel::PlaylistModel(QList<QPair<QString, bool>> headerList, QObject *parent)
    : QAbstractItemModel{parent},
      m_header(headerList)
{

}

QModelIndex PlaylistModel::parent(const QModelIndex &index) const
{
    // TODO: Check if incorrent.
    return QModelIndex();
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent)) {
         return QModelIndex();
    }
    // TODO: Check if incorrent.
    return createIndex(row, column);
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_content.length();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    // Only shows used column
    return m_header.usedHeaderCount();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.model() != this){
            return QVariant();
        }
        if(role == Qt::TextAlignmentRole){
            return QVariant(Qt::AlignCenter);
        }
        if(role == Qt::DisplayRole){
            return QVariant(m_content[index.row()].value(m_header.usedHeader(index.column())));
        }
        return QVariant();
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DecorationRole:
        if (section == 0) {
            QImage pixmap(16, 1, QImage::Format_ARGB32_Premultiplied);
            pixmap.fill(Qt::transparent);
            return pixmap;
        }
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignLeft;
    }

    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractItemModel::headerData(section, orientation, role);
    }
    return m_header.usedHeader(section);
//        QString returnValue;
//        switch (section) {
//        case 0: returnValue = tr("Name");
//                break;
//        case 1: returnValue = tr("Size");
//                break;
//        case 2: returnValue =
//    #ifdef Q_OS_MAC
//                       tr("Kind", "Match OS X Finder");
//    #else
//                       tr("Type", "All other platforms");
//    #endif
//               break;
//        // Windows   - Type
//        // OS X      - Kind
//        // Konqueror - File Type
//        // Nautilus  - Type
//        case 3: returnValue = tr("Date Modified");
//                break;
//        default: return QVariant();
//        }
//        return returnValue;
}

void PlaylistModel::addContent(const PlayContent &content)
{
    m_content.append(content);
}


