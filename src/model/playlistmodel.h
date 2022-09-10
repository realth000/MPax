#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QMimeData>
#include <QtCore/QObject>

#include "core/playcontent.h"
#include "core/playlist.h"
#include "playlistmodelheader.h"

// QPair<Name, Width>, width is temporary deprecated.
typedef QPair<QString, int> PlaylistHeaderItem;

struct PlayContentPos {
  int index;
  PlayContent *content;
};

class PlaylistModel : public QAbstractItemModel {
  // Translation need this marco!!!
  Q_OBJECT
 public:
  explicit PlaylistModel(const QString &playlistName,
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
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  Qt::DropActions supportedDropActions() const override;
  QMimeData *mimeData(const QModelIndexList &indexList) const override;
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                    int column, const QModelIndex &parent) override;

  int count() const;
  bool contains(PlayContent *content) const;
  const QModelIndex find(const QString &contentPath) const;
  int indexOf(PlayContent *content) const;
  void addContent(PlayContent *content);
  bool removeContent(QList<int> indexes);
  void setPlaylistName(const QString &name);
  QString playlistName() const;
  PlayContentPos currentPlayContent() const;
  void setCurrentPlayContent(const int &index);
  PlayContentPos findNextContent() const;
  PlayContentPos findPreContent() const;
  PlayContentPos content(const int &index) const;
  PlayContentPos content(const QString &contentPath) const;
  Playlist list() const;
  void updatePlayContent(const PlayContent *playContent) const;
  void setUsedHeader(const QString &header, bool used);

 public slots:
  void reloadPlayContentInfo();
  void reloadPlayContentInfo(PlayContent *content);
  void reloadPlaylistWithOrder(const int &column, Qt::SortOrder order);

 signals:
  void reloadInfoStatusChanged(QString playlistName, bool finished, int count,
                               qint64 time);
  void currentPlayContentUpdated(PlayContentPos pos);
  void playlistChanged(Playlist *playlist);

 private:
  QString m_playlistName;
  // TODO: Combine PlaylistInfo and PlayContentList to PlayContentList.
  Playlist *m_playlist;
  PlayContentList m_contentList;
  // Copy of PlaylistWidget::m_header.
  PlayContent *m_currentPlayContent;
  // FIXME: Duplicate with PlaylistModelHeader::m_headerTrans
  QMap<QString, QString> m_headerTrans;

  void updatePlaylistState();
};
#endif  // PLAYLISTMODEL_H
