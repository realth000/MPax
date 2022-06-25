#include "playlistjson.h"

#include <QtCore/QDateTime>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>

#define KEY_TYPE "type"
#define VAL_TYPE_PLAYLIST "playlist"
#define KEY_FORMAT_VER "format_version"
#define VAL_FORMAT_VER 1
#define KEY_PLAYLIST_COUNT "playlist_count"
#define KEY_SAVE_TIME "save_time"
#define TIME_FORMAT "yyyy-MM-dd HH:mm:ss"
#define KEY_DATA "data"

QString PlaylistJson::toJsonString(const PlayContentList &playlist) {
  return toJsonString(QList<PlayContentList>{playlist});
}

QString PlaylistJson::toJsonString(const QList<PlayContentList> &playlist) {
  QJsonDocument doc;
  QJsonObject obj;
  obj.insert(KEY_TYPE, VAL_TYPE_PLAYLIST);
  obj.insert(KEY_FORMAT_VER, VAL_FORMAT_VER);
  obj.insert(KEY_PLAYLIST_COUNT, playlist.count());
  obj.insert(KEY_SAVE_TIME, QDateTime::currentDateTime().toString(TIME_FORMAT));

  QJsonObject contentObj;
  QList<PlayContentList>::const_iterator it = playlist.constBegin();
  int pos = 0;
  while (it != playlist.constEnd()) {
    PlayContentList::const_iterator itt = it.i->t().constBegin();
    QJsonArray arr;
    while (itt != it.i->t().constEnd()) {
      arr.append(itt.i->t()->value("ContentPath").toString());
    }
    contentObj.insert(QString::number(pos), arr);
    pos++;
  }
  obj.insert(KEY_DATA, contentObj);
  doc.setObject(obj);
  return doc.toJson();
}

QList<PlayContentList> PlaylistJson::fromJsonString(const QString &jsonString) {
  QJsonParseError err;
  const QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &err);
  if (doc.isNull()) {
    qDebug() << "can not parse json:" << err.errorString();
    return QList<PlayContentList>();
  }
  QJsonObject obj = doc.object();
  QJsonObject contentObj = obj.value(KEY_DATA).toObject();
  if (contentObj.isEmpty()) {
    return QList<PlayContentList>();
  }
  QList<PlayContentList> allList;
  const QStringList contentKeys = contentObj.keys();
  for (const auto &key : contentKeys) {
    PlayContentList list;
    const QJsonArray arr = contentObj.value(key).toArray();
    for (const auto &a : arr) {
      list.append(new PlayContent(a.toString()));
    }
    allList.append(list);
  }
  return allList;
}
