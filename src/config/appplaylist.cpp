#include "appplaylist.h"

#include <QtCore/QtDebug>

namespace Config {
void AppPlaylist::savePlaylist(const QString &filePath, const QString &data) {
  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "can not save playlist to" << filePath;
    return;
  }
  QTextStream stream;
  stream.setCodec("UTF-8");
  stream.setDevice(&file);
  stream << data;
  file.close();
}

QList<Playlist> AppPlaylist::loadPlaylist(const QString &filePath) {
  return PlaylistJson::fromJsonString(filePath);
}
}  // namespace Config