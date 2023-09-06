#include "listtabmodel.h"

#include <QtCore/QFile>
#include <QtCore/QMimeData>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>
#include <QtCore/QtDebug>

#include "config/appplaylist.h"
#include "core/playlistjson.h"
#include "core/sql/playlistsql.h"

ListTabModel::ListTabModel()
    : m_currentPlayListModel(nullptr), m_reloadPlaylistTimer(new QTimer(this)) {
  connect(this, &ListTabModel::dataChanged, this,
          &ListTabModel::saveDefaultPlaylist);
  for (auto list : m_playlistList) {
    connect(list, &PlaylistModel::reloadInfoStatusChanged, this,
            &ListTabModel::reloadInfoStatusChanged);
  }
  connect(this, &ListTabModel::reloadPlaylist, m_reloadPlaylistTimer,
          QOverload<>::of(&QTimer::start));
  m_reloadPlaylistTimer->setInterval(20);
  m_reloadPlaylistTimer->setSingleShot(true);
  connect(m_reloadPlaylistTimer, &QTimer::timeout, this,
          [this]() { emit currentPlaylistChanged(m_currentPlayListModel); });
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
    emit reloadPlaylist();
  }
  connect(playlistModel, &PlaylistModel::reloadInfoStatusChanged, this,
          &ListTabModel::reloadInfoStatusChanged);
  connect(playlistModel, &PlaylistModel::currentPlayContentUpdated, this,
          &ListTabModel::currentPlayContentUpdated);
  connect(playlistModel, &PlaylistModel::playlistChanged, this,
          &ListTabModel::savePlaylist);
#if 0
  QMetaObject::invokeMethod(playlistModel, "reloadPlayContentInfo",
                            Qt::QueuedConnection);
#endif
}

void ListTabModel::removePlaylist(const int &index) {
  if (m_playlistList.length() <= index) {
    return;
  }
  m_currentPlayListModel = nullptr;
  PlaylistSql::getInstance()->removePlaylist(m_playlistList[index]->list());
  m_playlistList.removeAt(index);
}

void ListTabModel::setCurrentPlaylist(const int &index) {
  if (m_playlistList.length() <= index) {
    qDebug() << "can not set to a playlist out of index with value" << index;
    return;
  }
  m_currentPlayListModel = m_playlistList[index];
  emit reloadPlaylist();
}

PlaylistModel *ListTabModel::currentPlaylist() const {
  return m_currentPlayListModel;
}

void ListTabModel::addContent(PlayContent *playContent) {
  if (m_currentPlayListModel == nullptr) {
    /**
     * Seems here is somewhere abnormal.
     * Before adding content, suggests using Mainui::addPlaylist() to ensure
     * there is at least one playlist.
     * Create a playlist here may lose some initialization.
     */
    m_currentPlayListModel = new PlaylistModel(DEFAULT_PLAYLIST_NAME);
    // Add playlist to ListTabWidget.
    addPlaylist(m_currentPlayListModel);
  }
  m_currentPlayListModel->addContent(playContent);
  // Sync playlist content to PlaylistWidget.
  emit reloadPlaylist();
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

void ListTabModel::saveAllPlaylist(const QString &dirPath) const {
  for (auto playlist : m_playlistList) {
    QFile f(dirPath + "/" + playlist->playlistName() + ".m3u8");
    QTextStream stream;
    stream.setCodec("UTF-8");
    stream.setDevice(&f);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
      qDebug() << "can not save playlist to " << f.fileName();
      continue;
    }
    stream << "#"
           << "\n";
    for (auto content : playlist->list().content()) {
      stream << content->contentPath << "\n";
    }
    f.close();
  }
}

void ListTabModel::saveAllPlaylist() const {
  QList<Playlist> allList;
  for (auto playlist : m_playlistList) {
    allList.append(playlist->list());
  }
  PlaylistSql::getInstance()->savePlaylist(allList);
}

void ListTabModel::saveDefaultPlaylist() const { saveAllPlaylist(); }

int ListTabModel::indexOf(PlaylistModel *playlistModel) const {
  return m_playlistList.indexOf(playlistModel);
}

void ListTabModel::saveCurrentPlaylist() {
  auto playlist =
      m_playlistList[m_playlistList.indexOf(m_currentPlayListModel)]->list();
  PlaylistSql::getInstance()->updatePlaylist(&playlist);
}

void ListTabModel::savePlaylist(Playlist *playlist) {
  PlaylistSql::getInstance()->updatePlaylist(playlist);
}

Qt::ItemFlags ListTabModel::flags(const QModelIndex &index) const {
  if (index.isValid()) {
    return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled |
           QAbstractItemModel::flags(index);
  }
  return QAbstractItemModel::flags(index);
}

Qt::DropActions ListTabModel::supportedDropActions() const {
  return Qt::MoveAction | QAbstractItemModel::supportedDropActions();
}

QMimeData *ListTabModel::mimeData(const QModelIndexList &indexList) const {
  return new QMimeData();
}

bool ListTabModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                int row, int column,
                                const QModelIndex &parent) {
  if (data == nullptr) {
    return false;
  }
  const QString oldPlaylistName =
      QString::fromUtf8(data->data("PlaylistTableName"));
  const QStringList oldRowsStrList =
      QString::fromUtf8(data->data("OldRows")).split(',');
  const QList<QUrl> urlList = data->urls();

  PlaylistModel *acceptPlaylist = m_playlistList[parent.row()];
  const QString acceptPlaylistTableName =
      acceptPlaylist->list().info().info(PLAYLIST_INFO_TABLE_NAME);
  PlaylistModel *oldPlaylist = nullptr;
  for (auto &playlist : m_playlistList) {
    if (playlist->list().info().info(PLAYLIST_INFO_TABLE_NAME) ==
        oldPlaylistName) {
      oldPlaylist = playlist;
    }
  }

  if (oldPlaylist != nullptr && !oldPlaylistName.isEmpty() &&
      !oldRowsStrList.isEmpty() && oldPlaylistName != acceptPlaylistTableName) {
    //    Accept data from another playlist.
    for (auto &url : urlList) {
      acceptPlaylist->addContent(oldPlaylist->content(url.path()).content);
    }
    // TODO: Only save the playlist accepted.
    saveAllPlaylist();
    return true;
  }

  return false;
}

void ListTabModel::renamePlaylist(int index, const QString &name) {
  if (m_playlistList.length() <= index || index < 0) {
    return;
  }
  beginResetModel();
  m_playlistList[index]->setPlaylistName(name);
  endResetModel();
}
