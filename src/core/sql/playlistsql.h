#ifndef MPAX_PLAYLISTSQL_H
#define MPAX_PLAYLISTSQL_H

#include <QtCore/QObject>
#include <QtCore/QStandardPaths>
#include <QtSql/QSqlDatabase>

#include "core/playlist.h"

#if defined(Q_OS_WINDOWS) || defined(Q_OS_WIN)
#define SQL_DB_NAME QCoreApplication::applicationDirPath() + "/playlist.db"
#else
#define SQL_DB_NAME                                                         \
  QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + \
      "/MPax/playlist.db"
#endif

class PlaylistSql : public QObject {
 public:
  static PlaylistSql *getInstance();
  PlaylistSql(const PlaylistSql &) = default;
  PlaylistSql &operator=(const PlaylistSql &) = delete;

 public slots:
  void savePlaylist(const QList<Playlist> &playlists);
  static void savePlaylist(const QString &filePath);
  void removePlaylist(const Playlist &playlist);
  void updatePlaylist(Playlist *playlist);
  QList<Playlist> loadPlaylist();
  bool loadPlaylistWithOrder(Playlist *playlist, const QString &columnName,
                             Qt::SortOrder order);
  void updatePlayContent(const Playlist *playlist,
                         const PlayContent *playContent);

 private:
  enum SqlAction : int { Create = 0, Insert, Update };

  struct ColumnHeader {
    QString name;
    QString type;
    QString properties;
  };

  QSqlDatabase m_database;
  QMap<QString, ColumnHeader> m_titleMap;

  PlaylistSql();
  ~PlaylistSql();
  bool tryOpenDatabase();
  void tryCloseDatabase();
  bool prepareSql(QSqlQuery *query, const PlayContent *playContent,
                  const QString &tableName, SqlAction action,
                  const QStringList &columnList, int id = -1);
};

#endif  // MPAX_PLAYLISTSQL_H
