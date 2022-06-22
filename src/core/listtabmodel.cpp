#include "listtabmodel.h"

ListTabModel::ListTabModel()
{

}

int ListTabModel::rowCount(const QModelIndex &parent) const
{
    return m_playlistList.length();
}

QVariant ListTabModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_playlistList.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return m_playlistList.at(index.row())->playlistName();
    }

    return QVariant();
}

void ListTabModel::addPlaylist(PlaylistModel *playlistModel)
{
    beginResetModel();
    m_playlistList.append(playlistModel);
    endResetModel();
}
