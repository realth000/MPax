#include "playlistmodel.h"

#include <QDebug>
#include <QtConcurrent/QtConcurrentMap>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFutureWatcher>
#include <QtCore/QTime>
#include <QtCore/QTimer>

#include "audio/audioinfo.h"
#include "core/playlistsql.h"

// clang-format off
#define MODEL_ALL_HEADER                                   \
  QMap<QString, QString> {                                 \
    {"Title", PlaylistModel::tr("Title")},                 \
    {"Artist", PlaylistModel::tr("Artist")},               \
    {"AlbumTitle", PlaylistModel::tr("AlbumTitle")},       \
    { "ContentName", PlaylistModel::tr("ContentName") }    \
  }
// clang-format on

PlaylistModel::PlaylistModel(const QString &playlistName,
                             QList<PlaylistHeaderItem> headerList,
                             QObject *parent)
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
  return m_header->usedHeaderCount();
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
        m_header->usedHeader(index.column())));
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
  return m_headerTrans[m_header->usedHeader(section)];
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
  return PlayContentPos{m_playlist->content().indexOf(m_currentPlayContent),
                        m_currentPlayContent};
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
          m_playlist, m_header->usedHeader(column), order)) {
    qDebug() << "failed to reload playlist with order";
    return;
  }
  endResetModel();
}

void PlaylistModel::setHeader(const PLModel::PlaylistModelHeader *header) {
  m_header = header;
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
