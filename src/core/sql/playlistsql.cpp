#include "playlistsql.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "config/appconfig.h"

#define SQL_RANDOM_NAME(VAL)                                   \
  QString::number(QDateTime::currentMSecsSinceEpoch()) + "_" + \
      QString::number(VAL)

#define SQL_DB_CONN "playlist_db_connection"
#define SQL_MEM_DB_CONN "playlist_mem_db_connection"
#define SQL_MEM_DB_NAME "playlist_mem_db"
#define SQL_MASTER_TABLE_NAME "playlist_info_table"

// Sql commands
#define SQL_INIT_INFO_TABLE                                                  \
  QString(                                                                   \
      "CREATE TABLE IF NOT EXISTS %1(id INT NOT NULL PRIMARY KEY,sort INT, " \
      "playlist_name TEXT,table_name TEXT);")                                \
      .arg(SQL_MASTER_TABLE_NAME)

#define SQL_CLEAR_PLAYLIST_INFO \
  QString("DELETE FROM %1;").arg(SQL_MASTER_TABLE_NAME)

#define SQL_SAVE_INFO_TABLE                                                 \
  QString(                                                                  \
      "REPLACE INTO %1(id, sort, playlist_name, table_name) VALUES(:v_id, " \
      ":v_sort, :v_playlist_name, :v_table_name);")                         \
      .arg(SQL_MASTER_TABLE_NAME)

#define SQL_INIT_PLAYLIST_TABLE                                             \
  "CREATE TABLE %1(id INT NOT NULL PRIMARY KEY, path TEXT, title TEXT, "    \
  "artist TEXT, album_title TEXT, "                                         \
  "album_artist TEXT, album_year INT, album_track_count INT, track_number " \
  "INT, bit_rate INT, sample_rate INT, genre TEXT, comment "                \
  "TEXT, channels INT, length INT);"

#define SQL_SAVE_PLAYLIST_TABLE                                    \
  "INSERT INTO %1(id  "                                            \
  ", path, title, artist, album_title, album_artist, album_year, " \
  "album_track_count, track_number, bit_rate, sample_rate, "       \
  "genre, comment, channels, length) "                             \
  "VALUES(:v_id, :v_path, :v_title, :v_artist, "                   \
  ":v_album_title, :v_album_artist, :v_album_year, "               \
  ":v_album_track_count, :v_track_number, :v_bit_rate, "           \
  ":v_sample_rate, :v_genre, :v_comment, :v_channels, :v_length);"

#define SQL_UPDATE_PLAYLIST_ROW                                             \
  "UPDATE %1 SET title=:v_title, artist=:v_artist, "                        \
  "album_title=:v_album_title, album_artist=:v_album_artist, "              \
  "album_year=:v_album_year, album_track_count=:v_album_track_count, "      \
  "track_number=:v_track_number, genre=:v_genre, comment=:v_comment WHERE " \
  "path=:v_path"

PlaylistSql* PlaylistSql::getInstance() {
  static PlaylistSql ps;
  return &ps;
}

void PlaylistSql::savePlaylist(const QList<Playlist>& playlists) {
  if (!tryOpenDatabase()) {
    qDebug() << "can not save playlist, database failed to open";
    return;
  }
  int playlistCount = 0;
  QStringList tableNames;
  m_database.transaction();
  QSqlQuery query(m_database);
  // Delete old playlist data.
  query.prepare(
      QString("SELECT table_name FROM %1").arg(SQL_MASTER_TABLE_NAME));
  bool ok = query.exec();
  if (!ok) {
    m_database.rollback();
    qDebug() << "can not get table name data" << query.lastError();
    goto exit;
  }
  while (query.next()) {
    tableNames.append(query.value("table_name").toString());
  }
  for (const auto& t : tableNames) {
    ok = query.exec(QString("DROP TABLE %1").arg(t));
    if (!ok) {
      m_database.rollback();
      qDebug() << "can not delete old playlist table" << query.lastError();
      goto exit;
    }
  }
  ok = query.exec(SQL_CLEAR_PLAYLIST_INFO);
  if (!ok) {
    m_database.rollback();
    qDebug() << "can not erase old playlist info" << query.lastError();
    goto exit;
  }
  for (const auto& playlist : playlists) {
    // Insert playlist info.
    const PlaylistInfo info = playlist.info();
    const QString tableName =
        QString("playlist_%1_table").arg(SQL_RANDOM_NAME(playlistCount));
    const QString playlistName = info.info(PLAYLIST_INFO_NAME);
    query.prepare(SQL_SAVE_INFO_TABLE);
    query.bindValue(QStringLiteral(":v_id"), playlistCount);
    query.bindValue(QStringLiteral(":v_sort"), playlistCount);
    query.bindValue(QStringLiteral(":v_playlist_name"), playlistName);
    query.bindValue(QStringLiteral(":v_table_name"), tableName);

    ok = query.exec();
    if (!ok) {
      m_database.rollback();
      qDebug() << "can not save playlist info:" << playlistName << ":"
               << query.lastError() << query.lastQuery();
      goto exit;
    }
    // Create playlist table.
#if 1
    ok = prepareSql(&query, nullptr, tableName, SqlAction::Create,
                    QStringList{});
    if (!ok) {
      m_database.rollback();
      qDebug() << "can not create playlist table: failed to prepare sql";
      goto exit;
    }
#else
    query.prepare(QString(SQL_INIT_PLAYLIST_TABLE).arg(tableName));
#endif
    ok = query.exec();
    if (!ok) {
      m_database.rollback();
      qDebug() << "can not create playlist table:" << playlistName << ":"
               << query.lastError();
      goto exit;
    }
    // Insert playlist data.
    int contentCount = 0;
    for (auto c : playlist.content()) {
#if 1
      ok =
          prepareSql(&query, c, tableName, SqlAction::Insert,
                     QStringList{"ContentPath", "Title", "Artist", "AlbumTitle",
                                 "AlbumArtist", "AlbumYear", "AlbumTrackCount",
                                 "TrackNumber", "BitRate", "SampleRate",
                                 "Genre", "Comment", "Channels", "Length"},
                     contentCount);
      if (!ok) {
        m_database.rollback();
        qDebug() << "can not create playlist table: failed to prepare sql";
        goto exit;
      }
#else
      query.prepare(QString(SQL_SAVE_PLAYLIST_TABLE).arg(tableName));
      query.bindValue(QStringLiteral(":v_id"), contentCount);
      query.bindValue(QStringLiteral(":v_path"), c->value("ContentPath"));
      query.bindValue(QStringLiteral(":v_title"), c->value("Title"));
      query.bindValue(QStringLiteral(":v_artist"), c->value("Artist"));
      query.bindValue(QStringLiteral(":v_album_title"), c->value("AlbumTitle"));
      query.bindValue(QStringLiteral(":v_album_artist"),
                      c->value("AlbumArtist"));
      query.bindValue(QStringLiteral(":v_album_year"), c->value("AlbumYear"));
      query.bindValue(QStringLiteral(":v_album_track_count"),
                      c->value("AlbumTrackCount"));
      query.bindValue(QStringLiteral(":v_track_number"),
                      c->value("TrackNumber"));
      query.bindValue(QStringLiteral(":v_bit_rate"), c->value("BitRate"));
      query.bindValue(QStringLiteral(":v_sample_rate"), c->value("SampleRate"));
      query.bindValue(QStringLiteral(":v_genre"), c->value("Genre"));
      query.bindValue(QStringLiteral(":v_comment"), c->value("Comment"));
      query.bindValue(QStringLiteral(":v_channels"), c->value("Channels"));
      query.bindValue(QStringLiteral(":v_length"), c->value("Length"));
#endif
      ok = query.exec();
      if (!ok) {
        m_database.rollback();
        qDebug() << "can not save playlist data:" << playlistName << ":"
                 << query.lastError();
        goto exit;
      }
      contentCount++;
    }
    playlistCount++;
    playlist.info().setInfo(PLAYLIST_INFO_TABLE_NAME, tableName);
  }
  m_database.commit();
exit:
  tryCloseDatabase();
}

void PlaylistSql::savePlaylist(const QString& dbPath) {
  QFile::copy(SQL_DB_NAME, dbPath);
}

void PlaylistSql::removePlaylist(const Playlist& playlist) {
  const QString tableName = playlist.info().info(PLAYLIST_INFO_TABLE_NAME);
  const QString playlistName = playlist.info().info(PLAYLIST_INFO_NAME);
  if (!tryOpenDatabase()) {
    qDebug() << "database not open, failed to delete" << playlistName;
    return;
  }
  m_database.transaction();
  QSqlQuery query(m_database);
  // Delete playlist info.
  query.prepare(QString("DELETE FROM %1 WHERE table_name = :v_table_name")
                    .arg(SQL_MASTER_TABLE_NAME));
  query.bindValue(":v_table_name", tableName);
  bool ok = query.exec();
  if (!ok) {
    m_database.rollback();
    qDebug() << "can not delete playlist info for " << playlistName
             << query.lastError();
    goto exit;
  }
  // Delete playlist.
  ok = query.exec(QString("DROP TABLE %1").arg(tableName));
  if (!ok) {
    m_database.rollback();
    qDebug() << "can not delete playlist data for" << playlistName
             << query.lastError();
    goto exit;
  }
  m_database.commit();
exit:
  tryCloseDatabase();
}

void PlaylistSql::updatePlaylist(Playlist* playlist) {
  int contentCount = 0;
  const QString tableName = playlist->info().info(PLAYLIST_INFO_TABLE_NAME);
  const QString playlistName = playlist->info().info(PLAYLIST_INFO_NAME);
  const QStringList headerList =
      QStringList{"ContentPath", "Title",      "Artist",          "AlbumTitle",
                  "AlbumArtist", "AlbumYear",  "AlbumTrackCount", "TrackNumber",
                  "BitRate",     "SampleRate", "Genre",           "Comment",
                  "Channels",    "Length"};
  qDebug() << "update playlist" << tableName << playlistName;
  if (!tryOpenDatabase()) {
    qDebug() << "database not open, failed to update" << playlistName;
    return;
  }
  m_database.transaction();
  QSqlQuery query(m_database);
  bool ok = query.exec(QString("DELETE FROM %1").arg(tableName));
  if (!ok) {
    qDebug() << "can not clear playlist data" << playlistName
             << query.lastError();
    m_database.rollback();
    goto exit;
  }

  for (auto c : playlist->content()) {
#if 1
    ok = prepareSql(&query, c, tableName, SqlAction::Insert, headerList,
                    contentCount);
    if (!ok) {
      m_database.rollback();
      qDebug() << "can not create playlist table: failed to prepare sql";
      goto exit;
    }
#else
    query.prepare(QString(SQL_SAVE_PLAYLIST_TABLE).arg(tableName));
    query.bindValue(QStringLiteral(":v_id"), contentCount);
    query.bindValue(QStringLiteral(":v_path"), c->value("ContentPath"));
    query.bindValue(QStringLiteral(":v_title"), c->value("Title"));
    query.bindValue(QStringLiteral(":v_artist"), c->value("Artist"));
    query.bindValue(QStringLiteral(":v_album_title"), c->value("AlbumTitle"));
    query.bindValue(QStringLiteral(":v_album_artist"), c->value("AlbumArtist"));
    query.bindValue(QStringLiteral(":v_album_year"), c->value("AlbumYear"));
    query.bindValue(QStringLiteral(":v_album_track_count"),
                    c->value("AlbumTrackCount"));
    query.bindValue(QStringLiteral(":v_track_count"), c->value("TrackCount"));
    query.bindValue(QStringLiteral(":v_bit_rate"), c->value("BitRate"));
    query.bindValue(QStringLiteral(":v_sample_rate"), c->value("SampleRate"));
    query.bindValue(QStringLiteral(":v_genre"), c->value("Genre"));
    query.bindValue(QStringLiteral(":v_comment"), c->value("Comment"));
    query.bindValue(QStringLiteral(":v_channels"), c->value("Channels"));
    query.bindValue(QStringLiteral(":v_length"), c->value("Length"));
#endif
    ok = query.exec();
    if (!ok) {
      m_database.rollback();
      qDebug() << "can not update playlist data:" << playlistName << ":"
               << query.lastError();
      goto exit;
    }
    contentCount++;
  }
  m_database.commit();
  playlist->info().setInfo(PLAYLIST_INFO_COUNT, QString::number(contentCount));
exit:
  tryCloseDatabase();
}

QList<Playlist> PlaylistSql::loadPlaylist() {
  QList<Playlist> allList;
  // Load playlist sort from config
  const QString sortHeaderName =
      m_titleMap[Config::AppConfig::getInstance()
                     ->config(CONFIG_PLAYLIST_SORT_HEADER)
                     .value.toString()]
          .name;
  const int sortOrder = Config::AppConfig::getInstance()
                            ->config(CONFIG_PLAYLIST_SORT_ORDER)
                            .value.toInt();
  if (!tryOpenDatabase()) {
    qDebug() << "can not load playlist, database failed to open";
    return QList<Playlist>{};
  }
  QSqlQuery query(m_database);
  bool ok = query.exec(QString("SELECT * FROM %1").arg(SQL_MASTER_TABLE_NAME));
  if (!ok) {
    qDebug() << "can not read playlist info" << query.lastError();
    goto exit;
  }
  while (query.next()) {
    PlaylistInfo* info = new PlaylistInfo;
    PlayContentList* list = new PlayContentList;
    int count = 0;

    int id = query.value("id").toInt();
    const QString tableName = query.value("table_name").toString();
    const QString playlistName = query.value("playlist_name").toString();
    QSqlQuery q(m_database);
    if (!sortHeaderName.isEmpty()) {
      ok = q.exec(QString("SELECT * FROM %1 ORDER BY %2 %3")
                      .arg(tableName, sortHeaderName,
                           sortOrder == Qt::DescendingOrder ? "DESC" : "ASC"));
    } else {
      ok = q.exec(QString("SELECT * FROM %1").arg(tableName));
    }

    if (!ok) {
      qDebug() << "can not load playlist" << playlistName << ":"
               << q.lastError();
      goto exit;
    }
    info->setInfo(PLAYLIST_INFO_NAME, playlistName);
    info->setInfo(PLAYLIST_INFO_TABLE_NAME, tableName);
    while (q.next()) {
      //      qDebug() << q.value("path").toString();
      PlayContent* playContent = new PlayContent(q.value("path").toString());
      playContent->title = q.value("title").toString();
      playContent->artist = q.value("artist").toString();
      playContent->albumTitle = q.value("album_title").toString();
      playContent->albumArtist = q.value("album_artist").toString();
      playContent->albumYear = q.value("album_year").toInt();
      playContent->albumTrackCount = q.value("album_track_count").toInt();
      playContent->trackNumber = q.value("track_number").toInt();
      playContent->bitRate = q.value("bit_rate").toInt();
      playContent->sampleRate = q.value("sample_rate").toInt();
      playContent->genre = q.value("genre").toString();
      playContent->comment = q.value("comment").toString();
      playContent->channels = q.value("channels").toInt();
      playContent->length = q.value("length").toInt();
      list->append(playContent);
      count++;
    }
    info->setInfo(PLAYLIST_INFO_COUNT, QString::number(count));
    allList.append(Playlist(info, list));
  }
exit:
  tryCloseDatabase();
  return allList;
}

PlaylistSql::PlaylistSql()
    : m_database(
          QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), SQL_DB_CONN)) {
#if 1
  m_titleMap.insert("ContentPath", ColumnHeader{"path", "TEXT"});
  m_titleMap.insert("Title", ColumnHeader{"title", "TEXT"});
  m_titleMap.insert("Artist", ColumnHeader{"artist", "TEXT"});
  m_titleMap.insert("AlbumTitle", ColumnHeader{"album_title", "TEXT"});
  m_titleMap.insert("AlbumArtist", ColumnHeader{"album_artist", "TEXT"});
  m_titleMap.insert("AlbumYear", ColumnHeader{"album_year", "INT"});
  m_titleMap.insert("AlbumTrackCount",
                    ColumnHeader{"album_track_count", "INT"});
  m_titleMap.insert("TrackNumber", ColumnHeader{"track_number", "INT"});
  m_titleMap.insert("BitRate", ColumnHeader{"bit_rate", "INT"});
  m_titleMap.insert("SampleRate", ColumnHeader{"sample_rate", "INT"});
  m_titleMap.insert("Genre", ColumnHeader{"genre", "TEXT"});
  m_titleMap.insert("Comment", ColumnHeader{"comment", "TEXT"});
  m_titleMap.insert("Channels", ColumnHeader{"channels", "INT"});
  m_titleMap.insert("Length", ColumnHeader{"length", "INT"});
#else
    m_titleMap(
          QMap<QString, QString>{{"ContentPath", "path"},
        {"Title", "title"},
        {"Artist", "artist"},
        {"AlbumTitle", "album_title"},
        {"AlbumArtist", "album_artist"},
        {"AlbumYear", "album_year"},
        {"AlbumTrackCount", "album_track_count"},
        {"TrackNumber", "track_number"},
        {"BitRate", "bit_rate"},
        {"SampleRate", "sample_rate"},
        {"Genre", "genre"},
        {"Comment", "comment"},
        {"Channels", "channels"},
    {"Length", "length"}};
#endif
  m_database.setDatabaseName(SQL_DB_NAME);
  if (!tryOpenDatabase()) {
    qDebug() << "can not open playlist database" << SQL_DB_NAME;
    return;
  }
  QSqlQuery query(m_database);
  // Init playlist info table.
  query.prepare(SQL_INIT_INFO_TABLE);
  bool ok = query.exec();
  QSqlError error = query.lastError();
  if (!ok) {
    qDebug() << "valid error" << error.type() << error.text();
  }
  tryCloseDatabase();
}

PlaylistSql::~PlaylistSql() { tryCloseDatabase(); }

bool PlaylistSql::tryOpenDatabase() {
  if (m_database.isOpen() || m_database.open()) {
    return true;
  }
  return false;
}

void PlaylistSql::tryCloseDatabase() {
  if (m_database.open()) {
    m_database.close();
  }
}

bool PlaylistSql::loadPlaylistWithOrder(Playlist* playlist,
                                        const QString& columnName,
                                        Qt::SortOrder order) {
  if (!tryOpenDatabase()) {
    qDebug() << "can not load playlist with order, database failed to open";
    return false;
  }
  int count = 0;
  const QString playlistName = playlist->info().info(PLAYLIST_INFO_NAME);
  if (playlistName.isEmpty()) {
    qDebug() << "can not load playlist with order, empty playlist name";
    return false;
  }
  PlayContentList* list = new PlayContentList;
  const QString tableName = playlist->info().info(PLAYLIST_INFO_TABLE_NAME);
  bool ok;
  QSqlQuery q(m_database);
  const QString sqlColumnName = m_titleMap[columnName].name;
  if (sqlColumnName.isEmpty()) {
    goto exit;
  }
  if (tableName.isEmpty()) {
    qDebug() << "can not load playlist with order for a null table name";
    goto exit;
  }
  ok = q.exec(QString("SELECT * FROM %1 ORDER BY %2 %3")
                  .arg(tableName, sqlColumnName,
                       order == Qt::DescendingOrder ? "DESC" : "ASC"));
  if (!ok) {
    qDebug() << "can not load playlist with order" << playlistName << ":"
             << q.lastError() << tableName << sqlColumnName;
    goto exit;
  }
  while (q.next()) {
    PlayContent* playContent = new PlayContent(q.value("path").toString());
    playContent->title = q.value("title").toString();
    playContent->artist = q.value("artist").toString();
    playContent->albumTitle = q.value("album_title").toString();
    playContent->albumArtist = q.value("album_artist").toString();
    playContent->albumYear = q.value("album_year").toInt();
    playContent->albumTrackCount = q.value("album_track_count").toInt();
    playContent->trackNumber = q.value("track_number").toInt();
    playContent->bitRate = q.value("bit_rate").toInt();
    playContent->sampleRate = q.value("sample_rate").toInt();
    playContent->genre = q.value("genre").toString();
    playContent->comment = q.value("comment").toString();
    playContent->channels = q.value("channels").toInt();
    playContent->length = q.value("length").toInt();
    list->append(playContent);
    count++;
  }
  playlist->setInfo(PLAYLIST_INFO_COUNT, QString::number(count));
  playlist->setContent(list);
  return true;

exit:
  tryCloseDatabase();
  return false;
}

void PlaylistSql::updatePlayContent(const Playlist* playlist,
                                    const PlayContent* playContent) {
  if (playlist == nullptr || playContent == nullptr) {
    qDebug() << __FUNCTION__ << " failed, null playlist or null play content";
    return;
  }

  if (!playlist->contains(playContent->contentPath)) {
    qDebug() << __FUNCTION__ << " failed, playlist not contains play content";
    return;
  }
  if (!tryOpenDatabase()) {
    qDebug() << __FUNCTION__ << " failed, can not open database";
    return;
  }
  const QString tableName = playlist->info().info(PLAYLIST_INFO_TABLE_NAME);
  if (tableName.isEmpty()) {
    qDebug() << __FUNCTION__ << " failed, can not find playlist table name";
    return;
  }
  m_database.transaction();
  QSqlQuery query(m_database);
  bool ok = false;
#if 1
  ok = prepareSql(&query, playContent, tableName, SqlAction::Update,
                  QStringList{"Title", "Artist", "AlbumTitle", "AlbumArtist",
                              "AlbumYear", "AlbumTrackCount", "TrackNumber",
                              "Genre", "Comment", "ContentPath"});
  if (!ok) {
    m_database.rollback();
    qDebug() << __FUNCTION__ << " failed, can not prepare sql statement";
    goto exit;
  }
#else
  query.prepare(QString(SQL_UPDATE_PLAYLIST_ROW).arg(tableName));
  query.bindValue(QStringLiteral(":v_title"), playContent->value("Title"));
  query.bindValue(QStringLiteral(":v_artist"), playContent->value("Artist"));
  query.bindValue(QStringLiteral(":v_album_title"),
                  playContent->value("AlbumTitle"));
  query.bindValue(QStringLiteral(":v_album_artist"),
                  playContent->value("AlbumArtist"));
  query.bindValue(QStringLiteral(":v_album_year"),
                  playContent->value("AlbumYear"));
  query.bindValue(QStringLiteral(":v_album_track_count"),
                  playContent->value("AlbumTrackCount"));
  query.bindValue(QStringLiteral(":v_track_number"),
                  playContent->value("TrackNumber"));
  query.bindValue(QStringLiteral(":v_genre"), playContent->value("Genre"));
  query.bindValue(QStringLiteral(":v_comment"), playContent->value("Comment"));
  query.bindValue(QStringLiteral(":v_path"), playContent->value("ContentPath"));
#endif
  ok = query.exec();
  if (!ok) {
    m_database.rollback();
    qDebug() << "failed to update" << query.lastError() << query.lastQuery();
    goto exit;
  }
  m_database.commit();

exit:
  tryCloseDatabase();
}

bool PlaylistSql::prepareSql(QSqlQuery* query, const PlayContent* playContent,
                             const QString& tableName, SqlAction action,
                             const QStringList& columnList, int id) {
  if (query == nullptr) {
    qDebug() << "failed to generate sql: null query";
    return false;
  }
  if (action != SqlAction::Create && playContent == nullptr) {
    qDebug() << "failed to generate sql: null play content";
    return false;
  }
  switch (action) {
    case SqlAction::Create:
    case SqlAction::Insert:
    case SqlAction::Update:
      break;
    default:
      qDebug() << "failed to generate sql: unknown sql action " << action;
      return false;
  }

  QString queryStatement;

  switch (action) {
    case SqlAction::Create: {
      QString columnPart;
      for (auto it = m_titleMap.cbegin(); it != m_titleMap.cend(); it++) {
        columnPart +=
            QString("%1 %2 %3, ")
                .arg(it.value().name, it.value().type, it.value().properties);
      }
      columnPart.chop(2);
      queryStatement =
          QString("CREATE TABLE %1(id INT NOT NULL PRIMARY KEY, %2);")
              .arg(tableName, columnPart);
      break;
    }
    case SqlAction::Insert: {
      QString columnNamesPart;
      QString columnValuePart;
      for (auto& column : columnList) {
        if (!m_titleMap.contains(column)) {
          qDebug() << "failed to generate sql: unknown column title " << column;
          return false;
        }
        columnNamesPart += m_titleMap.value(column).name + ", ";
        columnValuePart += ":v_" + m_titleMap.value(column).name + ", ";
      }
      if (id >= 0) {
        columnNamesPart += "id, ";
        columnValuePart += ":v_id, ";
      }
      columnNamesPart.chop(2);
      columnValuePart.chop(2);
      queryStatement = QString("INSERT INTO %1(%2) VALUES(%3);")
                           .arg(tableName, columnNamesPart, columnValuePart);
      break;
    }
    case SqlAction::Update: {
      QString columnPart;
      for (auto& column : columnList) {
        if (!m_titleMap.contains(column)) {
          qDebug() << "failed to generate sql: unknown column title " << column;
          return false;
        }
        columnPart += m_titleMap.value(column).name + "=:v_" +
                      m_titleMap.value(column).name + ", ";
      }
      if (id >= 0) {
        columnPart += "id=:v_id, ";
      }
      columnPart.chop(2);
      queryStatement = QString("UPDATE %1 SET %2 WHERE path=:v_path;")
                           .arg(tableName, columnPart);
      break;
    }
    default:
      qDebug() << "failed to generate sql: unknown sql action " << action;
      return false;
  }

  query->prepare(queryStatement);
  if (action != SqlAction::Create) {
    for (auto& column : columnList) {
      query->bindValue(":v_" + m_titleMap.value(column).name,
                       playContent->value(column));
    }
  }

  if (id >= 0) {
    query->bindValue(":v_id", id);
  }

  // Update action should restrict row.
  // Current is by file path.
  if (action == SqlAction::Update) {
    query->bindValue(":v_path", playContent->value("ContentPath"));
  }

  return true;
}
