#ifndef MPAX_PLAYLISTSQL_H
#define MPAX_PLAYLISTSQL_H

#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>

#include "core/playlist.h"

#define SQL_DB_NAME QCoreApplication::applicationDirPath() + "/playlist.db"

class PlaylistSql : public QObject {
 public:
  static PlaylistSql* getInstance();
  PlaylistSql(const PlaylistSql&) = default;
  PlaylistSql& operator=(const PlaylistSql&) = delete;

 public slots:
  void savePlaylist(const QList<Playlist>& playlists);
  static void savePlaylist(const QString& filePath);
  void removePlaylist(const int& index);
  void updatePlaylist(const int& index, const Playlist& playlist);
  QList<Playlist> loadPlaylist();

 private:
  QSqlDatabase m_database;
  QVector<QPair<QString, QString>> m_nameVector;

  PlaylistSql();
  ~PlaylistSql();
  bool tryOpenDatabase();
  void tryCloseDatabase();
};

#endif  // MPAX_PLAYLISTSQL_H
