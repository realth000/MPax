#ifndef MPAX_APPPLAYLIST_H
#define MPAX_APPPLAYLIST_H

#include <QtCore/QObject>

#include "core/playlistjson.h"

#define CONFIG_PLAYLIST_FILE_PATH "./mpax.list.conf"

namespace Config {

class AppPlaylist : public QObject {
  Q_OBJECT
 public:
  static void savePlaylist(const QString &filePath, const QString &data);
  static QList<Playlist> loadPlaylist(const QString &filePath);
};

}  // namespace Config

#endif  // MPAX_APPPLAYLIST_H
