#include "playlistmodel.h"

#include <QDebug>

PlaylistModelHeader::PlaylistModelHeader(
    const QList<PlaylistHeaderItem> &headerList)
    : m_header(headerList) {}

int PlaylistModelHeader::headerCount() const { return m_header.count(); }

int PlaylistModelHeader::usedHeaderCount() const {
  int c = 0;
  for (const auto &h : m_header) {
    if (h.second > 0) {
      c++;
    }
  }
  return c;
}

bool PlaylistModelHeader::setHeaderUsed(const PlaylistHeaderItem &headerItem) {
  for (auto &&h : m_header) {
    if (h.first == headerItem.first) {
      h.second = headerItem.second;
      return true;
    }
  }
  return false;
}

QString PlaylistModelHeader::header(const int &index) const {
  return m_header[index].first;
}

QString PlaylistModelHeader::usedHeader(const int &index) const {
  int i = index;
  if (m_header.length() <= i) {
    return QString();
  }
  for (const auto &h : m_header) {
    if (h.second <= 0) {
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

QList<PlaylistHeaderItem> PlaylistModelHeader::defaultHeaderList() {
  return QList<PlaylistHeaderItem>{PlaylistHeaderItem("ContentName", 100)};
}

PlaylistModel::PlaylistModel(const QString &playlistName,
                             QList<PlaylistHeaderItem> headerList,
                             QObject *parent)
    : QAbstractItemModel{parent},
      m_playlistName(playlistName),
      m_listInfo(PlaylistInfo(
          QMap<QString, QString>{{PLAYLIST_INFO_NAME, m_playlistName}})),
      m_currentPlayContent(nullptr),
      m_headerTrans(
          QMap<QString, QString>{{"ContentName", tr("ContentName")}}) {}

PlaylistModel::PlaylistModel(const Playlist &playlist, QObject *parent)
    : QAbstractItemModel{parent},
      m_playlistName(playlist.info()->info(PLAYLIST_INFO_NAME)),
      m_listInfo(*playlist.info()),
      m_contentList(*playlist.content()),
      m_currentPlayContent(nullptr),
      m_headerTrans(
          QMap<QString, QString>{{"ContentName", tr("ContentName")}}) {}

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
  return m_contentList.length();
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
    return QVariant(m_contentList[index.row()]->value(
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

int PlaylistModel::count() const { return m_contentList.length(); }

void PlaylistModel::addContent(PlayContent *content) {
  beginResetModel();
  m_contentList.append(content);
  endResetModel();
  m_listInfo.setInfo(
      PLAYLIST_INFO_COUNT,
      QString::number(m_listInfo.info(PLAYLIST_INFO_COUNT).toInt() + 1));
}

bool PlaylistModel::removeContent(const int &index) {
  if (m_contentList.size() <= index) {
    return false;
  }
  beginResetModel();
  m_contentList.removeAt(index);
  endResetModel();
  return true;
}

void PlaylistModel::setPlaylistName(const QString &name) {
  m_playlistName = name;
  m_listInfo.setInfo(PLAYLIST_INFO_NAME, m_playlistName);
}

QString PlaylistModel::playlistName() const { return m_playlistName; }

PlayContentPos PlaylistModel::currentPlayContent() const {
  return PlayContentPos{m_contentList.indexOf(m_currentPlayContent),
                        m_currentPlayContent};
}

void PlaylistModel::setCurrentPlayContent(const int &index) {
  if (m_contentList.length() <= index) {
    qDebug() << "can not set current play content out of index";
    return;
  }
  m_currentPlayContent = m_contentList[index];
}

PlayContentPos PlaylistModel::findNextContent() const {
  if (m_contentList.length() == 0) {
    return PlayContentPos{-1, nullptr};
  }
  const int i = m_contentList.indexOf(m_currentPlayContent);
  if (i == m_contentList.length() - 1) {
    return PlayContentPos{0, m_contentList[0]};
  }
  return PlayContentPos{i + 1, m_contentList[i + 1]};
}

PlayContentPos PlaylistModel::findPreContent() const {
  if (m_contentList.length() == 0) {
    return PlayContentPos{-1, nullptr};
  }
  const int i = m_contentList.indexOf(m_currentPlayContent);
  if (i == 0) {
    return PlayContentPos{m_contentList.length() - 1,
                          m_contentList[m_contentList.length() - 1]};
  }
  return PlayContentPos{i - 1, m_contentList[i - 1]};
}

bool PlaylistModel::contains(PlayContent *content) const {
  return m_contentList.contains(content);
}

int PlaylistModel::indexOf(PlayContent *content) const {
  return m_contentList.indexOf(content);
}

PlayContentPos PlaylistModel::content(const int &index) const {
  if (m_contentList.length() <= index) {
    return PlayContentPos{-1, nullptr};
  }
  return PlayContentPos{index, m_contentList[index]};
}

Playlist PlaylistModel::list() const {
  return Playlist(m_listInfo, m_contentList);
}

void PlaylistModel::setHeader(const PlaylistModelHeader *header) {
  m_header = header;
}
