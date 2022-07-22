#include "playlistsql.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QRandomGenerator>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

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

#define SQL_SAVE_PLAYLIST                                                   \
  QString(                                                                  \
      "REPLACE INTO %1(id, sort, playlist_name, table_name) VALUES(:v_id, " \
      ":v_sort, :v_playlist_name, :v_table_name);")                         \
      .arg(SQL_MASTER_TABLE_NAME)

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
  QVector<QPair<QString, QString>> nameVectorBackup = m_nameVector;
  m_nameVector.clear();
  m_database.transaction();
  QSqlQuery query(m_database);
  // Delete old playlist data.
  query.prepare(
      QString("SELECT table_name FROM %1").arg(SQL_MASTER_TABLE_NAME));
  bool ok = query.exec();
  if (!ok) {
    m_database.rollback();
    m_nameVector = nameVectorBackup;
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
      m_nameVector = nameVectorBackup;
      qDebug() << "can not delete old playlist table" << query.lastError();
      goto exit;
    }
  }
  ok = query.exec(SQL_CLEAR_PLAYLIST_INFO);
  if (!ok) {
    m_database.rollback();
    m_nameVector = nameVectorBackup;
    qDebug() << "can not erase old playlist info" << query.lastError();
    goto exit;
  }
  for (const auto& playlist : playlists) {
    // Insert playlist info.
    const PlaylistInfo* info = playlist.info();
    const QString tableName =
        QString("playlist_%1_table").arg(SQL_RANDOM_NAME(playlistCount));
    const QString playlistName = info->info(PLAYLIST_INFO_NAME);
    query.prepare(SQL_SAVE_PLAYLIST);
    query.bindValue(QStringLiteral(":v_id"), playlistCount);
    query.bindValue(QStringLiteral(":v_sort"), playlistCount);
    query.bindValue(QStringLiteral(":v_playlist_name"), playlistName);
    query.bindValue(QStringLiteral(":v_table_name"), tableName);

    ok = query.exec();
    if (!ok) {
      m_database.rollback();
      m_nameVector = nameVectorBackup;
      qDebug() << "can not save playlist info:" << playlistName << ":"
               << query.lastError() << query.lastQuery();
      goto exit;
    }
    // Create playlist table.
    query.prepare(QString("CREATE TABLE %1(id INT NOT NULL "
                          "PRIMARY KEY, path TEXT, title TEXT, artist "
                          "TEXT, album_title TEXT);")
                      .arg(tableName));
    ok = query.exec();
    if (!ok) {
      m_database.rollback();
      m_nameVector = nameVectorBackup;
      qDebug() << "can not create playlist table:" << playlistName << ":"
               << query.lastError();
      goto exit;
    }
    // Insert playlist data.
    int contentCount = 0;
    for (auto c : *playlist.content()) {
      query.prepare(QString("INSERT INTO %1(id  "
                            ", path, title, artist, album_title) "
                            "VALUES(:v_id, :v_path, :v_title, :v_artist, "
                            ":v_album_title);")
                        .arg(tableName));
      query.bindValue(QStringLiteral(":v_id"), contentCount);
      query.bindValue(QStringLiteral(":v_path"), c->value("ContentPath"));
      query.bindValue(QStringLiteral(":v_title"), c->value("Title"));
      query.bindValue(QStringLiteral(":v_artist"), c->value("Artist"));
      query.bindValue(QStringLiteral(":v_album_title"), c->value("AlbumTitle"));
      ok = query.exec();
      if (!ok) {
        m_database.rollback();
        m_nameVector = nameVectorBackup;
        qDebug() << "can not save playlist data:" << playlistName << ":"
                 << query.lastError();
        goto exit;
      }
      contentCount++;
    }
    m_nameVector.append(QPair<QString, QString>(tableName, playlistName));
    playlistCount++;
  }
  m_database.commit();
exit:
  tryCloseDatabase();
}

void PlaylistSql::savePlaylist(const QString& dbPath) {
  QFile::copy(SQL_DB_NAME, dbPath);
}

void PlaylistSql::removePlaylist(const int& index) {
  if (m_nameVector.length() <= index) {
    qDebug() << "playlist index intended to delete is out of range";
    return;
  }
  const QString tableName = m_nameVector[index].first;
  const QString playlistName = m_nameVector[index].second;
  if (!tryOpenDatabase()) {
    qDebug() << "database not open, failed to delete" << playlistName;
    return;
  }
  QVector<QPair<QString, QString>> nameVectorBackup = m_nameVector;
  m_database.transaction();
  QSqlQuery query(m_database);
  // Delete playlist info.
  query.prepare(QString("DELETE FROM %1 WHERE table_name = :v_table_name")
                    .arg(SQL_MASTER_TABLE_NAME));
  query.bindValue(":v_table_name", tableName);
  bool ok = query.exec();
  if (!ok) {
    m_database.rollback();
    m_nameVector = nameVectorBackup;
    qDebug() << "can not delete playlist info for " << playlistName
             << query.lastError();
    goto exit;
  }
  // Delete playlist.
  ok = query.exec(QString("DROP TABLE %1").arg(tableName));
  if (!ok) {
    m_database.rollback();
    m_nameVector = nameVectorBackup;
    qDebug() << "can not delete playlist data for" << playlistName
             << query.lastError();
    goto exit;
  }
  m_database.commit();
  m_nameVector.removeAt(index);
exit:
  tryCloseDatabase();
}

void PlaylistSql::updatePlaylist(const int& index, const Playlist& playlist) {
  if (m_nameVector.length() <= index) {
    qDebug() << "update Playlist index out of range" << index
             << m_nameVector.length();
    return;
  }
  int contentCount = 0;
  const QString tableName = m_nameVector[index].first;
  const QString playlistName = m_nameVector[index].second;
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
  for (auto c : *playlist.content()) {
    query.prepare(QString("INSERT INTO %1(id  "
                          ", path, title, artist, album_title) "
                          "VALUES(:v_id, :v_path, :v_title, :v_artist, "
                          ":v_album_title);")
                      .arg(tableName));
    query.bindValue(QStringLiteral(":v_id"), contentCount);
    query.bindValue(QStringLiteral(":v_path"), c->value("ContentPath"));
    query.bindValue(QStringLiteral(":v_title"), c->value("Title"));
    query.bindValue(QStringLiteral(":v_artist"), c->value("Artist"));
    query.bindValue(QStringLiteral(":v_album_title"), c->value("AlbumTitle"));
    ok = query.exec();
    if (!ok) {
      m_database.rollback();
      qDebug() << "can not update playlist data:" << playlistName << ":"
               << query.lastError();
      goto exit;
    }
    contentCount++;
  }
exit:
  tryCloseDatabase();
}

QList<Playlist> PlaylistSql::loadPlaylist() {
  QList<Playlist> allList;
  QVector<QPair<QString, QString>> nameVectorBackup = m_nameVector;
  m_nameVector.clear();
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

    int id = query.value("id").toInt();
    const QString tableName = query.value("table_name").toString();
    const QString playlistName = query.value("playlist_name").toString();
    QSqlQuery q(m_database);
    ok = q.exec(QString("SELECT * FROM %1").arg(tableName));
    if (!ok) {
      qDebug() << "can not load playlist" << playlistName << ":"
               << q.lastError();
      m_nameVector = nameVectorBackup;
      goto exit;
    }
    info->setInfo(PLAYLIST_INFO_NAME, playlistName);
    m_nameVector.append(QPair<QString, QString>(tableName, playlistName));

    while (q.next()) {
      //      qDebug() << q.value("path").toString();
      PlayContent* playContent = new PlayContent(q.value("path").toString());
      playContent->title = q.value("title").toString();
      playContent->artist = q.value("artist").toString();
      playContent->albumTitle = q.value("album_title").toString();
      list->append(playContent);
    }
    allList.append(Playlist(info, list));
  }
exit:
  tryCloseDatabase();
  return allList;
}

PlaylistSql::PlaylistSql()
    : m_database(
          QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), SQL_DB_CONN)) {
  m_database.setDatabaseName(SQL_DB_NAME);
  if (!tryOpenDatabase()) {
    qDebug() << "can not open playlist database" << SQL_DB_NAME;
    exit(1);
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
