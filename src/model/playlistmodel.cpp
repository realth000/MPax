#include "playlistmodel.h"

#include <QDebug>
#include <QtConcurrent/QtConcurrentMap>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFutureWatcher>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#include "audio/audioinfo.h"
#include "core/sql/playlistsql.h"

PlaylistModel::PlaylistModel(const QString &playlistName, QObject *parent)
    : QAbstractItemModel{parent},
      m_playlistName(playlistName),
      m_playlist(new Playlist(PlaylistInfo(QMap<QString, QString>{
                                  {PLAYLIST_INFO_NAME, m_playlistName}}),
                              PlayContentList())),
      m_currentPlayContent(nullptr),
      m_headerTrans(MODEL_ALL_HEADER) {}

PlaylistModel::PlaylistModel(const Playlist &playlist, QObject *parent)
    : QAbstractItemModel{parent},
      m_playlistName(playlist.info().info(PLAYLIST_INFO_NAME)),
      m_playlist(new Playlist(playlist.info(), playlist.content())),
      m_currentPlayContent(nullptr),
      m_headerTrans(MODEL_ALL_HEADER) {}

QModelIndex PlaylistModel::parent(const QModelIndex &index) const {
  // TODO: Check if incorrent.
  return QModelIndex();
}

QModelIndex PlaylistModel::index(int row, int column,
                                 const QModelIndex &parent) const {
  if (row < 0 || column < 0 || row >= rowCount(parent) ||
      column >= columnCount(parent)) {
    return QModelIndex();
  }
  // TODO: Check if incorrent.
  return createIndex(row, column);
}

int PlaylistModel::rowCount(const QModelIndex &parent) const {
  return m_playlist->content().length();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const {
  // Only shows used column
  return PLModel::PlaylistModelHeader::getInstance()->usedHeaderCount();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.model() != this) {
    return QVariant();
  }
  if (role == Qt::TextAlignmentRole) {
    return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
  }
  if (role == Qt::DisplayRole) {
    return QVariant(m_playlist->content()[index.row()]->value(
        PLModel::PlaylistModelHeader::getInstance()->usedHeader(
            index.column())));
  }
  return QVariant();
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  switch (role) {
    case Qt::DecorationRole:
      //      if (section == 0) {
      //        QImage pixmap(16, 1, QImage::Format_ARGB32_Premultiplied);
      //        pixmap.fill(Qt::transparent);
      //        return pixmap;
      //      }
      return QVariant();
      break;
    case Qt::TextAlignmentRole:
      return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
  }
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  return m_headerTrans[PLModel::PlaylistModelHeader::getInstance()->usedHeader(
      section)];
}

int PlaylistModel::count() const { return m_playlist->content().length(); }

void PlaylistModel::addContent(PlayContent *content) {
  reloadPlayContentInfo(content);
  beginResetModel();
  m_playlist->appendContent(content);
  endResetModel();
  m_playlist->setInfo(
      PLAYLIST_INFO_COUNT,
      QString::number(m_playlist->info().info(PLAYLIST_INFO_COUNT).toInt() +
                      1));
}

bool PlaylistModel::removeContent(QList<int> indexes) {
  std::sort(indexes.begin(), indexes.end());
  QList<int>::const_reverse_iterator it = indexes.crbegin();
  while (it != indexes.crend()) {
    if (m_playlist->content().size() <= *it) {
      qDebug() << "remove content out of index" << m_playlist->content().size()
               << *it;
      return false;
    }
    beginResetModel();
    m_playlist->removeContentAt(*it);
    endResetModel();
    it++;
  }
  return true;
}

void PlaylistModel::setPlaylistName(const QString &name) {
  m_playlistName = name;
  m_playlist->setInfo(PLAYLIST_INFO_NAME, m_playlistName);
}

QString PlaylistModel::playlistName() const { return m_playlistName; }

PlayContentPos PlaylistModel::currentPlayContent() const {
  int i = m_playlist->content().indexOf(m_currentPlayContent);
  if (i < 0) {
    return PlayContentPos{-1, nullptr};
  }
  return PlayContentPos{i, m_currentPlayContent};
}

void PlaylistModel::setCurrentPlayContent(const int &index) {
  if (m_playlist->content().length() <= index) {
    qDebug() << "can not set current play content out of index";
    return;
  }
  m_currentPlayContent = m_playlist->content()[index];
}

PlayContentPos PlaylistModel::findNextContent() const {
  if (m_playlist->content().length() == 0) {
    return PlayContentPos{-1, nullptr};
  }
  const int i = m_playlist->content().indexOf(m_currentPlayContent);
  if (i == m_playlist->content().length() - 1) {
    return PlayContentPos{0, m_playlist->content()[0]};
  }
  return PlayContentPos{i + 1, m_playlist->content()[i + 1]};
}

PlayContentPos PlaylistModel::findPreContent() const {
  if (m_playlist->content().length() == 0) {
    return PlayContentPos{-1, nullptr};
  }
  const int i = m_playlist->content().indexOf(m_currentPlayContent);
  if (i == 0) {
    return PlayContentPos{
        m_playlist->content().length() - 1,
        m_playlist->content()[m_playlist->content().length() - 1]};
  }
  return PlayContentPos{i - 1, m_playlist->content()[i - 1]};
}

bool PlaylistModel::contains(PlayContent *content) const {
  return m_playlist->content().contains(content);
}

int PlaylistModel::indexOf(PlayContent *content) const {
  return m_playlist->content().indexOf(content);
}

PlayContentPos PlaylistModel::content(const int &index) const {
  if (m_playlist->content().length() <= index) {
    return PlayContentPos{-1, nullptr};
  }
  return PlayContentPos{index, m_playlist->content()[index]};
}

PlayContentPos PlaylistModel::content(const QString &contentPath) const {
  int i = 0;
  for (auto c : m_playlist->content()) {
    if (c->contentPath == contentPath) {
      return PlayContentPos{i, c};
    }
    i++;
  }
  return PlayContentPos{-1, nullptr};
}

Playlist PlaylistModel::list() const { return *m_playlist; }

void PlaylistModel::reloadPlayContentInfo() {
  // FIXME: Do not use the latter one which will cause OOM.
#if 1
  QFutureWatcher<void> *reloadWatcher = new QFutureWatcher<void>;
  QTimer *progressTimer = new QTimer;
  QElapsedTimer *t = new QElapsedTimer;
  connect(progressTimer, &QTimer::timeout, this,
          [this, reloadWatcher, t, progressTimer]() {
            emit this->reloadInfoStatusChanged(
                m_playlist->info().info(PLAYLIST_INFO_NAME),
                reloadWatcher->isFinished(), reloadWatcher->progressValue(),
                t->elapsed());
            if (reloadWatcher->isFinished()) {
              progressTimer->stop();
              progressTimer->deleteLater();
              t->invalidate();
              delete t;
              reloadWatcher->deleteLater();
            }
          });
  progressTimer->start(100);
  t->start();
  PlayContentList contentList = m_playlist->content();
  QFuture<void> reloadFuture = QtConcurrent::map(
      contentList, [&](PlayContent *content) -> PlayContent * {
        AudioInfo::readAudioInfo(content->contentPath, content,
                                 AudioInfo::InfoOption::NoAlbumCover);
        return content;
      });
  reloadWatcher->setFuture(reloadFuture);
  m_playlist->setContent(&contentList);
#else
  AudioInfo *reloader = new AudioInfo;
  connect(reloader, &AudioInfo::reloadInfoStatusChanged, this,
          [this, reloader](bool finished, int count, qint64 time) {
            emit this->reloadInfoStatusChanged(
                m_listInfo.info(PLAYLIST_INFO_NAME), finished, count, time);
            if (finished) {
              reloader->deleteLater();
            }
          });
  reloader->readAudioInfoList(&m_contentList,
                              AudioInfo::InfoOption::NoAlbumCover);
#endif
}

void PlaylistModel::reloadPlayContentInfo(PlayContent *content) {
  AudioInfo::readAudioInfo(content->contentPath, content,
                           AudioInfo::InfoOption::NoAlbumCover);
}

void PlaylistModel::reloadPlaylistWithOrder(const int &column,
                                            Qt::SortOrder order) {
  beginResetModel();
  if (!PlaylistSql::getInstance()->loadPlaylistWithOrder(
          m_playlist,
          PLModel::PlaylistModelHeader::getInstance()->usedHeader(column),
          order)) {
    qDebug() << "failed to reload playlist with order";
    return;
  }
  endResetModel();
  // Playlist will be rebuilt here, everything about this playlist that requires
  // update must be done.
  updatePlaylistState();
}

const QModelIndex PlaylistModel::find(const QString &contentPath) const {
  for (int i = 0; i < m_playlist->content().length(); i++) {
    if (m_playlist->content()[i]->contentPath == contentPath) {
      return index(i, 0);
    }
  }
  return QModelIndex();
}

void PlaylistModel::updatePlayContent(const PlayContent *playContent) const {
  PlaylistSql::getInstance()->updatePlayContent(m_playlist, playContent);
}

void PlaylistModel::setUsedHeader(const QString &header, bool used) {
  beginResetModel();
  PLModel::PlaylistModelHeader::getInstance()->setUsedHeader(header, used);
  endResetModel();
}

void PlaylistModel::updatePlaylistState() {
  PlayContentList playContentList = m_playlist->content();
  for (int i = 0; i < playContentList.length(); i++) {
    if (m_currentPlayContent != nullptr &&
        playContentList[i]->contentPath == m_currentPlayContent->contentPath) {
      m_currentPlayContent = playContentList[i];
      emit currentPlayContentUpdated(PlayContentPos{i, m_currentPlayContent});
      break;
    }
  }
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const {
  if (index.isValid()) {
    return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled |
           QAbstractItemModel::flags(index);
  }
  return QAbstractItemModel::flags(index);
}

Qt::DropActions PlaylistModel::supportedDropActions() const {
  return Qt::MoveAction | QAbstractItemModel::supportedDropActions();
}

QMimeData *PlaylistModel::mimeData(const QModelIndexList &indexList) const {
  QMimeData *data = QAbstractItemModel::mimeData(indexList);
  if (data == nullptr) {
    return QAbstractItemModel::mimeData(indexList);
  }
  QStringList oldRowsList;
  QList<QUrl> urlList;
  for (auto &index : indexList) {
    PlayContentPos pc = this->content(index.row());
    if (pc.index != -1 && pc.content != nullptr &&
        !oldRowsList.contains(QString::number(pc.index))) {
      oldRowsList.append(QString::number(pc.index));
      urlList.append(QUrl("file://" + pc.content->contentPath));
    }
  }
  data->setData("PlaylistTableName",
                m_playlist->info().info(PLAYLIST_INFO_TABLE_NAME).toUtf8());
  data->setData("OldRows", oldRowsList.join(',').toUtf8());
  data->setUrls(urlList);
  return data;
}

bool PlaylistModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                 int row, int column,
                                 const QModelIndex &parent) {
  if (data == nullptr ||
      (action != Qt::MoveAction && action != Qt::CopyAction)) {
    return false;
  }

  const QString oldPlaylistName =
      QString::fromUtf8(data->data("PlaylistTableName"));
  const QStringList oldRowsStrList =
      QString::fromUtf8(data->data("OldRows")).split(',');
  const QList<QUrl> urlList = data->urls();

  // Probably not happen but check to ensure.
  if (oldPlaylistName.isEmpty() || oldRowsStrList.isEmpty() ||
      urlList.isEmpty() || oldRowsStrList.count() != urlList.count()) {
    return false;
  };

  QList<int> oldRows;
  for (auto &t : oldRowsStrList) {
    oldRows.append(t.toInt());
  }
  std::sort(oldRows.begin(), oldRows.end(),
            [](int a, int b) -> bool { return a > b; });

  beginResetModel();
  if (oldPlaylistName == m_playlist->info().info(PLAYLIST_INFO_TABLE_NAME)) {
    // Move and resort
    int countBefore = -1;
    QList<PlayContent *> contentList;
    for (auto &r : oldRows) {
      contentList.append(m_playlist->takeContent(r));
      if (r <= parent.row()) {
        countBefore++;
      }
    }
    for (auto &p : contentList) {
      m_playlist->insertContent(parent.row() - countBefore, p);
    }
  } else {
  }
  endResetModel();
  emit playlistChanged(m_playlist);
  return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
}
