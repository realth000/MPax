#include "playlistsql.h"

#define SQL_DB_CONN "playlist_db_connection"
#define SQL_DB_NAME "playlist_db"
#define SQL_MEM_DB_CONN "playlist_mem_db_connection"
#define SQL_MEM_DB_NAME "playlist_mem_db"

PlaylistSql* PlaylistSql::getInstance() {
  static PlaylistSql ps;
  return &ps;
}

void PlaylistSql::savePlaylist(const QList<Playlist>& playlists) {}

void PlaylistSql::updatePlaylist(const QList<Playlist>& playlists) {}

void PlaylistSql::loadPlaylist() {}

PlaylistSql::PlaylistSql()
    : m_database(
          QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), SQL_DB_CONN)),
      m_memDatabase(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"),
                                              SQL_MEM_DB_CONN)) {
  m_database.setDatabaseName(SQL_DB_NAME);
  m_memDatabase.setDatabaseName(SQL_MEM_DB_NAME);
}

PlaylistSql::~PlaylistSql() {}
