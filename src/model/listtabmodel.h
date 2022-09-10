#ifndef LISTTABMODEL_H
#define LISTTABMODEL_H

#include <QtCore/QList>
#include <QtCore/QStringListModel>
#include <QtCore/QTimer>

#include "model/playlistmodel.h"

#define DEFAULT_PLAYLIST_NAME "default"

class ListTabModel : public QStringListModel {
  Q_OBJECT

 public:
  ListTabModel();
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole) override;

 signals:
  void currentPlaylistChanged(PlaylistModel *playlistModel);
  void dataChanged();
  void reloadInfoStatusChanged(QString playlistName, bool finished, int count,
                               qint64 time);
  void currentPlayContentUpdated(PlayContentPos pos);
  void reloadPlaylist();

 public slots:
  void addContent(PlayContent *playContent);
  void addPlaylist(PlaylistModel *playlistModel);
  void removePlaylist(const int &index);
  void setCurrentPlaylist(const int &index);
  PlaylistModel *currentPlaylist() const;
  void saveAllPlaylist(const QString &dirPath) const;
  void saveAllPlaylist() const;
  void saveDefaultPlaylist() const;
  int indexOf(PlaylistModel *playlistModel) const;
  void saveCurrentPlaylist();
  static void savePlaylist(Playlist *playlist);

 private:
  QList<PlaylistModel *> m_playlistList;
  PlaylistModel *m_currentPlayListModel;
  QTimer *m_reloadPlaylistTimer;
};

#endif  // LISTTABMODEL_H
