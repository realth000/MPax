#include "playlistsql.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#define SQL_DB_CONN "playlist_db_connection"
#define SQL_DB_NAME QCoreApplication::applicationDirPath() + "/playlist.db"
#define SQL_MEM_DB_CONN "playlist_mem_db_connection"
#define SQL_MEM_DB_NAME "playlist_mem_db"
#define SQL_MASTER_TABLE_NAME "playlist_info_table"

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
  m_database.transaction();
  QSqlQuery query(m_database);
  bool ok = query.exec(QString("DELETE FROM %1").arg(SQL_MASTER_TABLE_NAME));
  if (!ok) {
    m_database.rollback();
    qDebug() << "can not erase old playlist info" << query.lastError();
    tryCloseDatabase();
    return;
  }
  for (const auto& playlist : playlists) {
    // Insert playlist info.
    const PlaylistInfo* info = playlist.info();
    query.prepare(QString("REPLACE INTO %1(id, name) VALUES(:v_id, :v_name);")
                      .arg(SQL_MASTER_TABLE_NAME));
    query.bindValue(QStringLiteral(":v_id"), playlistCount);
    query.bindValue(QStringLiteral(":v_name"), info->info(PLAYLIST_INFO_NAME));
    ok = query.exec();
    if (!ok) {
      m_database.rollback();
      qDebug() << "can not save playlist info:"
               << info->info(PLAYLIST_INFO_NAME) << ":" << query.lastError();
      goto exit;
    }
    // Drop old playlist data.
    query.prepare(QString("DROP TABLE IF EXISTS playlist_%1_table")
                      .arg(QString::number(playlistCount)));
    ok = query.exec();
    if (!ok) {
      m_database.rollback();
      qDebug() << "can not erase old playlist:"
               << info->info(PLAYLIST_INFO_NAME) << ":" << query.lastError();
      goto exit;
    }
    // Create playlist table.
    query.prepare(QString("CREATE TABLE playlist_%1_table(id INT "
                          "PRIMARY KEY, path TEXT);")
                      .arg(QString::number(playlistCount)));
    ok = query.exec();
    if (!ok) {
      m_database.rollback();
      qDebug() << "can not create playlist table:"
               << info->info(PLAYLIST_INFO_NAME) << ":" << query.lastError();
      goto exit;
    }
    // Insert playlist data.
    int contentCount = 0;
    for (auto c : *playlist.content()) {
      query.prepare(QString("INSERT INTO playlist_%1_table(id  "
                            ", path) VALUES(:v_id, :v_path);")
                        .arg(QString::number(playlistCount)));
      query.bindValue(QStringLiteral(":v_id"), contentCount);
      query.bindValue(QStringLiteral(":v_path"), c->value("ContentPath"));
      ok = query.exec();
      if (!ok) {
        m_database.rollback();
        qDebug() << "can not save playlist data:"
                 << info->info(PLAYLIST_INFO_NAME) << ":" << query.lastError();
        goto exit;
      }
      contentCount++;
    }
    playlistCount++;
  }
  m_database.commit();
exit:
  tryCloseDatabase();
}

void PlaylistSql::updatePlaylist(const QList<Playlist>& playlists) {}

QList<Playlist> PlaylistSql::loadPlaylist() {
  if (!tryOpenDatabase()) {
    qDebug() << "can not load playlist, database failed to open";
    return QList<Playlist>{};
  }
  QSqlQuery query(m_database);
  bool ok = query.exec(QString("SELECT * FROM %1").arg(SQL_MASTER_TABLE_NAME));
  if (!ok) {
    qDebug() << "can not read playlist info";
    tryCloseDatabase();
    return QList<Playlist>{};
  }
  QList<Playlist> allList;
  while (query.next()) {
    PlaylistInfo* info = new PlaylistInfo;
    PlayContentList* list = new PlayContentList;

    int id = query.value("id").toInt();
    QString name = query.value("name").toString();
    QSqlQuery q(m_database);
    ok = q.exec(QString("SELECT * FROM playlist_%1_table").arg(id));
    if (!ok) {
      qDebug() << "can not load playlist data" << name << ":" << q.lastError();
      tryCloseDatabase();
      return QList<Playlist>{};
    }
    info->setInfo(PLAYLIST_INFO_NAME, name);

    while (q.next()) {
      //      qDebug() << q.value("path").toString();
      list->append(new PlayContent(q.value("path").toString()));
    }
    allList.append(Playlist(info, list));
  }
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
  query.prepare(QString("CREATE TABLE IF NOT EXISTS %1(id INT PRIMARY KEY, "
                        "name TEXT, table_name TEXT)")
                    .arg(SQL_MASTER_TABLE_NAME));
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
