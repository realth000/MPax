#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QtCore/QAbstractItemModel>

#include "core/playcontent.h"
#include "core/playlist.h"

typedef QPair<QString, bool> PlaylistHeaderItem;

class PlaylistModelHeader {
 public:
  explicit PlaylistModelHeader(const QList<PlaylistHeaderItem> &headerList);
  int headerCount() const;
  int usedHeaderCount() const;
  bool setHeaderUsed(const PlaylistHeaderItem &headerItem);
  QString header(const int &index) const;
  QString usedHeader(const int &index) const;
  static QList<PlaylistHeaderItem> defaultHeaderList();

 private:
  QList<QPair<QString, bool>> m_header;
};

class PlaylistModel : public QAbstractItemModel {
 public:
  explicit PlaylistModel(const QString &playlistName, QList<PlaylistHeaderItem>,
                         QObject *parent = nullptr);
  QModelIndex parent(const QModelIndex &index) const override;
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  int count() const;
  bool contains(PlayContent *content) const;
  int indexOf(PlayContent *content) const;
  void addContent(PlayContent *content);
  void setPlaylistName(const QString &name);
  QString playlistName() const;
  PlayContent *currentPlayContent() const;
  void setCurrentPlayContent(const int &index);
  PlayContent *findNextContent() const;
  PlayContent *findPreContent() const;
  PlayContent *content(const int &index) const;
  Playlist list() const;

 private:
  QString m_playlistName;
  // TODO: Combine PlaylistInfo and PlayContentList to PlayContentList.
  PlaylistInfo m_listInfo;
  PlayContentList m_contentList;
  PlaylistModelHeader m_header;
  PlayContent *m_currentPlayContent;
};

#endif  // PLAYLISTMODEL_H