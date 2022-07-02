#ifndef MPAX_PLAYLISTSQL_H
#define MPAX_PLAYLISTSQL_H

#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>

#include "core/playlist.h"

class PlaylistSql : public QObject {
 public:
  static PlaylistSql* getInstance();
  PlaylistSql(const PlaylistSql&) = default;
  PlaylistSql& operator=(const PlaylistSql&) = delete;

 public slots:
  void savePlaylist(const QList<Playlist>& playlists);
  void updatePlaylist(const QList<Playlist>& playlists);
  void loadPlaylist();

 private:
  PlaylistSql();
  ~PlaylistSql();
  QSqlDatabase m_database;
  QSqlDatabase m_memDatabase;
};

#endif  // MPAX_PLAYLISTSQL_H