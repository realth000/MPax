#ifndef MPAX_PLAYLIST_H
#define MPAX_PLAYLIST_H

#include <QtCore/QMap>

#include "core/playcontent.h"

#define DEFAULT_PLAYLIST_NAME "default"
#define PLAYLIST_INFO_NAME "name"
#define PLAYLIST_INFO_COUNT "count"

class PlaylistInfo {
 public:
  explicit PlaylistInfo();
  explicit PlaylistInfo(const QMap<QString, QString> &info);
  static QMap<QString, QString> defaultInfo();
  const QMap<QString, QString> *info() const;
  QString info(const QString &infoName) const;
  bool setInfo(const QString &infoName, const QString &infoValue);

 private:
  QMap<QString, QString> *m_info;
};

class Playlist {
 public:
  explicit Playlist();
  explicit Playlist(const QMap<QString, QString> &info,
                    PlayContentList *playContentList);
  explicit Playlist(PlaylistInfo *playlistInfo,
                    PlayContentList *playContentList);
  explicit Playlist(const PlaylistInfo &playlistInfo,
                    const PlayContentList &playContentList);
  const PlaylistInfo *info() const;
  const PlayContentList *content() const;

 private:
  PlaylistInfo *m_playListInfo;
  PlayContentList *m_playContentList;
};

#endif  // MPAX_PLAYLIST_H
