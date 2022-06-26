#include "listtabmodel.h"

#include <QtCore/QFile>
#include <QtCore/QtDebug>

#include "config/appplaylist.h"
#include "core/playlistjson.h"

ListTabModel::ListTabModel() : m_currentPlayListModel(nullptr) {
  connect(this, &ListTabModel::dataChanged, this,
          &ListTabModel::saveDefaultPlaylist);
}

int ListTabModel::rowCount(const QModelIndex &parent) const {
  return m_playlistList.length();
}

QVariant ListTabModel::data(const QModelIndex &index, int role) const {
  if (index.row() < 0 || index.row() >= m_playlistList.size()) {
    return QVariant();
  }

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    return m_playlistList.at(index.row())->playlistName();
  }

  return QVariant();
}

void ListTabModel::addPlaylist(PlaylistModel *playlistModel) {
  beginResetModel();
  m_playlistList.append(playlistModel);
  endResetModel();
  if (m_currentPlayListModel == nullptr) {
    m_currentPlayListModel = m_playlistList[m_playlistList.length() - 1];
    emit currentPlaylistChanged(m_currentPlayListModel);
  }
}

void ListTabModel::setCurrentPlaylist(const int &index) {
  if (m_playlistList.length() <= index) {
    qDebug() << "can not set to a playlist out of index with value" << index;
    return;
  }
  m_currentPlayListModel = m_playlistList[index];
  emit currentPlaylistChanged(m_currentPlayListModel);
}

PlaylistModel *ListTabModel::currentPlaylist() const {
  return m_currentPlayListModel;
}

void ListTabModel::addContent(PlayContent *playContent) {
  if (m_currentPlayListModel == nullptr) {
    m_currentPlayListModel = new PlaylistModel(
        DEFAULT_PLAYLIST_NAME,
        QList<PlaylistHeaderItem>{PlaylistModelHeader::defaultHeaderList()});
    // Add playlist to ListTabWidget.
    addPlaylist(m_currentPlayListModel);
  }
  m_currentPlayListModel->addContent(playContent);
  // Sync playlist content to PlaylistWidget.
  emit currentPlaylistChanged(m_currentPlayListModel);
}

bool ListTabModel::setData(const QModelIndex &index, const QVariant &value,
                           int role) {
  if (role == Qt::EditRole && !value.toString().isEmpty() &&
      m_playlistList.length() > index.row()) {
    beginResetModel();
    m_playlistList[index.row()]->setPlaylistName(value.toString());
    endResetModel();
    emit dataChanged();
    return true;
  }
  emit dataChanged();
  return QStringListModel::setData(index, value, role);
}

void ListTabModel::saveCurrentPlaylist(const QString &filePath) const {
  Config::AppPlaylist::savePlaylist(
      filePath, PlaylistJson::toJsonString(m_currentPlayListModel->list()));
}

void ListTabModel::saveAllPlaylist(const QString &filePath) const {
  QList<Playlist> allList;
  for (auto playlist : m_playlistList) {
    allList.append(playlist->list());
  }
  Config::AppPlaylist::savePlaylist(filePath,
                                    PlaylistJson::toJsonString(allList));
}

void ListTabModel::saveDefaultPlaylist() const {
  saveAllPlaylist(CONFIG_PLAYLIST_FILE_PATH);
}
