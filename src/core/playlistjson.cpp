#include "playlistjson.h"

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QTextStream>

/*
 * {
 *  "type": "playlist",
 *  "format_version": 1,
 *    "count": 123,
 *    "save_time": "yyyy-MM-dd HH:mm:ss",
 *    "data": {
 *      "0": {
 *        "play_list_info": {
 *          "name": "play_list_name1",
 *          "count": 100
 *        },
 *        "play_list_data": [
 *          "/path1/1",
 *          "/path1/2",
 *          ...,
 *          "/path1/100",
 *        ]
 *      },
 *      "1": {
 *        "play_list_info": {
 *          "name": "play_list_name2",
 *          "count": 23
 *        },
 *        "play_list_data": [
 *          "/path2/1",
 *          "/path2/2",
 *          ...,
 *          "/path2/23",
 *        ]
 *      }
 *    }
 * }
 */

#define KEY_TYPE "type"
#define VAL_TYPE_PLAYLIST "playlist"
#define KEY_FORMAT_VER "format_version"
#define VAL_FORMAT_VER 1
#define KEY_COUNT "count"
#define KEY_SAVE_TIME "save_time"
#define TIME_FORMAT "yyyy-MM-dd HH:mm:ss"
#define KEY_DATA "data"
#define KEY_PLAYLIST_INFO "play_list_info"
#define KEY_PLAYLIST_DATA "play_list_data"

QString PlaylistJson::toJsonString(const Playlist &playlist) {
  return toJsonString(QList<Playlist>{playlist});
}

QString PlaylistJson::toJsonString(const QList<Playlist> &playlist) {
  QJsonDocument doc;
  QJsonObject obj;
  obj.insert(KEY_TYPE, VAL_TYPE_PLAYLIST);
  obj.insert(KEY_FORMAT_VER, VAL_FORMAT_VER);
  obj.insert(KEY_COUNT, playlist.count());
  obj.insert(KEY_SAVE_TIME, QDateTime::currentDateTime().toString(TIME_FORMAT));
  QJsonObject dataObj;
  QList<Playlist>::const_iterator it = playlist.constBegin();
  int pos = 0;
  while (it != playlist.constEnd()) {
    QJsonObject playlistObj;
    // Read info.
    QJsonObject playlistInfoObj;
    playlistInfoObj.insert(PLAYLIST_INFO_NAME,
                           it.i->t().info()->info(PLAYLIST_INFO_NAME));
    playlistInfoObj.insert(PLAYLIST_INFO_COUNT,
                           it.i->t().info()->info(PLAYLIST_INFO_COUNT));
    // Read content.
    PlayContentList::const_iterator itt = it.i->t().content()->constBegin();
    QJsonArray arr;
    while (itt != it.i->t().content()->constEnd()) {
      arr.append(itt.i->t()->value("ContentPath").toString());
      itt++;
    }
    playlistObj.insert(KEY_PLAYLIST_INFO, playlistInfoObj);
    playlistObj.insert(KEY_PLAYLIST_DATA, arr);
    dataObj.insert(QString::number(pos), playlistObj);
    pos++;
    it++;
  }
  obj.insert(KEY_DATA, dataObj);
  doc.setObject(obj);
  return doc.toJson();
}

QList<Playlist> PlaylistJson::fromJsonString(const QString &filePath) {
  QFile file(filePath);
  QTextStream stream;
  stream.setCodec("UTF-8");
  stream.setDevice(&file);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "can not open playlist file.";
    return QList<Playlist>();
  }
  const QString jsonString = stream.readAll();
  file.close();
  QJsonParseError err;
  const QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &err);
  if (doc.isNull()) {
    qDebug() << "can not parse json:" << err.errorString();
    return QList<Playlist>();
  }
  QJsonObject obj = doc.object();
  QJsonObject dataObj = obj.value(KEY_DATA).toObject();
  if (dataObj.isEmpty()) {
    return QList<Playlist>();
  }
  QList<Playlist> allList;
  const QStringList contentKeys = dataObj.keys();
  for (const auto &key : contentKeys) {
    PlaylistInfo *info = new PlaylistInfo;
    PlayContentList *list = new PlayContentList;
    // Read info.
    const QJsonObject playlistInfoObj =
        dataObj.value(key).toObject().value(KEY_PLAYLIST_INFO).toObject();
    info->setInfo(PLAYLIST_INFO_NAME,
                  playlistInfoObj.value(PLAYLIST_INFO_NAME).toString());
    info->setInfo(PLAYLIST_INFO_COUNT,
                  playlistInfoObj.value(PLAYLIST_INFO_COUNT).toString());

    // Read content.
    const QJsonArray arr =
        dataObj.value(key).toObject().value(KEY_PLAYLIST_DATA).toArray();
    for (const auto &a : arr) {
      list->append(new PlayContent(a.toString()));
    }
    allList.append(Playlist(info, list));
  }
  return allList;
}
