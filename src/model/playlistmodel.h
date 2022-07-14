#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QObject>

#include "core/playcontent.h"
#include "core/playlist.h"

// QPair<Name, Width>, width is temporary deprecated.
typedef QPair<QString, int> PlaylistHeaderItem;

struct PlayContentPos {
  int index;
  PlayContent *content;
};

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
  QList<QPair<QString, int>> m_header;
};

class PlaylistModel : public QAbstractItemModel {
  // Translation need this marco!!!
  Q_OBJECT
 public:
  explicit PlaylistModel(const QString &playlistName, QList<PlaylistHeaderItem>,
                         QObject *parent = nullptr);
  explicit PlaylistModel(const Playlist &playlist, QObject *parent = nullptr);
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
  bool removeContent(QList<int> indexes);
  void setPlaylistName(const QString &name);
  void setHeader(const PlaylistModelHeader *header);
  QString playlistName() const;
  PlayContentPos currentPlayContent() const;
  void setCurrentPlayContent(const int &index);
  PlayContentPos findNextContent() const;
  PlayContentPos findPreContent() const;
  PlayContentPos content(const int &index) const;
  Playlist list() const;

 public slots:
  void reloadPlayContentInfo();
  void reloadPlayContentInfo(PlayContent *content);

 signals:
  void reloadInfoStatusChanged(QString playlistName, bool finished, int count,
                               qint64 time);

 private:
  QString m_playlistName;
  // TODO: Combine PlaylistInfo and PlayContentList to PlayContentList.
  PlaylistInfo m_listInfo;
  PlayContentList m_contentList;
  // Copy of PlaylistWidget::m_header.
  const PlaylistModelHeader *m_header;
  PlayContent *m_currentPlayContent;
  QMap<QString, QString> m_headerTrans;
};
#endif  // PLAYLISTMODEL_H
